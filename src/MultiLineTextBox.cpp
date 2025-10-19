#include "MultiLineTextBox.h"
#include "MiniFB.h"
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <array>

MultiLineTextBox::MultiLineTextBox(int x, int y, int width, int height)
    : Widget(x, y, width, height), text(""), cursorLine(0), cursorColumn(0),
      scrollOffset(0), isFocused(false), isHovered(false), isSelecting(false),
      selectionStartLine(-1), selectionStartColumn(-1),
      selectionEndLine(-1), selectionEndColumn(-1),
      backgroundColor(0xFFFFFFFF), focusedBackgroundColor(0xFFFFFFF0),
      borderColor(0xFF888888), focusedBorderColor(0xFF0078D7),
      textColor(MFB_RGB(0, 0, 0)), cursorColor(MFB_RGB(0, 0, 0)),
      selectionColor(0xFF3399FF),
      changeCallback(nullptr), cursorBlinkCounter(0), showCursor(true),
      visibleLines(0), lineHeight(20) {
    lines.push_back("");
    isWrappedLine.push_back(false);
}

void MultiLineTextBox::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int endX = std::min(absX + width, bufferWidth);
    int endY = std::min(absY + height, bufferHeight);

    uint32_t bgColor = isFocused ? focusedBackgroundColor : backgroundColor;
    uint32_t bColor = isFocused ? focusedBorderColor : borderColor;

    for (int py = absY; py < endY; py++) {
        for (int px = absX; px < endX; px++) {
            if (px >= 0 && py >= 0) {
                if (py == absY || py == endY - 1 || px == absX || px == endX - 1) {
                    buffer[py * bufferWidth + px] = bColor;
                } else {
                    buffer[py * bufferWidth + px] = bgColor;
                }
            }
        }
    }

    if (fontRenderer) {
        lineHeight = fontRenderer->getTextHeight() + 4;
        visibleLines = (height - 4) / lineHeight;

        int textX = absX + 5;
        int textY = absY + 5;

        int selStartLine, selStartCol, selEndLine, selEndCol;
        bool hasSel = hasSelection();
        if (hasSel) {
            getSelectionBounds(selStartLine, selStartCol, selEndLine, selEndCol);
        }

        for (int i = scrollOffset; i < (int)lines.size() && i < scrollOffset + visibleLines; i++) {
            int lineY = textY + (i - scrollOffset) * lineHeight;
            int lineTextY = lineY + fontRenderer->getTextHeight();

            if (hasSel && i >= selStartLine && i <= selEndLine) {
                int startCol = (i == selStartLine) ? selStartCol : 0;
                int endCol = (i == selEndLine) ? selEndCol : lines[i].length();

                std::string beforeSel = lines[i].substr(0, startCol);
                std::string selection = lines[i].substr(startCol, endCol - startCol);

                int selStartX = textX + fontRenderer->getTextWidth(beforeSel);
                int selWidth = fontRenderer->getTextWidth(selection);

                for (int py = lineY; py < lineY + fontRenderer->getTextHeight() && py < endY; py++) {
                    for (int px = selStartX; px < selStartX + selWidth && px < endX; px++) {
                        if (px >= absX + 2 && py >= absY + 2) {
                            buffer[py * bufferWidth + px] = selectionColor;
                        }
                    }
                }
            }

            fontRenderer->drawText(buffer, bufferWidth, bufferHeight, lines[i], textX, lineTextY, textColor);
        }

        if (isFocused && showCursor && !hasSelection() && cursorLine >= scrollOffset && cursorLine < scrollOffset + visibleLines) {
            cursorColumn = std::min(cursorColumn, (int)lines[cursorLine].length());
            std::string textBeforeCursor = lines[cursorLine].substr(0, cursorColumn);
            int cursorX = textX + fontRenderer->getTextWidth(textBeforeCursor);
            int cursorYTop = textY + (cursorLine - scrollOffset) * lineHeight;
            int cursorYBottom = cursorYTop + fontRenderer->getTextHeight();

            for (int py = cursorYTop; py < cursorYBottom && py < endY; py++) {
                if (cursorX >= absX + 2 && cursorX < endX - 2 && py >= absY + 2) {
                    buffer[py * bufferWidth + cursorX] = cursorColor;
                }
            }
        }
    }

    cursorBlinkCounter++;
    if (cursorBlinkCounter > 30) {
        showCursor = !showCursor;
        cursorBlinkCounter = 0;
    }
}

void MultiLineTextBox::checkHover(int mouseX, int mouseY) {
    isHovered = containsPoint(mouseX, mouseY);
}

void MultiLineTextBox::handleChar(unsigned int charCode) {
    if (!isFocused) return;
    if (charCode >= 32 && charCode <= 126) {
        if (hasSelection()) {
            deleteSelection();
        }
        insertCharAtCursor((char)charCode);
        checkAndWrapCurrentLine();
        rebuildTextFromLines();
        if (changeCallback) {
            changeCallback(text);
        }
    }
}

void MultiLineTextBox::handleKey(int key, bool isPressed) {
    if (!isFocused || !isPressed) return;

    bool shiftPressed = false;

    if (key == KB_KEY_BACKSPACE) {
        if (hasSelection()) {
            deleteSelection();
            rebuildTextFromLines();
            updateScrollOffset();
            if (changeCallback) {
                changeCallback(text);
            }
        } else if (cursorColumn > 0) {
            cursorColumn--;
            deleteCharAtCursor();
            checkAndWrapCurrentLine();
            rebuildTextFromLines();
            updateScrollOffset();
            if (changeCallback) {
                changeCallback(text);
            }
        } else if (cursorLine > 0) {
            int prevLineLength = lines[cursorLine - 1].length();
            lines[cursorLine - 1] += lines[cursorLine];
            lines.erase(lines.begin() + cursorLine);
            isWrappedLine.erase(isWrappedLine.begin() + cursorLine);
            cursorLine--;
            cursorColumn = prevLineLength;
            checkAndWrapCurrentLine();
            rebuildTextFromLines();
            updateScrollOffset();
            if (changeCallback) {
                changeCallback(text);
            }
        }
    } else if (key == KB_KEY_DELETE) {
        if (hasSelection()) {
            deleteSelection();
            rebuildTextFromLines();
            if (changeCallback) {
                changeCallback(text);
            }
        } else if (cursorColumn < (int)lines[cursorLine].length()) {
            deleteCharAtCursor();
            checkAndWrapCurrentLine();
            rebuildTextFromLines();
            if (changeCallback) {
                changeCallback(text);
            }
        } else if (cursorLine < (int)lines.size() - 1) {
            lines[cursorLine] += lines[cursorLine + 1];
            lines.erase(lines.begin() + cursorLine + 1);
            isWrappedLine.erase(isWrappedLine.begin() + cursorLine + 1);
            checkAndWrapCurrentLine();
            rebuildTextFromLines();
            if (changeCallback) {
                changeCallback(text);
            }
        }
    } else if (key == KB_KEY_ENTER) {
        if (hasSelection()) {
            deleteSelection();
        }
        std::string remainingText = lines[cursorLine].substr(cursorColumn);
        lines[cursorLine] = lines[cursorLine].substr(0, cursorColumn);
        cursorLine++;
        cursorColumn = 0;
        lines.insert(lines.begin() + cursorLine, remainingText);
        isWrappedLine.insert(isWrappedLine.begin() + cursorLine, false);
        rebuildTextFromLines();
        updateScrollOffset();
        if (changeCallback) {
            changeCallback(text);
        }
    } else if (key == KB_KEY_LEFT) {
        moveCursorLeft(shiftPressed);
    } else if (key == KB_KEY_RIGHT) {
        moveCursorRight(shiftPressed);
    } else if (key == KB_KEY_UP) {
        moveCursorUp(shiftPressed);
    } else if (key == KB_KEY_DOWN) {
        moveCursorDown(shiftPressed);
    } else if (key == KB_KEY_HOME) {
        moveCursorToLineStart(shiftPressed);
    } else if (key == KB_KEY_END) {
        moveCursorToLineEnd(shiftPressed);
    }
}

void MultiLineTextBox::handleMouseButton(int mouseX, int mouseY, bool isPressed) {
    if (!containsPoint(mouseX, mouseY) && !isPressed) {
        if (isSelecting) {
            isSelecting = false;
        }
        return;
    }

    if (isPressed && containsPoint(mouseX, mouseY)) {
        int absX = getAbsoluteX();
        int absY = getAbsoluteY();
        int relativeY = mouseY - absY - 5;
        int clickedLine = scrollOffset + (relativeY / lineHeight);

        if (clickedLine >= 0 && clickedLine < (int)lines.size()) {
            cursorLine = clickedLine;
            int relativeX = mouseX - absX - 5;
            cursorColumn = getCharIndexFromPosition(cursorLine, relativeX);

            selectionStartLine = cursorLine;
            selectionStartColumn = cursorColumn;
            selectionEndLine = cursorLine;
            selectionEndColumn = cursorColumn;
            isSelecting = true;
        }
    } else {
        isSelecting = false;
    }
}

void MultiLineTextBox::handleMouseMove(int mouseX, int mouseY) {
    if (isSelecting && isFocused) {
        int absX = getAbsoluteX();
        int absY = getAbsoluteY();
        int relativeY = mouseY - absY - 5;
        int hoveredLine = scrollOffset + (relativeY / lineHeight);

        if (hoveredLine >= 0 && hoveredLine < (int)lines.size()) {
            int relativeX = mouseX - absX - 5;
            int hoveredColumn = getCharIndexFromPosition(hoveredLine, relativeX);

            cursorLine = hoveredLine;
            cursorColumn = hoveredColumn;
            selectionEndLine = hoveredLine;
            selectionEndColumn = hoveredColumn;
        }
    }
}

void MultiLineTextBox::setFocus(bool focused) {
    isFocused = focused;
    if (focused) {
        cursorBlinkCounter = 0;
        showCursor = true;
    } else {
        isSelecting = false;
    }
}

void MultiLineTextBox::setChangeCallback(std::function<void(const std::string&)> callback) {
    changeCallback = callback;
}

void MultiLineTextBox::setText(const std::string& newText) {
    text = newText;
    wrapText();
    cursorLine = 0;
    cursorColumn = 0;
    scrollOffset = 0;
    clearSelection();
    if (changeCallback) {
        changeCallback(text);
    }
}

void MultiLineTextBox::wrapText() {
    lines.clear();
    isWrappedLine.clear();
    if (text.empty()) {
        lines.push_back("");
        isWrappedLine.push_back(false);
        return;
    }

    if (!fontRenderer) {
        lines.push_back(text);
        isWrappedLine.push_back(false);
        return;
    }

    int maxWidth = width - 10;
    std::string currentLine = "";
    std::string currentWord = "";

    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];

        if (c == '\n') {
            currentLine += currentWord;
            lines.push_back(currentLine);
            isWrappedLine.push_back(false);
            currentLine = "";
            currentWord = "";
        } else if (c == ' ') {
            std::string testLine = currentLine + currentWord + " ";
            int testWidth = fontRenderer->getTextWidth(testLine);

            if (testWidth > maxWidth && !currentLine.empty()) {
                lines.push_back(currentLine);
                isWrappedLine.push_back(false);
                currentLine = currentWord + " ";
            } else {
                currentLine += currentWord + " ";
            }
            currentWord = "";
        } else {
            currentWord += c;
        }
    }

    if (!currentWord.empty()) {
        std::string testLine = currentLine + currentWord;
        int testWidth = fontRenderer->getTextWidth(testLine);

        if (testWidth > maxWidth && !currentLine.empty()) {
            lines.push_back(currentLine);
            isWrappedLine.push_back(false);
            lines.push_back(currentWord);
            isWrappedLine.push_back(true);
        } else {
            currentLine += currentWord;
            lines.push_back(currentLine);
            isWrappedLine.push_back(false);
        }
    } else if (!currentLine.empty()) {
        lines.push_back(currentLine);
        isWrappedLine.push_back(false);
    }

    if (lines.empty()) {
        lines.push_back("");
        isWrappedLine.push_back(false);
    }
}

void MultiLineTextBox::checkAndWrapCurrentLine() {
    if (!fontRenderer) return;
    if (cursorLine >= (int)lines.size()) return;

    int maxWidth = width - 10;

    while (cursorLine < (int)lines.size()) {
        int lineWidth = fontRenderer->getTextWidth(lines[cursorLine]);

        if (lineWidth <= maxWidth) {
            break;
        }

        int splitPos = lines[cursorLine].length();
        for (int i = lines[cursorLine].length() - 1; i > 0; i--) {
            if (lines[cursorLine][i] == ' ') {
                std::string testStr = lines[cursorLine].substr(0, i);
                if (fontRenderer->getTextWidth(testStr) <= maxWidth) {
                    splitPos = i;
                    break;
                }
            }
        }

        if (splitPos == (int)lines[cursorLine].length()) {
            for (int i = lines[cursorLine].length() - 1; i > 0; i--) {
                std::string testStr = lines[cursorLine].substr(0, i);
                if (fontRenderer->getTextWidth(testStr) <= maxWidth) {
                    splitPos = i;
                    break;
                }
            }
        }

        if (splitPos > 0 && splitPos < (int)lines[cursorLine].length()) {
            std::string remainingText = lines[cursorLine].substr(splitPos);
            if (!remainingText.empty() && remainingText[0] == ' ') {
                remainingText = remainingText.substr(1);
            }
            lines[cursorLine] = lines[cursorLine].substr(0, splitPos);

            if (cursorColumn > splitPos) {
                int offset = splitPos;
                if (!lines[cursorLine].empty() && lines[cursorLine][lines[cursorLine].length() - 1] == ' ') {
                    offset++;
                }
                cursorColumn -= offset;
                cursorLine++;
                lines.insert(lines.begin() + cursorLine, remainingText);
                isWrappedLine.insert(isWrappedLine.begin() + cursorLine, true);
            } else {
                lines.insert(lines.begin() + cursorLine + 1, remainingText);
                isWrappedLine.insert(isWrappedLine.begin() + cursorLine + 1, true);
                break;
            }
        } else {
            break;
        }
    }

    if (cursorLine >= (int)lines.size()) {
        cursorLine = lines.size() - 1;
    }
    cursorColumn = std::min(cursorColumn, (int)lines[cursorLine].length());
}

void MultiLineTextBox::insertCharAtCursor(char c) {
    if (cursorLine >= (int)lines.size()) {
        lines.push_back("");
        isWrappedLine.push_back(false);
        cursorLine = lines.size() - 1;
    }

    cursorColumn = std::min(cursorColumn, (int)lines[cursorLine].length());
    lines[cursorLine].insert(cursorColumn, 1, c);
    cursorColumn++;
}

void MultiLineTextBox::deleteCharAtCursor() {
    if (cursorLine >= (int)lines.size()) {
        return;
    }

    cursorColumn = std::min(cursorColumn, (int)lines[cursorLine].length());

    if (cursorColumn < (int)lines[cursorLine].length()) {
        lines[cursorLine].erase(cursorColumn, 1);
    }
}

void MultiLineTextBox::moveCursorLeft(bool shift) {
    if (!shift) clearSelection();

    if (cursorColumn > 0) {
        cursorColumn--;
    } else if (cursorLine > 0) {
        cursorLine--;
        cursorColumn = lines[cursorLine].length();
        updateScrollOffset();
    }

    if (shift) updateSelection();
}

void MultiLineTextBox::moveCursorRight(bool shift) {
    if (!shift) clearSelection();

    if (cursorColumn < (int)lines[cursorLine].length()) {
        cursorColumn++;
    } else if (cursorLine < (int)lines.size() - 1) {
        cursorLine++;
        cursorColumn = 0;
        updateScrollOffset();
    }

    if (shift) updateSelection();
}

void MultiLineTextBox::moveCursorUp(bool shift) {
    if (!shift) clearSelection();

    if (cursorLine > 0) {
        cursorLine--;
        cursorColumn = std::min(cursorColumn, (int)lines[cursorLine].length());
        updateScrollOffset();
    }

    if (shift) updateSelection();
}

void MultiLineTextBox::moveCursorDown(bool shift) {
    if (!shift) clearSelection();

    if (cursorLine < (int)lines.size() - 1) {
        cursorLine++;
        cursorColumn = std::min(cursorColumn, (int)lines[cursorLine].length());
        updateScrollOffset();
    }

    if (shift) updateSelection();
}

void MultiLineTextBox::moveCursorToLineStart(bool shift) {
    if (!shift) clearSelection();

    cursorColumn = 0;

    if (shift) updateSelection();
}

void MultiLineTextBox::moveCursorToLineEnd(bool shift) {
    if (!shift) clearSelection();

    cursorColumn = lines[cursorLine].length();

    if (shift) updateSelection();
}

void MultiLineTextBox::updateScrollOffset() {
    if (cursorLine < scrollOffset) {
        scrollOffset = cursorLine;
    } else if (cursorLine >= scrollOffset + visibleLines) {
        scrollOffset = cursorLine - visibleLines + 1;
    }
}

void MultiLineTextBox::rebuildTextFromLines() {
    text = "";
    for (size_t i = 0; i < lines.size(); i++) {
        text += lines[i];
        if (i < lines.size() - 1) {
            text += "\n";
        }
    }
}

int MultiLineTextBox::getCharIndexFromPosition(int lineIndex, int pixelX) {
    if (!fontRenderer || lineIndex < 0 || lineIndex >= (int)lines.size()) {
        return 0;
    }

    const std::string& line = lines[lineIndex];
    int currentX = 0;

    for (size_t i = 0; i < line.length(); i++) {
        std::string charStr(1, line[i]);
        int charWidth = fontRenderer->getTextWidth(charStr);

        if (pixelX < currentX + charWidth / 2) {
            return i;
        }

        currentX += charWidth;
    }

    return line.length();
}

void MultiLineTextBox::clearSelection() {
    selectionStartLine = -1;
    selectionStartColumn = -1;
    selectionEndLine = -1;
    selectionEndColumn = -1;
}

void MultiLineTextBox::updateSelection() {
    if (selectionStartLine == -1) {
        selectionStartLine = cursorLine;
        selectionStartColumn = cursorColumn;
    }
    selectionEndLine = cursorLine;
    selectionEndColumn = cursorColumn;
}

bool MultiLineTextBox::hasSelection() const {
    return selectionStartLine != -1 && selectionEndLine != -1 &&
           !(selectionStartLine == selectionEndLine && selectionStartColumn == selectionEndColumn);
}

void MultiLineTextBox::deleteSelection() {
    if (!hasSelection()) return;

    int startLine, startCol, endLine, endCol;
    getSelectionBounds(startLine, startCol, endLine, endCol);

    if (startLine == endLine) {
        lines[startLine].erase(startCol, endCol - startCol);
    } else {
        lines[startLine] = lines[startLine].substr(0, startCol) + lines[endLine].substr(endCol);
        lines.erase(lines.begin() + startLine + 1, lines.begin() + endLine + 1);
        isWrappedLine.erase(isWrappedLine.begin() + startLine + 1, isWrappedLine.begin() + endLine + 1);
    }

    cursorLine = startLine;
    cursorColumn = startCol;
    clearSelection();
}

std::string MultiLineTextBox::getSelectedText() const {
    if (!hasSelection()) return "";

    int startLine, startCol, endLine, endCol;
    getSelectionBounds(startLine, startCol, endLine, endCol);

    std::string result;
    if (startLine == endLine) {
        result = lines[startLine].substr(startCol, endCol - startCol);
    } else {
        result = lines[startLine].substr(startCol);

        if (fontRenderer && startLine + 1 < (int)lines.size()) {
            int maxWidth = width - 10;
            std::string nextLine = lines[startLine + 1];
            std::string firstWord = "";
            for (size_t j = 0; j < nextLine.length(); j++) {
                if (nextLine[j] == ' ') break;
                firstWord += nextLine[j];
            }

            int testWidth = fontRenderer->getTextWidth(lines[startLine] + " " + firstWord);
            if (testWidth <= maxWidth) {
                result += "\n";
            }
        } else if (startLine + 1 >= (int)lines.size()) {
            result += "\n";
        }

        for (int i = startLine + 1; i < endLine; i++) {
            result += lines[i];

            if (fontRenderer && i + 1 < (int)lines.size()) {
                int maxWidth = width - 10;
                std::string nextLine = lines[i + 1];
                std::string firstWord = "";
                for (size_t j = 0; j < nextLine.length(); j++) {
                    if (nextLine[j] == ' ') break;
                    firstWord += nextLine[j];
                }

                int testWidth = fontRenderer->getTextWidth(lines[i] + " " + firstWord);
                if (testWidth <= maxWidth) {
                    result += "\n";
                }
            } else if (i + 1 >= (int)lines.size()) {
                result += "\n";
            }
        }
        result += lines[endLine].substr(0, endCol);
    }

    return result;
}

void MultiLineTextBox::getSelectionBounds(int& startLine, int& startCol, int& endLine, int& endCol) const {
    if (selectionStartLine < selectionEndLine ||
        (selectionStartLine == selectionEndLine && selectionStartColumn <= selectionEndColumn)) {
        startLine = selectionStartLine;
        startCol = selectionStartColumn;
        endLine = selectionEndLine;
        endCol = selectionEndColumn;
    } else {
        startLine = selectionEndLine;
        startCol = selectionEndColumn;
        endLine = selectionStartLine;
        endCol = selectionStartColumn;
    }
}

void MultiLineTextBox::copy() {
    if (!hasSelection()) return;

    std::string selectedText = getSelectedText();

    const char* waylandDisplay = std::getenv("WAYLAND_DISPLAY");
    if (waylandDisplay != nullptr) {
        FILE* pipe = popen("wl-copy", "w");
        if (pipe) {
            fwrite(selectedText.c_str(), 1, selectedText.length(), pipe);
            pclose(pipe);
            return;
        }
    }

    FILE* pipe = popen("xclip -selection clipboard", "w");
    if (pipe) {
        fwrite(selectedText.c_str(), 1, selectedText.length(), pipe);
        pclose(pipe);
    }
}

void MultiLineTextBox::cut() {
    if (!hasSelection()) return;

    copy();
    deleteSelection();
    rebuildTextFromLines();

    if (changeCallback) {
        changeCallback(text);
    }
}

void MultiLineTextBox::paste() {
    std::string pastedText;

    const char* waylandDisplay = std::getenv("WAYLAND_DISPLAY");
    if (waylandDisplay != nullptr) {
        FILE* pipe = popen("wl-paste", "r");
        if (pipe) {
            std::array<char, 128> buffer;
            while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
                pastedText += buffer.data();
            }
            pclose(pipe);

            if (!pastedText.empty()) {
                if (hasSelection()) {
                    deleteSelection();
                }

                for (char c : pastedText) {
                    if (c == '\n') {
                        std::string remainingText = lines[cursorLine].substr(cursorColumn);
                        lines[cursorLine] = lines[cursorLine].substr(0, cursorColumn);
                        cursorLine++;
                        cursorColumn = 0;
                        lines.insert(lines.begin() + cursorLine, remainingText);
                        isWrappedLine.insert(isWrappedLine.begin() + cursorLine, false);
                    } else if (c >= 32 && c <= 126) {
                        insertCharAtCursor(c);
                        checkAndWrapCurrentLine();
                    }
                }

                rebuildTextFromLines();
                updateScrollOffset();

                if (changeCallback) {
                    changeCallback(text);
                }
                return;
            }
        }
    }

    FILE* pipe = popen("xclip -selection clipboard -o", "r");
    if (pipe) {
        std::array<char, 128> buffer;
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            pastedText += buffer.data();
        }
        pclose(pipe);

        if (!pastedText.empty()) {
            if (hasSelection()) {
                deleteSelection();
            }

            for (char c : pastedText) {
                if (c == '\n') {
                    std::string remainingText = lines[cursorLine].substr(cursorColumn);
                    lines[cursorLine] = lines[cursorLine].substr(0, cursorColumn);
                    cursorLine++;
                    cursorColumn = 0;
                    lines.insert(lines.begin() + cursorLine, remainingText);
                    isWrappedLine.insert(isWrappedLine.begin() + cursorLine, false);
                } else if (c >= 32 && c <= 126) {
                    insertCharAtCursor(c);
                    checkAndWrapCurrentLine();
                }
            }

            rebuildTextFromLines();
            updateScrollOffset();

            if (changeCallback) {
                changeCallback(text);
            }
        }
    }
}

void MultiLineTextBox::selectAll() {
    if (lines.empty()) return;

    selectionStartLine = 0;
    selectionStartColumn = 0;
    selectionEndLine = lines.size() - 1;
    selectionEndColumn = lines[lines.size() - 1].length();
    cursorLine = selectionEndLine;
    cursorColumn = selectionEndColumn;
}
