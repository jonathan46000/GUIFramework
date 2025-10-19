#ifndef FONTRENDERER_H
#define FONTRENDERER_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <cstdint>

class FontRenderer {
private:
    FT_Library library;
    FT_Face face;
    int fontSize;

public:
    FontRenderer();
    ~FontRenderer();

    bool loadFont(const char* fontPath, int size);
    void drawText(uint32_t* buffer, int bufferWidth, int bufferHeight,
                  const std::string& text, int x, int y, uint32_t color);
    int getTextWidth(const std::string& text);
    int getTextHeight();
};

#endif
