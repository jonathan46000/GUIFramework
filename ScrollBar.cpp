#include "ScrollBar.h"
#include "MiniFB.h"
#include <algorithm>
#include <iostream>

ScrollBar::ScrollBar(int x, int y, int length, ScrollBarOrientation orient)
    : Widget(x, y, (orient == ScrollBarOrientation::HORIZONTAL ? length : 20),
             (orient == ScrollBarOrientation::VERTICAL ? length : 20)),
      orientation(orient), value(0.0), minValue(0.0), maxValue(100.0),
      visibleAmount(10.0), thumbSize(20), thumbPosition(0),
      isThumbHovered(false), isThumbDragging(false),
      dragStartMousePos(0), dragStartThumbPos(0),
      backgroundColor(0xFFF0F0F0), thumbColor(0xFFC0C0C0),
      thumbHoverColor(0xFFB0B0B0), thumbDragColor(0xFFA0A0A0),
      borderColor(0xFF808080), changeCallback(nullptr) {

    int fullTrackLength = (orientation == ScrollBarOrientation::VERTICAL) ? height : width;
    double range = maxValue - minValue;
    if (range > 0) {
        thumbSize = std::max(20, (int)((visibleAmount / range) * fullTrackLength));
    }

    updateThumbPosition();

    std::cerr << "ScrollBar created: orientation=" << (int)orientation
              << " length=" << length << " thumbSize=" << thumbSize << std::endl;
}

void ScrollBar::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int endX = std::min(absX + width, bufferWidth);
    int endY = std::min(absY + height, bufferHeight);

    for (int py = absY; py < endY; py++) {
        for (int px = absX; px < endX; px++) {
            if (px >= 0 && py >= 0) {
                if (py == absY || py == endY - 1 || px == absX || px == endX - 1) {
                    buffer[py * bufferWidth + px] = borderColor;
                } else {
                    buffer[py * bufferWidth + px] = backgroundColor;
                }
            }
        }
    }

    uint32_t thumbCol = isThumbDragging ? thumbDragColor : (isThumbHovered ? thumbHoverColor : thumbColor);

    if (orientation == ScrollBarOrientation::VERTICAL) {
        int thumbY = absY + thumbPosition;
        int thumbEndX = std::min(absX + width, bufferWidth);
        int thumbEndY = std::min(thumbY + thumbSize, absY + height);

        for (int py = thumbY; py < thumbEndY; py++) {
            for (int px = absX; px < thumbEndX; px++) {
                if (px >= 0 && py >= 0 && px < bufferWidth && py < bufferHeight) {
                    if (py == thumbY || py == thumbEndY - 1 || px == absX || px == thumbEndX - 1) {
                        buffer[py * bufferWidth + px] = borderColor;
                    } else {
                        buffer[py * bufferWidth + px] = thumbCol;
                    }
                }
            }
        }
    } else {
        int thumbX = absX + thumbPosition;
        int thumbEndX = std::min(thumbX + thumbSize, absX + width);
        int thumbEndY = std::min(absY + height, bufferHeight);

        for (int py = absY; py < thumbEndY; py++) {
            for (int px = thumbX; px < thumbEndX; px++) {
                if (px >= 0 && py >= 0 && px < bufferWidth && py < bufferHeight) {
                    if (py == absY || py == thumbEndY - 1 || px == thumbX || px == thumbEndX - 1) {
                        buffer[py * bufferWidth + px] = borderColor;
                    } else {
                        buffer[py * bufferWidth + px] = thumbCol;
                    }
                }
            }
        }
    }
}

void ScrollBar::checkHover(int mouseX, int mouseY) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    if (orientation == ScrollBarOrientation::VERTICAL) {
        int thumbY = absY + thumbPosition;
        isThumbHovered = mouseX >= absX && mouseX < absX + width &&
                        mouseY >= thumbY && mouseY < thumbY + thumbSize;
    } else {
        int thumbX = absX + thumbPosition;
        isThumbHovered = mouseX >= thumbX && mouseX < thumbX + thumbSize &&
                        mouseY >= absY && mouseY < absY + height;
    }
}

void ScrollBar::handleMouseButton(int mouseX, int mouseY, bool isPressed) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    if (isPressed) {
        checkHover(mouseX, mouseY);

        std::cerr << "ScrollBar clicked: isThumbHovered=" << isThumbHovered << std::endl;

        if (isThumbHovered) {
            isThumbDragging = true;
            if (orientation == ScrollBarOrientation::VERTICAL) {
                dragStartMousePos = mouseY;
            } else {
                dragStartMousePos = mouseX;
            }
            dragStartThumbPos = thumbPosition;
            std::cerr << "Started dragging thumb at position " << thumbPosition << std::endl;
        } else if (containsPoint(mouseX, mouseY)) {
            int targetPos;
            if (orientation == ScrollBarOrientation::VERTICAL) {
                targetPos = mouseY - absY - thumbSize / 2;
            } else {
                targetPos = mouseX - absX - thumbSize / 2;
            }

            int trackLength = getThumbTrackLength();
            thumbPosition = std::max(0, std::min(targetPos, trackLength));
            updateValueFromThumbPosition();
            std::cerr << "Jumped to position " << thumbPosition << std::endl;
        }
    } else {
        std::cerr << "Mouse released, was dragging: " << isThumbDragging << std::endl;
        isThumbDragging = false;
    }
}

void ScrollBar::handleMouseMove(int mouseX, int mouseY) {
    if (isThumbDragging) {
        int currentMousePos = (orientation == ScrollBarOrientation::VERTICAL) ? mouseY : mouseX;
        int delta = currentMousePos - dragStartMousePos;
        int newThumbPos = dragStartThumbPos + delta;

        int trackLength = getThumbTrackLength();
        thumbPosition = std::max(0, std::min(newThumbPos, trackLength));
        updateValueFromThumbPosition();

        std::cerr << "Dragging: delta=" << delta << " thumbPosition=" << thumbPosition << " value=" << value << std::endl;
    }
}

void ScrollBar::setValue(double val) {
    value = std::max(minValue, std::min(val, maxValue - visibleAmount));
    updateThumbPosition();
}

void ScrollBar::setRange(double min, double max) {
    minValue = min;
    maxValue = max;
    value = std::max(minValue, std::min(value, maxValue - visibleAmount));
    updateThumbPosition();
}

void ScrollBar::setVisibleAmount(double amount) {
    visibleAmount = amount;
    value = std::max(minValue, std::min(value, maxValue - visibleAmount));

    int fullTrackLength = (orientation == ScrollBarOrientation::VERTICAL) ? height : width;
    double range = maxValue - minValue;
    if (range > 0) {
        thumbSize = std::max(20, (int)((visibleAmount / range) * fullTrackLength));
    }

    updateThumbPosition();
}

void ScrollBar::setChangeCallback(std::function<void(double)> callback) {
    changeCallback = callback;
}

void ScrollBar::updateThumbPosition() {
    int trackLength = getThumbTrackLength();
    double range = maxValue - minValue - visibleAmount;

    if (range > 0) {
        thumbPosition = (int)((value - minValue) / range * trackLength);
    } else {
        thumbPosition = 0;
    }
}

void ScrollBar::updateValueFromThumbPosition() {
    int trackLength = getThumbTrackLength();
    double range = maxValue - minValue - visibleAmount;

    if (trackLength > 0) {
        value = minValue + (thumbPosition / (double)trackLength) * range;
        value = std::max(minValue, std::min(value, maxValue - visibleAmount));

        if (changeCallback) {
            changeCallback(value);
        }
    }
}

int ScrollBar::getThumbTrackLength() const {
    if (orientation == ScrollBarOrientation::VERTICAL) {
        return height - thumbSize;
    } else {
        return width - thumbSize;
    }
}

int ScrollBar::getThumbPositionFromMouse(int mousePos) const {
    int absPos = (orientation == ScrollBarOrientation::VERTICAL) ? getAbsoluteY() : getAbsoluteX();
    return mousePos - absPos - thumbSize / 2;
}

void ScrollBar::setBackgroundColor(uint32_t color) {
    backgroundColor = color;
}

void ScrollBar::setThumbColor(uint32_t color) {
    thumbColor = color;
}

void ScrollBar::setThumbHoverColor(uint32_t color) {
    thumbHoverColor = color;
}

void ScrollBar::setThumbDragColor(uint32_t color) {
    thumbDragColor = color;
}

void ScrollBar::setBorderColor(uint32_t color) {
    borderColor = color;
}
