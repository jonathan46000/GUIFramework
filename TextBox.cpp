#include "TextBox.h"
#include "MiniFB.h"
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <array>

TextBox::TextBox(int x, int y, int width, int height)
    : Widget(x, y, width, height), text(""), cursorPosition(0), textOffset(0),
      isFocused(false), isHovered(false), isSelecting(false),
      selectionStart(-1), selectionEnd(-1),
      backgroundColor(0xFFFFFFFF), focusedBackgroundColor(0xFFFFFFF0),
      borderColor(0xFF888888), focusedBorderColor(0xFF0078D7),
      textColor(MFB_RGB(0, 0, 0)), cursorColor(MFB_RGB(0, 0, 0)),
      selectionColor(0xFF3399FF), changeCallback(nullptr),
      cursorBlinkCounter(0), showCursor(true) {
}

void TextBox::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
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

    if (fontRenderer && !text.empty()) {
        int textX = absX + 5 - textOffset;
        int textY = absY + (height - fontRenderer->getTextHeight()) / 2 + fontRenderer->getTextHeight();

        int clipLeft = absX + 2;
        int clipRight = absX + width - 2;

        int selStart = getSelectionStart();
        int selEnd = getSelectionEnd();

        int currentX = textX;
        for (size_t i = 0; i < text.length(); i++) {
            std::string charStr(1, text[i]);
            int charWidth = fontRenderer->getTextWidth(charStr);

            if (currentX + charWidth > clipLeft && currentX < clipRight) {
                if (hasSelection() && (int)i >= selStart && (int)i < selEnd) {
                    int charEndX = std::min(currentX + charWidth, clipRight);
                    int charStartX = std::max(currentX, clipLeft);

                    for (int py = absY + 2; py < absY + height - 2; py++) {
                        for (int px = charStartX; px < charEndX; px++) {
                            if (px >= 0 && py >= 0 && px < bufferWidth && py < bufferHeight) {
                                buffer[py * bufferWidth + px] = selectionColor;
                            }
                        }
                    }
                }

                fontRenderer->drawText(buffer, bufferWidth, bufferHeight, charStr, currentX, textY, textColor);
            }

            currentX += charWidth;

            if (currentX >= clipRight) {
                break;
            }
        }
    }

    if (isFocused && showCursor && fontRenderer) {
        bool shouldShowCursor = !isSelecting && !hasSelection();

        if (shouldShowCursor) {
            int cursorX = absX + 5 + getCursorPixelPosition() - textOffset;
            int cursorY1 = absY + 5;
            int cursorY2 = absY + height - 5;

            if (cursorX >= absX + 2 && cursorX < absX + width - 2) {
                for (int py = cursorY1; py < cursorY2; py++) {
                    if (cursorX >= 0 && cursorX < bufferWidth && py >= 0 && py < bufferHeight) {
                        buffer[py * bufferWidth + cursorX] = cursorColor;
                    }
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

void TextBox::checkHover(int mouseX, int mouseY) {
    isHovered = containsPoint(mouseX, mouseY);
}

void TextBox::handleChar(unsigned int charCode) {
    if (!isFocused) return;
    if (charCode >= 32 && charCode <= 126) {
        if (hasSelection()) {
            deleteSelection();
        }
        isSelecting = false;
        text.insert(cursorPosition, 1, (char)charCode);
        cursorPosition++;
        adjustTextOffset();
        if (changeCallback) {
            changeCallback(text);
        }
    }
}

void TextBox::handleKey(int key, bool isPressed) {
    if (!isFocused || !isPressed) return;

    isSelecting = false;

    if (key == KB_KEY_BACKSPACE) {
        if (hasSelection()) {
            deleteSelection();
            if (changeCallback) {
                changeCallback(text);
            }
        } else if (cursorPosition > 0) {
            text.erase(cursorPosition - 1, 1);
            cursorPosition--;
            adjustTextOffset();
            if (changeCallback) {
                changeCallback(text);
            }
        }
    } else if (key == KB_KEY_DELETE) {
        if (hasSelection()) {
            deleteSelection();
            if (changeCallback) {
                changeCallback(text);
            }
        } else if (cursorPosition < (int)text.length()) {
            text.erase(cursorPosition, 1);
            if (changeCallback) {
                changeCallback(text);
            }
        }
    } else if (key == KB_KEY_LEFT) {
        if (cursorPosition > 0) {
            cursorPosition--;
            selectionStart = -1;
            selectionEnd = -1;
            adjustTextOffset();
        }
    } else if (key == KB_KEY_RIGHT) {
        if (cursorPosition < (int)text.length()) {
            cursorPosition++;
            selectionStart = -1;
            selectionEnd = -1;
            adjustTextOffset();
        }
    } else if (key == KB_KEY_HOME) {
        cursorPosition = 0;
        selectionStart = -1;
        selectionEnd = -1;
        adjustTextOffset();
    } else if (key == KB_KEY_END) {
        cursorPosition = text.length();
        selectionStart = -1;
        selectionEnd = -1;
        adjustTextOffset();
    }
}

void TextBox::handleMouseButton(int mouseX, int mouseY, bool isPressed) {
    if (!containsPoint(mouseX, mouseY) && !isPressed) {
        if (isSelecting) {
            isSelecting = false;
            if (selectionStart == selectionEnd) {
                selectionStart = -1;
                selectionEnd = -1;
            }
        }
        return;
    }

    if (isPressed) {
        int absX = getAbsoluteX();
        int relativeX = mouseX - absX - 5 + textOffset;
        cursorPosition = getCharacterIndexAtPosition(relativeX);
        selectionStart = cursorPosition;
        selectionEnd = cursorPosition;
        isSelecting = true;
        adjustTextOffset();
    } else {
        isSelecting = false;
        if (selectionStart == selectionEnd) {
            selectionStart = -1;
            selectionEnd = -1;
        }
    }
}

void TextBox::handleMouseMove(int mouseX, int mouseY) {
    (void)mouseY;

    if (isSelecting && isFocused) {
        int absX = getAbsoluteX();
        int relativeX = mouseX - absX - 5 + textOffset;
        int newPos = getCharacterIndexAtPosition(relativeX);

        selectionEnd = newPos;
        cursorPosition = newPos;
        adjustTextOffset();
    }
}

void TextBox::setFocus(bool focused) {
    isFocused = focused;
    if (focused) {
        cursorBlinkCounter = 0;
        showCursor = true;
        isSelecting = false;
    } else {
        isSelecting = false;
    }
}

void TextBox::setChangeCallback(std::function<void(const std::string&)> callback) {
    changeCallback = callback;
}

void TextBox::setText(const std::string& newText) {
    text = newText;
    cursorPosition = text.length();
    selectionStart = -1;
    selectionEnd = -1;
    adjustTextOffset();
    if (changeCallback) {
        changeCallback(text);
    }
}

int TextBox::getCursorPixelPosition() {
    if (!fontRenderer || cursorPosition == 0) {
        return 0;
    }

    std::string textUpToCursor = text.substr(0, cursorPosition);
    return fontRenderer->getTextWidth(textUpToCursor);
}

void TextBox::adjustTextOffset() {
    if (!fontRenderer) return;

    int cursorPixelPos = getCursorPixelPosition();
    int visibleWidth = width - 10;

    if (cursorPixelPos - textOffset > visibleWidth) {
        textOffset = cursorPixelPos - visibleWidth;
    }

    if (cursorPixelPos - textOffset < 0) {
        textOffset = cursorPixelPos;
    }

    if (textOffset < 0) {
        textOffset = 0;
    }
}

int TextBox::getCharacterIndexAtPosition(int pixelX) {
    if (!fontRenderer || text.empty()) {
        return 0;
    }

    int currentX = 0;
    for (size_t i = 0; i < text.length(); i++) {
        std::string charStr(1, text[i]);
        int charWidth = fontRenderer->getTextWidth(charStr);

        if (pixelX < currentX + charWidth / 2) {
            return i;
        }

        currentX += charWidth;
    }

    return text.length();
}

void TextBox::deleteSelection() {
    if (!hasSelection()) return;

    int selStart = getSelectionStart();
    int selEnd = getSelectionEnd();

    text.erase(selStart, selEnd - selStart);
    cursorPosition = selStart;
    selectionStart = -1;
    selectionEnd = -1;
    adjustTextOffset();
}

std::string TextBox::getSelectedText() const {
    if (!hasSelection()) return "";

    int selStart = getSelectionStart();
    int selEnd = getSelectionEnd();

    return text.substr(selStart, selEnd - selStart);
}

int TextBox::getSelectionStart() const {
    if (selectionStart == -1 || selectionEnd == -1) return -1;
    return std::min(selectionStart, selectionEnd);
}

int TextBox::getSelectionEnd() const {
    if (selectionStart == -1 || selectionEnd == -1) return -1;
    return std::max(selectionStart, selectionEnd);
}

bool TextBox::hasSelection() const {
    return selectionStart != -1 && selectionEnd != -1 && selectionStart != selectionEnd;
}

void TextBox::copy() {
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

void TextBox::cut() {
    if (!hasSelection()) return;

    copy();
    deleteSelection();

    if (changeCallback) {
        changeCallback(text);
    }
}

void TextBox::paste() {
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

                text.insert(cursorPosition, pastedText);
                cursorPosition += pastedText.length();
                adjustTextOffset();

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

            text.insert(cursorPosition, pastedText);
            cursorPosition += pastedText.length();
            adjustTextOffset();

            if (changeCallback) {
                changeCallback(text);
            }
        }
    }
}

void TextBox::selectAll() {
    if (text.empty()) return;

    selectionStart = 0;
    selectionEnd = text.length();
    cursorPosition = text.length();
    adjustTextOffset();
}

void TextBox::stopSelecting() {
    isSelecting = false;
}
