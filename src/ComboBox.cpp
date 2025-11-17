#include "ComboBox.h"
#include "MiniFB.h"
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <iostream>

ComboBox::ComboBox(int x, int y, int width, int height)
    : Widget(x, y, width, height), text(""), isOpen(false), isFocused(false), isHovered(false),
      cursorPosition(0), textOffset(0), isSelecting(false),
      selectionStart(-1), selectionEnd(-1), hoveredItemIndex(-1), selectedItemIndex(-1),
      backgroundColor(0xFFFFFFFF), focusedBackgroundColor(0xFFFFFFF0),
      borderColor(0xFF888888), focusedBorderColor(0xFF0078D7),
      textColor(MFB_RGB(0, 0, 0)), cursorColor(MFB_RGB(0, 0, 0)),
      selectionColor(0xFF3399FF), menuBackgroundColor(0xFFFFFFFF),
      menuBorderColor(0xFF808080), menuHoverColor(0xFFE0E0FF),
      buttonWidth(20), maxVisibleItems(8), scrollOffset(0),
      changeCallback(nullptr), selectionCallback(nullptr),
      cursorBlinkCounter(0), showCursor(true) {
}

void ComboBox::addItem(const std::string& item) {
    items.push_back(item);
}

void ComboBox::clearItems() {
    items.clear();
    scrollOffset = 0;
    selectedItemIndex = -1;
}

void ComboBox::setSelectedIndex(int index) {
    if (index >= -1 && index < (int)items.size()) {
        selectedItemIndex = index;
        if (index >= 0) {
            text = items[index];
            cursorPosition = text.length();
            adjustTextOffset();
            if (changeCallback) {
                changeCallback(text);
            }
        }
    }
}

void ComboBox::setText(const std::string& newText) {
    text = newText;
    cursorPosition = std::min(cursorPosition, (int)text.length());
    adjustTextOffset();
}

void ComboBox::adjustTextOffset() {
    if (!fontRenderer) return;

    int visibleWidth = width - 10 - buttonWidth;
    int cursorPixelPos = getCursorPixelPosition();

    if (cursorPixelPos - textOffset > visibleWidth - 5) {
        textOffset = cursorPixelPos - visibleWidth + 5;
    }

    if (cursorPixelPos - textOffset < 5) {
        textOffset = std::max(0, cursorPixelPos - 5);
    }
}

int ComboBox::getCursorPixelPosition() {
    if (!fontRenderer || cursorPosition == 0) return 0;

    std::string textToCursor = text.substr(0, cursorPosition);
    return fontRenderer->getTextWidth(textToCursor);
}

int ComboBox::getCharacterIndexAtPosition(int pixelX) {
    if (!fontRenderer) return 0;

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

void ComboBox::deleteSelection() {
    if (!hasSelection()) return;

    int selStart = std::min(selectionStart, selectionEnd);
    int selEnd = std::max(selectionStart, selectionEnd);

    text = text.substr(0, selStart) + text.substr(selEnd);
    cursorPosition = selStart;
    selectionStart = -1;
    selectionEnd = -1;
    adjustTextOffset();
}

int ComboBox::getMenuHeight() const {
    int visibleCount = std::min((int)items.size(), maxVisibleItems);
    return visibleCount * 25 + 2;
}

int ComboBox::getItemAtPosition(int mouseX, int mouseY) {
    if (items.empty()) return -1;

    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int menuY = absY + height;
    int menuHeight = getMenuHeight();

    if (mouseX < absX || mouseX >= absX + width ||
        mouseY < menuY || mouseY >= menuY + menuHeight) {
        return -1;
    }

    int relativeY = mouseY - menuY - 1;
    int itemIndex = scrollOffset + (relativeY / 25);

    if (itemIndex >= 0 && itemIndex < (int)items.size()) {
        return itemIndex;
    }
    return -1;
}

void ComboBox::drawDropdownButton(uint32_t* buffer, int bufferWidth, int bufferHeight, int buttonX, int buttonY) {
    int buttonEndX = std::min(buttonX + buttonWidth, bufferWidth);
    int buttonEndY = std::min(buttonY + height, bufferHeight);

    uint32_t btnColor = isHovered ? 0xFFD0D0D0 : 0xFFE0E0E0;

    for (int py = buttonY; py < buttonEndY; py++) {
        for (int px = buttonX; px < buttonEndX; px++) {
            if (px >= 0 && py >= 0) {
                if (px == buttonX || px == buttonEndX - 1 || py == buttonY || py == buttonEndY - 1) {
                    buffer[py * bufferWidth + px] = borderColor;
                } else {
                    buffer[py * bufferWidth + px] = btnColor;
                }
            }
        }
    }

    // Draw arrow
    int arrowCenterX = buttonX + buttonWidth / 2;
    int arrowCenterY = buttonY + height / 2;
    int arrowSize = 4;

    for (int i = 0; i < arrowSize; i++) {
        for (int j = -i; j <= i; j++) {
            int px = arrowCenterX + j;
            int py = arrowCenterY + i - arrowSize / 2;
            if (px >= buttonX && px < buttonEndX && py >= buttonY && py < buttonEndY &&
                px >= 0 && py >= 0 && px < bufferWidth && py < bufferHeight) {
                buffer[py * bufferWidth + px] = textColor;
            }
        }
    }
}

void ComboBox::drawDropdownMenu(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    if (items.empty()) return;

    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int menuY = absY + height;
    int menuHeight = getMenuHeight();
    int menuEndX = std::min(absX + width, bufferWidth);
    int menuEndY = std::min(menuY + menuHeight, bufferHeight);

    // Draw menu background and border
    for (int py = menuY; py < menuEndY; py++) {
        for (int px = absX; px < menuEndX; px++) {
            if (px >= 0 && py >= 0) {
                if (py == menuY || py == menuEndY - 1 || px == absX || px == menuEndX - 1) {
                    buffer[py * bufferWidth + px] = menuBorderColor;
                } else {
                    buffer[py * bufferWidth + px] = menuBackgroundColor;
                }
            }
        }
    }

    // Draw items
    if (fontRenderer) {
        int visibleCount = std::min((int)items.size() - scrollOffset, maxVisibleItems);
        for (int i = 0; i < visibleCount; i++) {
            int itemIndex = scrollOffset + i;
            int itemY = menuY + 1 + i * 25;

            // Highlight hovered or selected item
            if (itemIndex == hoveredItemIndex || itemIndex == selectedItemIndex) {
                uint32_t highlightColor = (itemIndex == hoveredItemIndex) ? menuHoverColor : 0xFFF0F0F0;
                for (int py = itemY; py < std::min(itemY + 25, menuEndY - 1); py++) {
                    for (int px = absX + 1; px < menuEndX - 1; px++) {
                        if (px >= 0 && py >= 0 && px < bufferWidth && py < bufferHeight) {
                            buffer[py * bufferWidth + px] = highlightColor;
                        }
                    }
                }
            }

            // Draw item text
            int textY = itemY + 20;
            fontRenderer->drawText(buffer, bufferWidth, bufferHeight,
                                 items[itemIndex], absX + 5, textY, textColor);
        }
    }
}

void ComboBox::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int endX = std::min(absX + width - buttonWidth, bufferWidth);
    int endY = std::min(absY + height, bufferHeight);

    uint32_t bgColor = isFocused ? focusedBackgroundColor : backgroundColor;
    uint32_t bColor = isFocused ? focusedBorderColor : borderColor;

    // Draw text input area
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

    // Draw text with selection
    if (fontRenderer && !text.empty()) {
        int textX = absX + 5 - textOffset;
        int textY = absY + (height - fontRenderer->getTextHeight()) / 2 + fontRenderer->getTextHeight();
        int clipLeft = absX + 2;
        int clipRight = absX + width - buttonWidth - 2;

        int selStart = std::min(selectionStart, selectionEnd);
        int selEnd = std::max(selectionStart, selectionEnd);

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
            if (currentX >= clipRight) break;
        }
    }

    // Draw cursor
    if (isFocused && showCursor && fontRenderer) {
        bool shouldShowCursor = !isSelecting && !hasSelection();

        if (shouldShowCursor) {
            int cursorX = absX + 5 + getCursorPixelPosition() - textOffset;
            int cursorY1 = absY + 5;
            int cursorY2 = absY + height - 5;

            if (cursorX >= absX + 2 && cursorX < absX + width - buttonWidth - 2) {
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

    // Draw dropdown button
    drawDropdownButton(buffer, bufferWidth, bufferHeight, absX + width - buttonWidth, absY);
}

void ComboBox::checkHover(int mouseX, int mouseY) {
    isHovered = checkClick(mouseX, mouseY);
}

void ComboBox::handleChar(unsigned int charCode) {
    if (!isFocused) return;

    if (charCode >= 32 && charCode <= 126) {
        if (hasSelection()) {
            deleteSelection();
        }

        text.insert(cursorPosition, 1, (char)charCode);
        cursorPosition++;
        adjustTextOffset();

        if (changeCallback) {
            changeCallback(text);
        }
    }
}

void ComboBox::handleKey(int key, bool isPressed) {
    if (!isPressed || !isFocused) return;

    if (key == KB_KEY_LEFT) {
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
    } else if (key == KB_KEY_BACKSPACE) {
        if (hasSelection()) {
            deleteSelection();
        } else if (cursorPosition > 0) {
            text.erase(cursorPosition - 1, 1);
            cursorPosition--;
        }
        adjustTextOffset();
        if (changeCallback) {
            changeCallback(text);
        }
    } else if (key == KB_KEY_DELETE) {
        if (hasSelection()) {
            deleteSelection();
        } else if (cursorPosition < (int)text.length()) {
            text.erase(cursorPosition, 1);
        }
        adjustTextOffset();
        if (changeCallback) {
            changeCallback(text);
        }
    } else if (key == KB_KEY_DOWN && isOpen) {
        if (hoveredItemIndex < (int)items.size() - 1) {
            hoveredItemIndex++;
            if (hoveredItemIndex >= scrollOffset + maxVisibleItems) {
                scrollOffset++;
            }
        }
    } else if (key == KB_KEY_UP && isOpen) {
        if (hoveredItemIndex > 0) {
            hoveredItemIndex--;
            if (hoveredItemIndex < scrollOffset) {
                scrollOffset--;
            }
        }
    } else if (key == KB_KEY_ENTER) {
        if (isOpen && hoveredItemIndex >= 0) {
            setSelectedIndex(hoveredItemIndex);
            if (selectionCallback) {
                selectionCallback(hoveredItemIndex, items[hoveredItemIndex]);
            }
            close();
        }
    } else if (key == KB_KEY_ESCAPE) {
        if (isOpen) {
            close();
        }
    }
}

void ComboBox::handleMouseButton(int mouseX, int mouseY, bool isPressed) {
    int absX = getAbsoluteX();
    int buttonX = absX + width - buttonWidth;

    if (isPressed) {
        // Check if clicking dropdown button
        if (mouseX >= buttonX && mouseX < absX + width) {
            toggle();
            return;
        }

        // Check if clicking menu item
        if (isOpen) {
            int itemIndex = getItemAtPosition(mouseX, mouseY);
            if (itemIndex >= 0) {
                setSelectedIndex(itemIndex);
                if (selectionCallback) {
                    selectionCallback(itemIndex, items[itemIndex]);
                }
                close();
                return;
            }
        }

        // Start text selection
        if (checkClick(mouseX, mouseY)) {
            int relativeX = mouseX - absX - 5 + textOffset;
            cursorPosition = getCharacterIndexAtPosition(relativeX);
            selectionStart = cursorPosition;
            selectionEnd = cursorPosition;
            isSelecting = true;
        }
    } else {
        isSelecting = false;
    }
}

void ComboBox::handleMouseMove(int mouseX, int mouseY) {
    if (isSelecting && isFocused) {
        int absX = getAbsoluteX();
        int relativeX = mouseX - absX - 5 + textOffset;
        cursorPosition = getCharacterIndexAtPosition(relativeX);
        selectionEnd = cursorPosition;
    }

    if (isOpen) {
        hoveredItemIndex = getItemAtPosition(mouseX, mouseY);
    }
}

void ComboBox::setFocus(bool focused) {
    isFocused = focused;
    if (!focused) {
        selectionStart = -1;
        selectionEnd = -1;
        isSelecting = false;
    }
}

void ComboBox::setChangeCallback(std::function<void(const std::string&)> callback) {
    changeCallback = callback;
}

void ComboBox::setSelectionCallback(std::function<void(int, const std::string&)> callback) {
    selectionCallback = callback;
}

void ComboBox::toggle() {
    isOpen = !isOpen;
    if (isOpen) {
        hoveredItemIndex = selectedItemIndex;
        if (hoveredItemIndex >= 0) {
            scrollOffset = std::max(0, hoveredItemIndex - maxVisibleItems / 2);
        }
    }
}

void ComboBox::open() {
    isOpen = true;
    hoveredItemIndex = selectedItemIndex;
    if (hoveredItemIndex >= 0) {
        scrollOffset = std::max(0, hoveredItemIndex - maxVisibleItems / 2);
    }
}

void ComboBox::close() {
    isOpen = false;
    hoveredItemIndex = -1;
}

void ComboBox::copy() {
    if (!hasSelection()) return;

    int selStart = std::min(selectionStart, selectionEnd);
    int selEnd = std::max(selectionStart, selectionEnd);
    std::string selectedText = text.substr(selStart, selEnd - selStart);

    FILE* pipe = popen("xclip -selection clipboard", "w");
    if (pipe) {
        fwrite(selectedText.c_str(), 1, selectedText.length(), pipe);
        pclose(pipe);
    }
}

void ComboBox::cut() {
    if (!hasSelection()) return;
    copy();
    deleteSelection();
    if (changeCallback) {
        changeCallback(text);
    }
}

void ComboBox::paste() {
    FILE* pipe = popen("xclip -selection clipboard -o", "r");
    if (pipe) {
        char buffer[1024];
        std::string pastedText;
        while (fgets(buffer, sizeof(buffer), pipe)) {
            pastedText += buffer;
        }
        pclose(pipe);

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

void ComboBox::selectAll() {
    selectionStart = 0;
    selectionEnd = text.length();
    cursorPosition = text.length();
}

bool ComboBox::hasSelection() const {
    return selectionStart != -1 && selectionEnd != -1 && selectionStart != selectionEnd;
}

void ComboBox::stopSelecting() {
    isSelecting = false;
}
