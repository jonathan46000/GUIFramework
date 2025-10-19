#include "CascadeMenu.h"
#include "MiniFB.h"
#include <algorithm>

CascadeMenuItem::CascadeMenuItem(const std::string& label)
    : MenuItem(label), submenu(nullptr) {
}

CascadeMenuItem::~CascadeMenuItem() {
    if (submenu) {
        delete submenu;
    }
}

void CascadeMenuItem::setSubmenu(ContextMenu* menu) {
    submenu = menu;
}

CascadeMenu::CascadeMenu(int menuWidth, int itemHeight)
    : ContextMenu(menuWidth, itemHeight), activeSubmenu(nullptr), activeSubmenuIndex(-1) {
}

CascadeMenu::~CascadeMenu() {
    closeActiveSubmenu();
}

void CascadeMenu::setFontRenderer(FontRenderer* renderer) {
    ContextMenu::setFontRenderer(renderer);

    for (size_t i = 0; i < items.size(); i++) {
        CascadeMenuItem* cascadeItem = dynamic_cast<CascadeMenuItem*>(items[i]);
        if (cascadeItem && cascadeItem->hasSubmenu()) {
            cascadeItem->getSubmenu()->setFontRenderer(renderer);
        }
    }
}

void CascadeMenu::closeActiveSubmenu() {
    if (activeSubmenu) {
        activeSubmenu->close();
        activeSubmenu = nullptr;
        activeSubmenuIndex = -1;
    }
}

void CascadeMenu::openSubmenu(int itemIndex, int menuX, int menuY, int menuWidth) {
    MenuItem* baseItem = items[itemIndex];
    CascadeMenuItem* cascadeItem = dynamic_cast<CascadeMenuItem*>(baseItem);

    if (cascadeItem && cascadeItem->hasSubmenu()) {
        closeActiveSubmenu();

        activeSubmenu = cascadeItem->getSubmenu();
        activeSubmenuIndex = itemIndex;

        int submenuX = menuX + menuWidth;
        int submenuY = menuY + itemIndex * 25;

        activeSubmenu->open(submenuX, submenuY);
    }
}

void CascadeMenu::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    ContextMenu::draw(buffer, bufferWidth, bufferHeight);

    if (activeSubmenu) {
        activeSubmenu->draw(buffer, bufferWidth, bufferHeight);
    }
}

void CascadeMenu::handleMouseMove(int mouseX, int mouseY) {
    if (!isOpen) return;

    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    bool hoveredOverSubmenu = false;
    if (activeSubmenu && activeSubmenu->checkMenuArea(mouseX, mouseY)) {
        activeSubmenu->handleMouseMove(mouseX, mouseY);
        hoveredOverSubmenu = true;
    }

    if (!hoveredOverSubmenu) {
        for (size_t i = 0; i < items.size(); i++) {
            int itemY = absY + i * 25;
            bool hover = mouseX >= absX && mouseX < absX + menuWidth &&
                        mouseY >= itemY && mouseY < itemY + 25;

            items[i]->setHovered(hover);

            if (hover) {
                CascadeMenuItem* cascadeItem = dynamic_cast<CascadeMenuItem*>(items[i]);
                if (cascadeItem && cascadeItem->hasSubmenu()) {
                    if (activeSubmenuIndex != (int)i) {
                        openSubmenu(i, absX, absY, menuWidth);
                    }
                } else {
                    closeActiveSubmenu();
                }
            }
        }
    }
}

bool CascadeMenu::checkMenuClick(int mouseX, int mouseY) {
    if (!isOpen) return false;

    if (activeSubmenu && activeSubmenu->checkMenuArea(mouseX, mouseY)) {
        bool clicked = activeSubmenu->checkMenuClick(mouseX, mouseY);
        if (clicked) {
            close();
        }
        return clicked;
    }

    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    for (size_t i = 0; i < items.size(); i++) {
        int itemY = absY + i * 25;
        if (mouseX >= absX && mouseX < absX + menuWidth &&
            mouseY >= itemY && mouseY < itemY + 25) {

            CascadeMenuItem* cascadeItem = dynamic_cast<CascadeMenuItem*>(items[i]);
            if (cascadeItem && cascadeItem->hasSubmenu()) {
                return false;
            }

            items[i]->onClick();
            close();
            return true;
        }
    }

    return false;
}

bool CascadeMenu::checkMenuArea(int mouseX, int mouseY) {
    if (!isOpen) return false;

    if (activeSubmenu && activeSubmenu->checkMenuArea(mouseX, mouseY)) {
        return true;
    }

    return ContextMenu::checkMenuArea(mouseX, mouseY);
}

void CascadeMenu::close() {
    closeActiveSubmenu();
    ContextMenu::close();
}
