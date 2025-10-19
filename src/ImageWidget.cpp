#include "ImageWidget.h"
#include <algorithm>
#include <cstring>

ImageWidget::ImageWidget(int x, int y, int width, int height)
    : Widget(x, y, width, height), imageLoader(nullptr), ownsLoader(false),
      maintainAspectRatio(true), backgroundColor(0xFFE0E0E0) {
}

ImageWidget::ImageWidget(int x, int y, int width, int height, const std::string& filepath)
    : Widget(x, y, width, height), imageLoader(nullptr), ownsLoader(true),
      maintainAspectRatio(true), backgroundColor(0xFFE0E0E0) {
    loadImage(filepath);
}

ImageWidget::~ImageWidget() {
    if (ownsLoader && imageLoader) {
        delete imageLoader;
    }
}

bool ImageWidget::loadImage(const std::string& filepath) {
    if (ownsLoader && imageLoader) {
        delete imageLoader;
    }

    imageLoader = new ImageLoader();
    ownsLoader = true;

    if (filepath.size() >= 4) {
        std::string ext = filepath.substr(filepath.size() - 4);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".png") {
            return imageLoader->loadPNG(filepath.c_str());
        } else if (ext == ".gif") {
            return imageLoader->loadGIF(filepath.c_str());
        }
    }

    if (imageLoader->loadPNG(filepath.c_str())) {
        return true;
    }

    if (imageLoader->loadGIF(filepath.c_str())) {
        return true;
    }

    delete imageLoader;
    imageLoader = nullptr;
    ownsLoader = false;
    return false;
}

void ImageWidget::setImageLoader(ImageLoader* loader, bool takeOwnership) {
    if (ownsLoader && imageLoader) {
        delete imageLoader;
    }

    imageLoader = loader;
    ownsLoader = takeOwnership;
}

void ImageWidget::clearImage() {
    if (ownsLoader && imageLoader) {
        delete imageLoader;
    }
    imageLoader = nullptr;
    ownsLoader = false;
}

void ImageWidget::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int endX = std::min(absX + width, bufferWidth);
    int endY = std::min(absY + height, bufferHeight);

    for (int py = absY; py < endY; py++) {
        for (int px = absX; px < endX; px++) {
            if (px >= 0 && py >= 0) {
                buffer[py * bufferWidth + px] = backgroundColor;
            }
        }
    }

    if (!imageLoader || !imageLoader->getPixelData()) {
        return;
    }

    int imgWidth = imageLoader->getWidth();
    int imgHeight = imageLoader->getHeight();
    uint32_t* imgData = imageLoader->getPixelData();

    int drawWidth = width;
    int drawHeight = height;
    int offsetX = 0;
    int offsetY = 0;

    if (maintainAspectRatio) {
        float aspectRatio = (float)imgWidth / (float)imgHeight;
        float widgetAspectRatio = (float)width / (float)height;

        if (aspectRatio > widgetAspectRatio) {
            drawWidth = width;
            drawHeight = (int)(width / aspectRatio);
            offsetY = (height - drawHeight) / 2;
        } else {
            drawHeight = height;
            drawWidth = (int)(height * aspectRatio);
            offsetX = (width - drawWidth) / 2;
        }
    }

    for (int py = 0; py < drawHeight; py++) {
        for (int px = 0; px < drawWidth; px++) {
            int screenX = absX + offsetX + px;
            int screenY = absY + offsetY + py;

            if (screenX >= absX && screenX < endX && screenY >= absY && screenY < endY &&
                screenX >= 0 && screenY >= 0 && screenX < bufferWidth && screenY < bufferHeight) {

                int imgX = (px * imgWidth) / drawWidth;
                int imgY = (py * imgHeight) / drawHeight;

                if (imgX >= 0 && imgX < imgWidth && imgY >= 0 && imgY < imgHeight) {
                    uint32_t pixel = imgData[imgY * imgWidth + imgX];
                    uint8_t alpha = (pixel >> 24) & 0xFF;

                    if (alpha == 255) {
                        buffer[screenY * bufferWidth + screenX] = pixel;
                    } else if (alpha > 0) {
                        uint32_t bgPixel = buffer[screenY * bufferWidth + screenX];

                        uint8_t srcR = (pixel >> 16) & 0xFF;
                        uint8_t srcG = (pixel >> 8) & 0xFF;
                        uint8_t srcB = pixel & 0xFF;

                        uint8_t bgR = (bgPixel >> 16) & 0xFF;
                        uint8_t bgG = (bgPixel >> 8) & 0xFF;
                        uint8_t bgB = bgPixel & 0xFF;

                        float a = alpha / 255.0f;
                        uint8_t finalR = (uint8_t)(srcR * a + bgR * (1 - a));
                        uint8_t finalG = (uint8_t)(srcG * a + bgG * (1 - a));
                        uint8_t finalB = (uint8_t)(srcB * a + bgB * (1 - a));

                        buffer[screenY * bufferWidth + screenX] =
                            0xFF000000 | (finalR << 16) | (finalG << 8) | finalB;
                    }
                }
            }
        }
    }
}
