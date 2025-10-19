#include "MenuItem.h"
#include "MiniFB.h"
#include <algorithm>

MenuItem::MenuItem(const std::string& text)
    : Widget(0, 0, 0, 25), text(text), index(0),
      backgroundColor(0xFFFFFFFF), hoverColor(MFB_RGB(0, 120, 215)),
      textColor(MFB_RGB(0, 0, 0)), isHovered(false), callback(nullptr) {
}

void MenuItem::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    (void)buffer;
    (void)bufferWidth;
    (void)bufferHeight;
}

void MenuItem::drawInMenu(uint32_t* buffer, int bufferWidth, int bufferHeight, int menuX, int menuY, int menuWidth, FontRenderer* fontRenderer) {
    int itemY = menuY + index * 25;
    int endX = std::min(menuX + menuWidth, bufferWidth);
    int endY = std::min(itemY + 25, bufferHeight);

    uint32_t bgColor = isHovered ? hoverColor : backgroundColor;
    uint32_t txtColor = isHovered ? MFB_RGB(255, 255, 255) : textColor;

    for (int py = itemY; py < endY; py++) {
        for (int px = menuX; px < endX; px++) {
            if (px >= 0 && py >= 0) {
                buffer[py * bufferWidth + px] = bgColor;
            }
        }
    }

    if (fontRenderer) {
        int textX = menuX + 5;
        int textY = itemY + 18;
        fontRenderer->drawText(buffer, bufferWidth, bufferHeight, text, textX, textY, txtColor);
    }
}

void MenuItem::setIndex(int newIndex) {
    index = newIndex;
}

void MenuItem::setHovered(bool hovered) {
    isHovered = hovered;
}

void MenuItem::setCallback(std::function<void()> cb) {
    callback = cb;
}

void MenuItem::onClick() {
    if (callback) {
        callback();
    }
}
