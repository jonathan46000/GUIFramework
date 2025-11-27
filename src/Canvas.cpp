#include "Canvas.h"
#include <algorithm>
#include <cmath>

Canvas::Canvas(int x, int y, int width, int height)
    : Widget(x, y, width, height),
      backgroundColor(0xFFFFFFFF),
      borderColor(0xFF808080),
      drawCallback(nullptr),
      mouseCallback(nullptr),
      mousePressed(false) {

    // Account for border (1px on each side)
    canvasWidth = width - 2;
    canvasHeight = height - 2;

    if (canvasWidth < 1) canvasWidth = 1;
    if (canvasHeight < 1) canvasHeight = 1;

    canvasBuffer = new uint32_t[canvasWidth * canvasHeight];
    clear(backgroundColor);
}

Canvas::~Canvas() {
    delete[] canvasBuffer;
}

void Canvas::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int endX = std::min(absX + width, bufferWidth);
    int endY = std::min(absY + height, bufferHeight);

    // Draw border
    for (int py = absY; py < endY; py++) {
        for (int px = absX; px < endX; px++) {
            if (px >= 0 && py >= 0) {
                if (py == absY || py == endY - 1 || px == absX || px == endX - 1) {
                    buffer[py * bufferWidth + px] = borderColor;
                }
            }
        }
    }

    // Call user draw callback if set
    if (drawCallback) {
        drawCallback(this, canvasBuffer, canvasWidth, canvasHeight);
    }

    // Copy canvas buffer to screen buffer (accounting for border offset)
    int contentStartX = absX + 1;
    int contentStartY = absY + 1;
    int contentEndX = std::min(contentStartX + canvasWidth, bufferWidth);
    int contentEndY = std::min(contentStartY + canvasHeight, bufferHeight);

    for (int py = contentStartY; py < contentEndY; py++) {
        for (int px = contentStartX; px < contentEndX; px++) {
            if (px >= 0 && py >= 0) {
                int canvasX = px - contentStartX;
                int canvasY = py - contentStartY;
                if (canvasX >= 0 && canvasX < canvasWidth && canvasY >= 0 && canvasY < canvasHeight) {
                    buffer[py * bufferWidth + px] = canvasBuffer[canvasY * canvasWidth + canvasX];
                }
            }
        }
    }
}

void Canvas::handleMouseButton(int mouseX, int mouseY, bool isPressed) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    mousePressed = isPressed;

    // Convert to canvas-relative coordinates (account for border)
    int canvasX = mouseX - absX - 1;
    int canvasY = mouseY - absY - 1;

    if (canvasX >= 0 && canvasX < canvasWidth && canvasY >= 0 && canvasY < canvasHeight) {
        if (mouseCallback) {
            mouseCallback(canvasX, canvasY, isPressed);
        }
    }
}

void Canvas::handleMouseMove(int mouseX, int mouseY) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    // Convert to canvas-relative coordinates (account for border)
    int canvasX = mouseX - absX - 1;
    int canvasY = mouseY - absY - 1;

    if (canvasX >= 0 && canvasX < canvasWidth && canvasY >= 0 && canvasY < canvasHeight) {
        if (mouseCallback) {
            mouseCallback(canvasX, canvasY, mousePressed);
        }
    }
}

void Canvas::setPixel(int x, int y, uint32_t color) {
    if (x >= 0 && x < canvasWidth && y >= 0 && y < canvasHeight) {
        canvasBuffer[y * canvasWidth + x] = color;
    }
}

void Canvas::drawLine(int x1, int y1, int x2, int y2, uint32_t color) {
    // Bresenham's line algorithm
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        setPixel(x1, y1, color);

        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void Canvas::drawRect(int x, int y, int w, int h, uint32_t color) {
    // Top and bottom edges
    for (int px = x; px < x + w; px++) {
        setPixel(px, y, color);
        setPixel(px, y + h - 1, color);
    }

    // Left and right edges
    for (int py = y; py < y + h; py++) {
        setPixel(x, py, color);
        setPixel(x + w - 1, py, color);
    }
}

void Canvas::fillRect(int x, int y, int w, int h, uint32_t color) {
    int endX = std::min(x + w, canvasWidth);
    int endY = std::min(y + h, canvasHeight);
    int startX = std::max(x, 0);
    int startY = std::max(y, 0);

    for (int py = startY; py < endY; py++) {
        for (int px = startX; px < endX; px++) {
            canvasBuffer[py * canvasWidth + px] = color;
        }
    }
}

void Canvas::drawCircle(int centerX, int centerY, int radius, uint32_t color) {
    // Midpoint circle algorithm
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        setPixel(centerX + x, centerY + y, color);
        setPixel(centerX + y, centerY + x, color);
        setPixel(centerX - y, centerY + x, color);
        setPixel(centerX - x, centerY + y, color);
        setPixel(centerX - x, centerY - y, color);
        setPixel(centerX - y, centerY - x, color);
        setPixel(centerX + y, centerY - x, color);
        setPixel(centerX + x, centerY - y, color);

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void Canvas::fillCircle(int centerX, int centerY, int radius, uint32_t color) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        // Draw horizontal lines to fill the circle
        for (int px = centerX - x; px <= centerX + x; px++) {
            setPixel(px, centerY + y, color);
            setPixel(px, centerY - y, color);
        }
        for (int px = centerX - y; px <= centerX + y; px++) {
            setPixel(px, centerY + x, color);
            setPixel(px, centerY - x, color);
        }

        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void Canvas::clear(uint32_t color) {
    for (int i = 0; i < canvasWidth * canvasHeight; i++) {
        canvasBuffer[i] = color;
    }
}

void Canvas::setDrawCallback(std::function<void(Canvas*, uint32_t*, int, int)> callback) {
    drawCallback = callback;
}

void Canvas::setMouseCallback(std::function<void(int, int, bool)> callback) {
    mouseCallback = callback;
}

void Canvas::setBackgroundColor(uint32_t color) {
    backgroundColor = color;
}

void Canvas::setBorderColor(uint32_t color) {
    borderColor = color;
}
