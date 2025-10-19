#ifndef TEXTLABEL_H
#define TEXTLABEL_H

#include "Widget.h"
#include <string>

class TextLabel : public Widget {
private:
    std::string text;
    uint32_t textColor;
    bool autoSize;

public:
    TextLabel(const std::string& text, int x, int y);
    TextLabel(const std::string& text, int x, int y, int width, int height);

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;

    void setText(const std::string& newText);
    void setTextColor(uint32_t color);
    void setAutoSize(bool enable);

    const std::string& getText() const { return text; }
};

#endif
