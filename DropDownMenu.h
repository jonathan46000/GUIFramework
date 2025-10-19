#ifndef DROPDOWNMENU_H
#define DROPDOWNMENU_H

#include "Widget.h"
#include "MenuItem.h"
#include <vector>
#include <string>

class ContextMenu;

class DropDownMenu : public Widget {
private:
    std::string label;
    bool isOpen;
    bool isHovered;
    uint32_t backgroundColor;
    uint32_t hoverColor;
    uint32_t textColor;
    uint32_t menuBackgroundColor;
    uint32_t borderColor;
    std::vector<MenuItem*> items;
    int menuWidth;
    ContextMenu* activeSubmenu;
    int activeSubmenuIndex;

    void closeActiveSubmenu();
    void openSubmenu(int itemIndex, int menuX, int menuY, int menuWidth);

public:
    DropDownMenu(const std::string& label, int width, int height);
    DropDownMenu(const std::string& label, int width, int height, int x, int y);
    ~DropDownMenu();

    void add(MenuItem* item);
    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void checkHover(int mouseX, int mouseY) override;
    void handleMouseMove(int mouseX, int mouseY) override;
    void setFontRenderer(FontRenderer* renderer) override;

    void toggle();
    void open();
    void close();
    bool checkMenuArea(int mouseX, int mouseY);
    void handleMenuClick(int mouseX, int mouseY);

    bool getIsOpen() const { return isOpen; }
};

#endif
