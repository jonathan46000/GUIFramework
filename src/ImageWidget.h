#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include "Widget.h"
#include "ImageLoader.h"
#include <string>

class ImageWidget : public Widget {
private:
    ImageLoader* imageLoader;
    bool ownsLoader;
    bool maintainAspectRatio;
    uint32_t backgroundColor;

public:
    ImageWidget(int x, int y, int width, int height);
    ImageWidget(int x, int y, int width, int height, const std::string& filepath);
    ~ImageWidget();

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;

    bool loadImage(const std::string& filepath);
    void setImageLoader(ImageLoader* loader, bool takeOwnership = false);
    void clearImage();

    void setMaintainAspectRatio(bool maintain) { maintainAspectRatio = maintain; }
    void setBackgroundColor(uint32_t color) { backgroundColor = color; }

    ImageLoader* getImageLoader() const { return imageLoader; }
    bool hasImage() const { return imageLoader && imageLoader->getPixelData(); }
};

#endif
