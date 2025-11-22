#include "ProgressBar.h"
#include <algorithm>

ProgressBar::ProgressBar(int x, int y, int width, int height,
                         uint32_t bgColor, uint32_t fillColor, uint32_t borderColor)
    : Widget(x, y, width, height), value(0.0), maxValue(100.0),
      backgroundColor(bgColor), fillColor(fillColor), borderColor(borderColor) {
}

void ProgressBar::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int endX = std::min(absX + width, bufferWidth);
    int endY = std::min(absY + height, bufferHeight);

    double fillPercent = (maxValue > 0) ? (value / maxValue) : 0.0;
    fillPercent = std::max(0.0, std::min(1.0, fillPercent));
    int fillWidth = static_cast<int>(fillPercent * (width - 2));

    for (int py = absY; py < endY; py++) {
        for (int px = absX; px < endX; px++) {
            if (px >= 0 && py >= 0 && px < bufferWidth && py < bufferHeight) {
                if (py == absY || py == endY - 1 || px == absX || px == endX - 1) {
                    buffer[py * bufferWidth + px] = borderColor;
                } else {
                    int localX = px - absX - 1;
                    if (localX < fillWidth) {
                        buffer[py * bufferWidth + px] = fillColor;
                    } else {
                        buffer[py * bufferWidth + px] = backgroundColor;
                    }
                }
            }
        }
    }
}

void ProgressBar::setValue(double val) {
    value = val;
}

void ProgressBar::setMaxValue(double max) {
    maxValue = max;
}

void ProgressBar::setPercentage(double percent) {
    value = percent;
    maxValue = 100.0;
}

double ProgressBar::getPercentage() const {
    if (maxValue <= 0) return 0.0;
    return (value / maxValue) * 100.0;
}
