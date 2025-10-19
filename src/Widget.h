#ifndef WIDGET_H
#define WIDGET_H

#include <cstdint>
#include "FontRenderer.h"

class Widget {
protected:
    int x, y, width, height;
    Widget* parent;
    FontRenderer* fontRenderer;

public:
    Widget(int x, int y, int width, int height);
    virtual ~Widget();

    virtual void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) = 0;
    virtual bool checkClick(int mouseX, int mouseY);
    virtual void checkHover(int mouseX, int mouseY);
    virtual void handleMouseMove(int mouseX, int mouseY);
    virtual void handleMouseButton(int mouseX, int mouseY, bool isPressed);
    virtual void handleChar(unsigned int charCode);
    virtual void handleKey(int key, bool isPressed);

    virtual void setPosition(int newX, int newY);
    virtual void setSize(int newWidth, int newHeight);
    virtual void setFontRenderer(FontRenderer* renderer);
    void setParent(Widget* parentWidget);

    virtual void copy();
    virtual void cut();
    virtual void paste();
    virtual void selectAll();
    virtual bool hasSelection() const;

    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getAbsoluteX() const;
    int getAbsoluteY() const;
    Widget* getParent() const { return parent; }

    bool containsPoint(int pointX, int pointY) const;
};

#endif
