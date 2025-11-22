#include "CheckBox.h"
#include "MiniFB.h"
#include <algorithm>

CheckBox::CheckBox(const std::string& label, int x, int y)
    : BooleanWidget(label, x, y, 200, 20), boxSize(16) {
}

CheckBox::CheckBox(const std::string& label, int x, int y, int boxSize)
    : BooleanWidget(label, x, y, 200, boxSize), boxSize(boxSize) {
}

void CheckBox::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    uint32_t bgColor = isHovered ? hoverColor : backgroundColor;

    int boxEndX = std::min(absX + boxSize, bufferWidth);
    int boxEndY = std::min(absY + boxSize, bufferHeight);

    for (int py = absY; py < boxEndY; py++) {
        for (int px = absX; px < boxEndX; px++) {
            if (px >= 0 && py >= 0) {
                if (py == absY || py == boxEndY - 1 || px == absX || px == boxEndX - 1) {
                    buffer[py * bufferWidth + px] = borderColor;
                } else {
                    buffer[py * bufferWidth + px] = bgColor;
                }
            }
        }
    }

    if (isChecked) {
        int checkMargin = boxSize / 4;
        int checkStartX = absX + checkMargin;
        int checkStartY = absY + checkMargin;
        int checkEndX = std::min(absX + boxSize - checkMargin, bufferWidth);
        int checkEndY = std::min(absY + boxSize - checkMargin, bufferHeight);

        for (int py = checkStartY; py < checkEndY; py++) {
            for (int px = checkStartX; px < checkEndX; px++) {
                if (px >= 0 && py >= 0) {
                    buffer[py * bufferWidth + px] = checkColor;
                }
            }
        }
    }

    if (fontRenderer && !label.empty()) {
        int textX = absX + boxSize + 5;
        int textY = absY + (boxSize + fontRenderer->getTextHeight()) / 2;
        fontRenderer->drawText(buffer, bufferWidth, bufferHeight, label, textX, textY, textColor);
    }
}

void CheckBox::setBoxSize(int size) {
    boxSize = size;
    height = size;
}

bool CheckBox::checkClick(int mouseX, int mouseY) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    return mouseX >= absX && mouseX < absX + boxSize &&
           mouseY >= absY && mouseY < absY + boxSize;
}
