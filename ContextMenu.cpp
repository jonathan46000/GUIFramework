#include "ContextMenu.h"
#include "MiniFB.h"
#include <algorithm>

ContextMenu::ContextMenu(int menuWidth, int itemHeight)
    : Widget(0, 0, 0, 0), isOpen(false),
      backgroundColor(0xFFFFFFFF), borderColor(0xFF808080),
      menuWidth(menuWidth), itemHeight(itemHeight) {
}

ContextMenu::~ContextMenu() {
    for (MenuItem* item : items) {
        delete item;
    }
}

void ContextMenu::add(MenuItem* item) {
    items.push_back(item);
    item->setIndex(items.size() - 1);
}

void ContextMenu::setFontRenderer(FontRenderer* renderer) {
    Widget::setFontRenderer(renderer);
}

void ContextMenu::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    if (!isOpen || items.empty()) {
        return;
    }

    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int menuHeight = items.size() * itemHeight;

    int endX = std::min(absX + menuWidth, bufferWidth);
    int endY = std::min(absY + menuHeight, bufferHeight);

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

    for (size_t i = 0; i < items.size(); i++) {
        items[i]->drawInMenu(buffer, bufferWidth, bufferHeight, absX, absY, menuWidth, fontRenderer);
    }
}

void ContextMenu::handleMouseMove(int mouseX, int mouseY) {
    if (!isOpen) return;

    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    for (size_t i = 0; i < items.size(); i++) {
        int itemY = absY + i * itemHeight;
        bool hover = mouseX >= absX && mouseX < absX + menuWidth &&
                    mouseY >= itemY && mouseY < itemY + itemHeight;
        items[i]->setHovered(hover);
    }
}

void ContextMenu::open(int mouseX, int mouseY) {
    x = mouseX;
    y = mouseY;
    width = menuWidth;
    height = items.size() * itemHeight;
    isOpen = true;
}

void ContextMenu::close() {
    isOpen = false;
}

bool ContextMenu::checkMenuArea(int mouseX, int mouseY) {
    if (!isOpen) return false;

    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int menuHeight = items.size() * itemHeight;

    return mouseX >= absX && mouseX < absX + menuWidth &&
           mouseY >= absY && mouseY < absY + menuHeight;
}

bool ContextMenu::checkMenuClick(int mouseX, int mouseY) {
    if (!isOpen) return false;

    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    for (size_t i = 0; i < items.size(); i++) {
        int itemY = absY + i * itemHeight;
        if (mouseX >= absX && mouseX < absX + menuWidth &&
            mouseY >= itemY && mouseY < itemY + itemHeight) {
            items[i]->onClick();
            close();
            return true;
        }
    }

    return false;
}
