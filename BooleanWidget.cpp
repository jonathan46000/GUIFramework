#include "BooleanWidget.h"
#include "MiniFB.h"

BooleanWidget::BooleanWidget(const std::string& label, int x, int y, int width, int height)
    : Widget(x, y, width, height), label(label), isChecked(false), isHovered(false),
      backgroundColor(0xFFFFFFFF), hoverColor(0xFFE0E0E0),
      checkColor(MFB_RGB(0, 120, 215)), borderColor(0xFF808080),
      textColor(MFB_RGB(0, 0, 0)), changeCallback(nullptr) {
}

BooleanWidget::~BooleanWidget() {
}

void BooleanWidget::checkHover(int mouseX, int mouseY) {
    isHovered = containsPoint(mouseX, mouseY);
}

void BooleanWidget::toggle() {
    isChecked = !isChecked;
    notifyChange();
}

void BooleanWidget::setChecked(bool checked) {
    if (isChecked != checked) {
        isChecked = checked;
        notifyChange();
    }
}

void BooleanWidget::setChangeCallback(std::function<void(bool)> callback) {
    changeCallback = callback;
}

void BooleanWidget::setLabel(const std::string& newLabel) {
    label = newLabel;
}

void BooleanWidget::setBackgroundColor(uint32_t color) {
    backgroundColor = color;
}

void BooleanWidget::setHoverColor(uint32_t color) {
    hoverColor = color;
}

void BooleanWidget::setCheckColor(uint32_t color) {
    checkColor = color;
}

void BooleanWidget::setBorderColor(uint32_t color) {
    borderColor = color;
}

void BooleanWidget::setTextColor(uint32_t color) {
    textColor = color;
}

void BooleanWidget::notifyChange() {
    if (changeCallback) {
        changeCallback(isChecked);
    }
}
