#include "DropDownMenu.h"
#include "CascadeMenu.h"
#include "MiniFB.h"
#include <algorithm>
#include <iostream>

DropDownMenu::DropDownMenu(const std::string& label, int width, int height)
    : Widget(0, 0, width, height), label(label), isOpen(false), isHovered(false),
      backgroundColor(0xFFD0D0D0), hoverColor(0xFFB0B0B0),
      textColor(MFB_RGB(0, 0, 0)), menuBackgroundColor(0xFFFFFFFF),
      borderColor(0xFF808080), menuWidth(150), activeSubmenu(nullptr), activeSubmenuIndex(-1) {
}

DropDownMenu::DropDownMenu(const std::string& label, int width, int height, int x, int y)
    : Widget(x, y, width, height), label(label), isOpen(false), isHovered(false),
      backgroundColor(0xFFD0D0D0), hoverColor(0xFFB0B0B0),
      textColor(MFB_RGB(0, 0, 0)), menuBackgroundColor(0xFFFFFFFF),
      borderColor(0xFF808080), menuWidth(150), activeSubmenu(nullptr), activeSubmenuIndex(-1) {
}

DropDownMenu::~DropDownMenu() {
    for (MenuItem* item : items) {
        delete item;
    }
}

void DropDownMenu::add(MenuItem* item) {
    items.push_back(item);
    item->setIndex(items.size() - 1);
}

void DropDownMenu::setFontRenderer(FontRenderer* renderer) {
    Widget::setFontRenderer(renderer);

    for (size_t i = 0; i < items.size(); i++) {
        CascadeMenuItem* cascadeItem = dynamic_cast<CascadeMenuItem*>(items[i]);
        if (cascadeItem && cascadeItem->hasSubmenu()) {
            cascadeItem->getSubmenu()->setFontRenderer(renderer);
        }
    }
}

void DropDownMenu::closeActiveSubmenu() {
    if (activeSubmenu) {
        activeSubmenu->close();
        activeSubmenu = nullptr;
        activeSubmenuIndex = -1;
    }
}

void DropDownMenu::openSubmenu(int itemIndex, int menuX, int menuY, int menuWidth) {
    CascadeMenuItem* cascadeItem = dynamic_cast<CascadeMenuItem*>(items[itemIndex]);

    if (cascadeItem && cascadeItem->hasSubmenu()) {
        closeActiveSubmenu();

        activeSubmenu = cascadeItem->getSubmenu();
        activeSubmenuIndex = itemIndex;

        int submenuX = menuX + menuWidth;
        int submenuY = menuY + itemIndex * 25;

        activeSubmenu->open(submenuX, submenuY);
    }
}

void DropDownMenu::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int endX = std::min(absX + width, bufferWidth);
    int endY = std::min(absY + height, bufferHeight);

    uint32_t bgColor = isHovered ? hoverColor : backgroundColor;

    for (int py = absY; py < endY; py++) {
        for (int px = absX; px < endX; px++) {
            if (px >= 0 && py >= 0) {
                if (py == absY || py == endY - 1 || px == absX || px == endX - 1) {
                    buffer[py * bufferWidth + px] = borderColor;
                } else {
                    buffer[py * bufferWidth + px] = bgColor;
                }
            }
        }
    }

    if (fontRenderer) {
        int textX = absX + 5;
        int textY = absY + height - 5;
        fontRenderer->drawText(buffer, bufferWidth, bufferHeight, label, textX, textY, textColor);
    }

    if (isOpen && !items.empty()) {
        int menuY = absY + height;
        int menuHeight = items.size() * 25;

        int menuEndX = std::min(absX + menuWidth, bufferWidth);
        int menuEndY = std::min(menuY + menuHeight, bufferHeight);

        for (int py = menuY; py < menuEndY; py++) {
            for (int px = absX; px < menuEndX; px++) {
                if (px >= 0 && py >= 0) {
                    if (py == menuY || py == menuEndY - 1 || px == absX || px == menuEndX - 1) {
                        buffer[py * bufferWidth + px] = borderColor;
                    } else {
                        buffer[py * bufferWidth + px] = menuBackgroundColor;
                    }
                }
            }
        }

        if (!fontRenderer) {
            std::cerr << "ERROR: DropDownMenu fontRenderer is NULL when drawing items!" << std::endl;
        }

        for (size_t i = 0; i < items.size(); i++) {
            items[i]->drawInMenu(buffer, bufferWidth, bufferHeight, absX, menuY, menuWidth, fontRenderer);
        }

        if (activeSubmenu) {
            activeSubmenu->draw(buffer, bufferWidth, bufferHeight);
        }
    }
}

void DropDownMenu::checkHover(int mouseX, int mouseY) {
    isHovered = containsPoint(mouseX, mouseY);
}

void DropDownMenu::toggle() {
    isOpen = !isOpen;
    if (!isOpen) {
        closeActiveSubmenu();
    }
}

void DropDownMenu::open() {
    isOpen = true;
}

void DropDownMenu::close() {
    isOpen = false;
    closeActiveSubmenu();
}

bool DropDownMenu::checkMenuArea(int mouseX, int mouseY) {
    if (!isOpen) return false;

    if (activeSubmenu && activeSubmenu->checkMenuArea(mouseX, mouseY)) {
        return true;
    }

    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int menuY = absY + height;
    int menuHeight = items.size() * 25;

    return mouseX >= absX && mouseX < absX + menuWidth &&
           mouseY >= menuY && mouseY < menuY + menuHeight;
}

void DropDownMenu::handleMouseMove(int mouseX, int mouseY) {
    if (isOpen) {
        int absX = getAbsoluteX();
        int absY = getAbsoluteY();
        int menuY = absY + height;

        bool hoveredOverSubmenu = false;
        if (activeSubmenu && activeSubmenu->checkMenuArea(mouseX, mouseY)) {
            activeSubmenu->handleMouseMove(mouseX, mouseY);
            hoveredOverSubmenu = true;
        }

        if (!hoveredOverSubmenu) {
            for (size_t i = 0; i < items.size(); i++) {
                int itemY = menuY + i * 25;
                bool hover = mouseX >= absX && mouseX < absX + menuWidth &&
                            mouseY >= itemY && mouseY < itemY + 25;
                items[i]->setHovered(hover);

                if (hover) {
                    CascadeMenuItem* cascadeItem = dynamic_cast<CascadeMenuItem*>(items[i]);
                    if (cascadeItem && cascadeItem->hasSubmenu()) {
                        if (activeSubmenuIndex != (int)i) {
                            openSubmenu(i, absX, menuY, menuWidth);
                        }
                    } else {
                        closeActiveSubmenu();
                    }
                }
            }
        }
    }
}

void DropDownMenu::handleMenuClick(int mouseX, int mouseY) {
    if (!isOpen) return;

    if (activeSubmenu && activeSubmenu->checkMenuArea(mouseX, mouseY)) {
        bool clicked = activeSubmenu->checkMenuClick(mouseX, mouseY);
        if (clicked) {
            close();
        }
        return;
    }

    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int menuY = absY + height;
    for (size_t i = 0; i < items.size(); i++) {
        int itemY = menuY + i * 25;
        if (mouseX >= absX && mouseX < absX + menuWidth &&
            mouseY >= itemY && mouseY < itemY + 25) {

            CascadeMenuItem* cascadeItem = dynamic_cast<CascadeMenuItem*>(items[i]);
            if (cascadeItem && cascadeItem->hasSubmenu()) {
                return;
            }

            items[i]->onClick();
            close();
            break;
        }
    }
}
