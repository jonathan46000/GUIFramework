#include "PushButton.h"
#include "MiniFB.h"
#include <algorithm>

PushButton::PushButton(const std::string& label, int x, int y, int width, int height)
    : Widget(x, y, width, height), label(label), isHovered(false), isPressed(false),
      backgroundColor(0xFFD0D0D0), hoverColor(0xFFB0B0B0),
      pressedColor(0xFF909090), borderColor(0xFF808080),
      textColor(MFB_RGB(0, 0, 0)), clickCallback(nullptr) {
}

void PushButton::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int endX = std::min(absX + width, bufferWidth);
    int endY = std::min(absY + height, bufferHeight);

    uint32_t bgColor = isPressed ? pressedColor : (isHovered ? hoverColor : backgroundColor);

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

    if (fontRenderer && !label.empty()) {
        int textWidth = fontRenderer->getTextWidth(label);
        int textHeight = fontRenderer->getTextHeight();
        int textX = absX + (width - textWidth) / 2;
        int textY = absY + (height - textHeight) / 2 + textHeight;
        fontRenderer->drawText(buffer, bufferWidth, bufferHeight, label, textX, textY, textColor);
    }
}

void PushButton::checkHover(int mouseX, int mouseY) {
    isHovered = containsPoint(mouseX, mouseY);
}

void PushButton::setPressed(bool pressed) {
    isPressed = pressed;
}

void PushButton::setClickCallback(std::function<void()> callback) {
    clickCallback = callback;
}

void PushButton::onClick() {
    if (clickCallback) {
        clickCallback();
    }
}

void PushButton::setBackgroundColor(uint32_t color) {
    backgroundColor = color;
}

void PushButton::setHoverColor(uint32_t color) {
    hoverColor = color;
}

void PushButton::setPressedColor(uint32_t color) {
    pressedColor = color;
}

void PushButton::setBorderColor(uint32_t color) {
    borderColor = color;
}

void PushButton::setTextColor(uint32_t color) {
    textColor = color;
}
