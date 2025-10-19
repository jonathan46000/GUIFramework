#ifndef MENUITEM_H
#define MENUITEM_H

#include "Widget.h"
#include <string>
#include <functional>

class MenuItem : public Widget {
private:
    std::string text;
    int index;
    uint32_t backgroundColor;
    uint32_t hoverColor;
    uint32_t textColor;
    bool isHovered;
    std::function<void()> callback;

public:
    MenuItem(const std::string& text);

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void drawInMenu(uint32_t* buffer, int bufferWidth, int bufferHeight, int menuX, int menuY, int menuWidth, FontRenderer* fontRenderer);
    void setIndex(int newIndex);
    void setHovered(bool hovered);
    void setCallback(std::function<void()> cb);
    void onClick();

    const std::string& getText() const { return text; }
};

#endif
