#ifndef CANVAS_H
#define CANVAS_H

#include "Widget.h"
#include <functional>

class Canvas : public Widget {
private:
    uint32_t backgroundColor;
    uint32_t borderColor;
    std::function<void(Canvas*, uint32_t*, int, int)> drawCallback;
    std::function<void(int, int, bool)> mouseCallback;

    uint32_t* canvasBuffer;
    int canvasWidth;
    int canvasHeight;
    bool mousePressed;

public:
    Canvas(int x, int y, int width, int height);
    ~Canvas();

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void handleMouseButton(int mouseX, int mouseY, bool isPressed) override;
    void handleMouseMove(int mouseX, int mouseY) override;

    // Drawing primitives (canvas-relative coordinates)
    void setPixel(int x, int y, uint32_t color);
    void drawLine(int x1, int y1, int x2, int y2, uint32_t color);
    void drawRect(int x, int y, int w, int h, uint32_t color);
    void fillRect(int x, int y, int w, int h, uint32_t color);
    void drawCircle(int centerX, int centerY, int radius, uint32_t color);
    void fillCircle(int centerX, int centerY, int radius, uint32_t color);
    void clear(uint32_t color);

    // Callbacks
    void setDrawCallback(std::function<void(Canvas*, uint32_t*, int, int)> callback);
    void setMouseCallback(std::function<void(int, int, bool)> callback);

    // Colors
    void setBackgroundColor(uint32_t color);
    void setBorderColor(uint32_t color);

    // Direct buffer access
    uint32_t* getBuffer() { return canvasBuffer; }
    int getCanvasWidth() const { return canvasWidth; }
    int getCanvasHeight() const { return canvasHeight; }
};

#endif
