#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include "Widget.h"
#include <functional>

enum class ScrollBarOrientation {
    VERTICAL,
    HORIZONTAL
};

class ScrollBar : public Widget {
private:
    ScrollBarOrientation orientation;
    double value;
    double minValue;
    double maxValue;
    double visibleAmount;
    int thumbSize;
    int thumbPosition;
    bool isThumbHovered;
    bool isThumbDragging;
    int dragStartMousePos;
    int dragStartThumbPos;
    uint32_t backgroundColor;
    uint32_t thumbColor;
    uint32_t thumbHoverColor;
    uint32_t thumbDragColor;
    uint32_t borderColor;
    std::function<void(double)> changeCallback;

    void updateThumbPosition();
    void updateValueFromThumbPosition();
    int getThumbTrackLength() const;
    int getThumbPositionFromMouse(int mousePos) const;

public:
    ScrollBar(int x, int y, int length, ScrollBarOrientation orient);

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void checkHover(int mouseX, int mouseY) override;
    void handleMouseButton(int mouseX, int mouseY, bool isPressed) override;
    void handleMouseMove(int mouseX, int mouseY) override;

    void setValue(double val);
    void setRange(double min, double max);
    void setVisibleAmount(double amount);
    void setChangeCallback(std::function<void(double)> callback);

    double getValue() const { return value; }
    double getMinValue() const { return minValue; }
    double getMaxValue() const { return maxValue; }
    bool isDragging() const { return isThumbDragging; }

    void setBackgroundColor(uint32_t color);
    void setThumbColor(uint32_t color);
    void setThumbHoverColor(uint32_t color);
    void setThumbDragColor(uint32_t color);
    void setBorderColor(uint32_t color);
};

#endif
