#include "ListBox.h"
#include "MiniFB.h"
#include <algorithm>

ListBox::ListBox(int x, int y, int width, int height)
    : Widget(x, y, width, height), selectedIndex(-1), scrollOffset(0),
      itemHeight(25), visibleItemCount(0), scrollBar(nullptr), scrollBarWidth(20),
      backgroundColor(0xFFF0F0F0), itemBackgroundColor(0xFFFFFFFF),
      selectedBackgroundColor(MFB_RGB(0, 120, 215)), hoverBackgroundColor(0xFFE0E0E0),
      textColor(MFB_RGB(0, 0, 0)), selectedTextColor(MFB_RGB(255, 255, 255)),
      borderColor(0xFF808080), hoveredIndex(-1), selectionCallback(nullptr) {

    visibleItemCount = (height - 2) / itemHeight;

    scrollBar = new ScrollBar(width - scrollBarWidth, 0, height, ScrollBarOrientation::VERTICAL);
    scrollBar->setParent(this);
    scrollBar->setRange(0, 0);
    scrollBar->setVisibleAmount(visibleItemCount);
    scrollBar->setValue(0);
    scrollBar->setChangeCallback([this](double value) {
        scrollOffset = (int)value;
    });
}

ListBox::~ListBox() {
    delete scrollBar;
}

void ListBox::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int endX = std::min(absX + width, bufferWidth);
    int endY = std::min(absY + height, bufferHeight);

    for (int py = absY; py < endY; py++) {
        for (int px = absX; px < endX; px++) {
            if (px >= 0 && py >= 0) {
                if (py == absY || py == endY - 1 || px == absX || px == endX - 1) {
                    buffer[py * bufferWidth + px] = borderColor;
                } else {
                    buffer[py * bufferWidth + px] = backgroundColor;
                }
            }
        }
    }

    int contentWidth = width - scrollBarWidth - 2;
    int drawY = absY + 1;

    for (int i = scrollOffset; i < (int)items.size() && i < scrollOffset + visibleItemCount; i++) {
        int itemEndX = std::min(absX + 1 + contentWidth, bufferWidth);
        int itemEndY = std::min(drawY + itemHeight, absY + height - 1);

        uint32_t bgColor = itemBackgroundColor;
        uint32_t txtColor = textColor;

        if (i == selectedIndex) {
            bgColor = selectedBackgroundColor;
            txtColor = selectedTextColor;
        } else if (i == hoveredIndex) {
            bgColor = hoverBackgroundColor;
        }

        for (int py = drawY; py < itemEndY; py++) {
            for (int px = absX + 1; px < itemEndX; px++) {
                if (px >= 0 && py >= 0) {
                    buffer[py * bufferWidth + px] = bgColor;
                }
            }
        }

        if (fontRenderer && i < (int)items.size()) {
            int textX = absX + 5;
            int textY = drawY + (itemHeight + fontRenderer->getTextHeight()) / 2;
            fontRenderer->drawText(buffer, bufferWidth, bufferHeight, items[i], textX, textY, txtColor);
        }

        drawY += itemHeight;
    }

    if (items.size() > (size_t)visibleItemCount) {
        scrollBar->draw(buffer, bufferWidth, bufferHeight);
    }
}

void ListBox::checkHover(int mouseX, int mouseY) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    if (mouseX >= absX && mouseX < absX + width - scrollBarWidth &&
        mouseY >= absY && mouseY < absY + height) {
        hoveredIndex = getItemIndexAtPosition(mouseX, mouseY);
    } else {
        hoveredIndex = -1;
    }

    if (items.size() > (size_t)visibleItemCount) {
        scrollBar->checkHover(mouseX, mouseY);
    }
}

void ListBox::handleMouseButton(int mouseX, int mouseY, bool isPressed) {
    int absX = getAbsoluteX();

    if (items.size() > (size_t)visibleItemCount &&
        mouseX >= absX + width - scrollBarWidth && mouseX < absX + width) {
        scrollBar->handleMouseButton(mouseX, mouseY, isPressed);
        return;
    }

    if (isPressed) {
        int clickedIndex = getItemIndexAtPosition(mouseX, mouseY);
        if (clickedIndex >= 0 && clickedIndex < (int)items.size()) {
            selectedIndex = clickedIndex;
            if (selectionCallback) {
                selectionCallback(selectedIndex, items[selectedIndex]);
            }
        }
    }
}

void ListBox::handleMouseMove(int mouseX, int mouseY) {
    if (scrollBar && scrollBar->isDragging()) {
        scrollBar->handleMouseMove(mouseX, mouseY);
    }
}

void ListBox::setFontRenderer(FontRenderer* renderer) {
    Widget::setFontRenderer(renderer);
    if (scrollBar) {
        scrollBar->setFontRenderer(renderer);
    }
}

void ListBox::addItem(const std::string& item) {
    items.push_back(item);
    updateScrollBar();
}

void ListBox::removeItem(int index) {
    if (index >= 0 && index < (int)items.size()) {
        items.erase(items.begin() + index);
        if (selectedIndex == index) {
            selectedIndex = -1;
        } else if (selectedIndex > index) {
            selectedIndex--;
        }
        updateScrollBar();
    }
}

void ListBox::clearItems() {
    items.clear();
    selectedIndex = -1;
    scrollOffset = 0;
    updateScrollBar();
}

void ListBox::setSelectedIndex(int index) {
    if (index >= -1 && index < (int)items.size()) {
        selectedIndex = index;

        if (selectedIndex >= 0) {
            if (selectedIndex < scrollOffset) {
                scrollOffset = selectedIndex;
                scrollBar->setValue(scrollOffset);
            } else if (selectedIndex >= scrollOffset + visibleItemCount) {
                scrollOffset = selectedIndex - visibleItemCount + 1;
                scrollBar->setValue(scrollOffset);
            }
        }
    }
}

std::string ListBox::getSelectedItem() const {
    if (selectedIndex >= 0 && selectedIndex < (int)items.size()) {
        return items[selectedIndex];
    }
    return "";
}

void ListBox::setSelectionCallback(std::function<void(int, const std::string&)> callback) {
    selectionCallback = callback;
}

void ListBox::setItemHeight(int height) {
    itemHeight = height;
    visibleItemCount = (this->height - 2) / itemHeight;
    updateScrollBar();
}

void ListBox::updateScrollBar() {
    if (items.size() > (size_t)visibleItemCount) {
        scrollBar->setRange(0, items.size());
        scrollBar->setVisibleAmount(visibleItemCount);
    } else {
        scrollOffset = 0;
        scrollBar->setValue(0);
    }
}

int ListBox::getItemIndexAtPosition(int mouseX, int mouseY) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    if (mouseX < absX + 1 || mouseX >= absX + width - scrollBarWidth - 1 ||
        mouseY < absY + 1 || mouseY >= absY + height - 1) {
        return -1;
    }

    int relativeY = mouseY - (absY + 1);
    int itemIndex = scrollOffset + (relativeY / itemHeight);

    if (itemIndex >= 0 && itemIndex < (int)items.size()) {
        return itemIndex;
    }

    return -1;
}

void ListBox::setBackgroundColor(uint32_t color) {
    backgroundColor = color;
}

void ListBox::setItemBackgroundColor(uint32_t color) {
    itemBackgroundColor = color;
}

void ListBox::setSelectedBackgroundColor(uint32_t color) {
    selectedBackgroundColor = color;
}

void ListBox::setHoverBackgroundColor(uint32_t color) {
    hoverBackgroundColor = color;
}

void ListBox::setTextColor(uint32_t color) {
    textColor = color;
}

void ListBox::setSelectedTextColor(uint32_t color) {
    selectedTextColor = color;
}

void ListBox::setBorderColor(uint32_t color) {
    borderColor = color;
}
