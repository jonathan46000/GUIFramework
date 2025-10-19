#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include "Widget.h"
#include <string>
#include <functional>

class PushButton : public Widget {
private:
    std::string label;
    bool isHovered;
    bool isPressed;
    uint32_t backgroundColor;
    uint32_t hoverColor;
    uint32_t pressedColor;
    uint32_t borderColor;
    uint32_t textColor;
    std::function<void()> clickCallback;

public:
    PushButton(const std::string& label, int x, int y, int width, int height);

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void checkHover(int mouseX, int mouseY) override;

    void setPressed(bool pressed);
    void setClickCallback(std::function<void()> callback);
    void onClick();

    void setBackgroundColor(uint32_t color);
    void setHoverColor(uint32_t color);
    void setPressedColor(uint32_t color);
    void setBorderColor(uint32_t color);
    void setTextColor(uint32_t color);
};

#endif
