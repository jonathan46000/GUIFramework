#ifndef MENUBAR_H
#define MENUBAR_H

#include "Widget.h"
#include "DropDownMenu.h"
#include <vector>

class MenuBar : public Widget {
private:
    bool anchorBottom;
    uint32_t backgroundColor;
    uint32_t borderColor;
    std::vector<DropDownMenu*> dropDownMenus;

public:
    MenuBar(int x, int y, int width, int height,
            bool anchorBottom = false,
            uint32_t bgColor = 0xFFD0D0D0,
            uint32_t borderColor = 0xFF808080);
    ~MenuBar();

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void handleMouseMove(int mouseX, int mouseY) override;
    void setFontRenderer(FontRenderer* renderer) override;

    void onWindowResize(int windowWidth, int windowHeight);
    void add(DropDownMenu* menu);
    void handleMouseClick(int mouseX, int mouseY);

    void setBackgroundColor(uint32_t color);
    void setBorderColor(uint32_t color);
};

#endif
