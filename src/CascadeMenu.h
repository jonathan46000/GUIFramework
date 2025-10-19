#ifndef CASCADEMENU_H
#define CASCADEMENU_H

#include "ContextMenu.h"
#include "MenuItem.h"
#include <vector>
#include <string>

class CascadeMenuItem : public MenuItem {
private:
    ContextMenu* submenu;

public:
    CascadeMenuItem(const std::string& label);
    ~CascadeMenuItem();

    void setSubmenu(ContextMenu* menu);
    ContextMenu* getSubmenu() const { return submenu; }
    bool hasSubmenu() const { return submenu != nullptr; }
};

class CascadeMenu : public ContextMenu {
private:
    ContextMenu* activeSubmenu;
    int activeSubmenuIndex;

    void closeActiveSubmenu();
    void openSubmenu(int itemIndex, int menuX, int menuY, int menuWidth);

public:
    CascadeMenu(int menuWidth = 150, int itemHeight = 25);
    ~CascadeMenu();

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void handleMouseMove(int mouseX, int mouseY) override;
    void setFontRenderer(FontRenderer* renderer) override;
    bool checkMenuClick(int mouseX, int mouseY);
    bool checkMenuArea(int mouseX, int mouseY);
    void close();
};

#endif
