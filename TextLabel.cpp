#include "TextLabel.h"
#include "MiniFB.h"

TextLabel::TextLabel(const std::string& text, int x, int y)
    : Widget(x, y, 0, 0), text(text), textColor(MFB_RGB(0, 0, 0)), autoSize(true) {
}

TextLabel::TextLabel(const std::string& text, int x, int y, int width, int height)
    : Widget(x, y, width, height), text(text), textColor(MFB_RGB(0, 0, 0)), autoSize(false) {
}

void TextLabel::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    if (!fontRenderer || text.empty()) {
        return;
    }

    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    if (autoSize) {
        width = fontRenderer->getTextWidth(text);
        height = fontRenderer->getTextHeight();
    }

    int textY = absY + fontRenderer->getTextHeight();
    fontRenderer->drawText(buffer, bufferWidth, bufferHeight, text, absX, textY, textColor);
}

void TextLabel::setText(const std::string& newText) {
    text = newText;
}

void TextLabel::setTextColor(uint32_t color) {
    textColor = color;
}

void TextLabel::setAutoSize(bool enable) {
    autoSize = enable;
}
