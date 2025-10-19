#include "RadioButton.h"
#include "MiniFB.h"
#include <algorithm>
#include <cmath>

RadioButton::RadioButton(const std::string& label, int x, int y)
    : BooleanWidget(label, x, y, 200, 20), circleSize(16), group(nullptr) {
}

RadioButton::RadioButton(const std::string& label, int x, int y, int circleSize)
    : BooleanWidget(label, x, y, 200, circleSize), circleSize(circleSize), group(nullptr) {
}

void RadioButton::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    uint32_t bgColor = isHovered ? hoverColor : backgroundColor;

    int centerX = absX + circleSize / 2;
    int centerY = absY + circleSize / 2;
    int radius = circleSize / 2;

    for (int py = absY; py < absY + circleSize && py < bufferHeight; py++) {
        for (int px = absX; px < absX + circleSize && px < bufferWidth; px++) {
            if (px >= 0 && py >= 0) {
                int dx = px - centerX;
                int dy = py - centerY;
                int distSq = dx * dx + dy * dy;
                int radiusSq = radius * radius;

                if (distSq <= radiusSq) {
                    if (distSq >= (radius - 1) * (radius - 1)) {
                        buffer[py * bufferWidth + px] = borderColor;
                    } else {
                        buffer[py * bufferWidth + px] = bgColor;
                    }
                }
            }
        }
    }

    if (isChecked) {
        int innerRadius = radius / 2;
        for (int py = absY; py < absY + circleSize && py < bufferHeight; py++) {
            for (int px = absX; px < absX + circleSize && px < bufferWidth; px++) {
                if (px >= 0 && py >= 0) {
                    int dx = px - centerX;
                    int dy = py - centerY;
                    int distSq = dx * dx + dy * dy;

                    if (distSq <= innerRadius * innerRadius) {
                        buffer[py * bufferWidth + px] = checkColor;
                    }
                }
            }
        }
    }

    if (fontRenderer && !label.empty()) {
        int textX = absX + circleSize + 5;
        int textY = absY + (circleSize + fontRenderer->getTextHeight()) / 2;
        fontRenderer->drawText(buffer, bufferWidth, bufferHeight, label, textX, textY, textColor);
    }
}

void RadioButton::toggle() {
    if (!isChecked) {
        if (group) {
            for (RadioButton* radio : *group) {
                if (radio != this) {
                    radio->setChecked(false);
                }
            }
        }
        isChecked = true;
        notifyChange();
    }
}

void RadioButton::setGroup(std::vector<RadioButton*>* radioGroup) {
    group = radioGroup;
}

void RadioButton::setCircleSize(int size) {
    circleSize = size;
    height = size;
}
