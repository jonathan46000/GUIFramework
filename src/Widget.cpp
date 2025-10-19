#include "Widget.h"

Widget::Widget(int x, int y, int width, int height)
    : x(x), y(y), width(width), height(height), parent(nullptr), fontRenderer(nullptr) {
}

Widget::~Widget() {
}

bool Widget::checkClick(int mouseX, int mouseY) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    return mouseX >= absX && mouseX < absX + width &&
           mouseY >= absY && mouseY < absY + height;
}

void Widget::checkHover(int mouseX, int mouseY) {
    (void)mouseX;
    (void)mouseY;
}

void Widget::handleMouseMove(int mouseX, int mouseY) {
    (void)mouseX;
    (void)mouseY;
}

void Widget::handleMouseButton(int mouseX, int mouseY, bool isPressed) {
    (void)mouseX;
    (void)mouseY;
    (void)isPressed;
}

void Widget::handleChar(unsigned int charCode) {
    (void)charCode;
}

void Widget::handleKey(int key, bool isPressed) {
    (void)key;
    (void)isPressed;
}

void Widget::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

void Widget::setSize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
}

void Widget::setParent(Widget* parentWidget) {
    parent = parentWidget;
}

void Widget::setFontRenderer(FontRenderer* renderer) {
    fontRenderer = renderer;
}

void Widget::copy() {
}

void Widget::cut() {
}

void Widget::paste() {
}

void Widget::selectAll() {
}

bool Widget::hasSelection() const {
    return false;
}

int Widget::getAbsoluteX() const {
    if (parent) {
        return parent->getAbsoluteX() + x;
    }
    return x;
}

int Widget::getAbsoluteY() const {
    if (parent) {
        return parent->getAbsoluteY() + y;
    }
    return y;
}

bool Widget::containsPoint(int pointX, int pointY) const {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    return pointX >= absX && pointX < absX + width &&
           pointY >= absY && pointY < absY + height;
}
