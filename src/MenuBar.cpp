#include "MenuBar.h"
#include <algorithm>

MenuBar::MenuBar(int x, int y, int width, int height,
                 bool anchorBottom,
                 uint32_t bgColor, uint32_t borderColor)
    : Widget(x, y, width, height), anchorBottom(anchorBottom),
      backgroundColor(bgColor), borderColor(borderColor) {
}

MenuBar::~MenuBar() {
    for (DropDownMenu* menu : dropDownMenus) {
        delete menu;
    }
}

void MenuBar::setFontRenderer(FontRenderer* renderer) {
    Widget::setFontRenderer(renderer);
    for (DropDownMenu* menu : dropDownMenus) {
        menu->setFontRenderer(renderer);
    }
}

void MenuBar::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
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

    for (DropDownMenu* menu : dropDownMenus) {
        menu->draw(buffer, bufferWidth, bufferHeight);
    }
}

void MenuBar::onWindowResize(int windowWidth, int windowHeight) {
    width = windowWidth;

    if (anchorBottom) {
        y = windowHeight - height;
    }

    int menuX = 5;
    for (DropDownMenu* menu : dropDownMenus) {
        menu->setPosition(menuX, 2);
        menuX += menu->getWidth() + 5;
    }
}

void MenuBar::add(DropDownMenu* menu) {
    int menuX = 5;
    for (DropDownMenu* existingMenu : dropDownMenus) {
        menuX += existingMenu->getWidth() + 5;
    }
    menu->setPosition(menuX, 2);
    menu->setParent(this);
    menu->setFontRenderer(fontRenderer);
    dropDownMenus.push_back(menu);
}

void MenuBar::handleMouseClick(int mouseX, int mouseY) {
    for (DropDownMenu* menu : dropDownMenus) {
        if (menu->checkClick(mouseX, mouseY)) {
            for (DropDownMenu* otherMenu : dropDownMenus) {
                if (otherMenu != menu) {
                    otherMenu->close();
                }
            }
            menu->toggle();
        } else if (menu->getIsOpen()) {
            if (menu->checkMenuArea(mouseX, mouseY)) {
                menu->handleMenuClick(mouseX, mouseY);
            } else {
                menu->close();
            }
        }
    }
}

void MenuBar::handleMouseMove(int mouseX, int mouseY) {
    for (DropDownMenu* menu : dropDownMenus) {
        menu->checkHover(mouseX, mouseY);
        menu->handleMouseMove(mouseX, mouseY);
    }
}

void MenuBar::setBackgroundColor(uint32_t color) {
    backgroundColor = color;
}

void MenuBar::setBorderColor(uint32_t color) {
    borderColor = color;
}
