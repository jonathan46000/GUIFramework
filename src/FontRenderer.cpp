#include "FontRenderer.h"
#include <algorithm>

FontRenderer::FontRenderer() : library(nullptr), face(nullptr), fontSize(12) {
    FT_Init_FreeType(&library);
}

FontRenderer::~FontRenderer() {
    if (face) {
        FT_Done_Face(face);
    }
    if (library) {
        FT_Done_FreeType(library);
    }
}

bool FontRenderer::loadFont(const char* fontPath, int size) {
    fontSize = size;

    if (FT_New_Face(library, fontPath, 0, &face)) {
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    return true;
}

void FontRenderer::drawText(uint32_t* buffer, int bufferWidth, int bufferHeight,
                           const std::string& text, int x, int y, uint32_t color) {
    if (!face) {
        return;
    }

    int cursorX = x;

    for (char c : text) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            continue;
        }

        FT_GlyphSlot slot = face->glyph;
        FT_Bitmap bitmap = slot->bitmap;

        int bitmapX = cursorX + slot->bitmap_left;
        int bitmapY = y - slot->bitmap_top;

        for (unsigned int row = 0; row < bitmap.rows; row++) {
            for (unsigned int col = 0; col < bitmap.width; col++) {
                int px = bitmapX + col;
                int py = bitmapY + row;

                if (px >= 0 && px < bufferWidth && py >= 0 && py < bufferHeight) {
                    unsigned char alpha = bitmap.buffer[row * bitmap.width + col];

                    if (alpha > 0) {
                        uint32_t existingColor = buffer[py * bufferWidth + px];

                        int r = (color & 0xFF0000) >> 16;
                        int g = (color & 0x00FF00) >> 8;
                        int b = (color & 0x0000FF);

                        int bgR = (existingColor & 0xFF0000) >> 16;
                        int bgG = (existingColor & 0x00FF00) >> 8;
                        int bgB = (existingColor & 0x0000FF);

                        float a = alpha / 255.0f;
                        int finalR = (int)(r * a + bgR * (1 - a));
                        int finalG = (int)(g * a + bgG * (1 - a));
                        int finalB = (int)(b * a + bgB * (1 - a));

                        buffer[py * bufferWidth + px] = 0xFF000000 | (finalR << 16) | (finalG << 8) | finalB;
                    }
                }
            }
        }

        cursorX += slot->advance.x >> 6;
    }
}

int FontRenderer::getTextWidth(const std::string& text) {
    if (!face) return 0;

    int width = 0;
    for (char c : text) {
        if (FT_Load_Char(face, c, FT_LOAD_DEFAULT)) {
            continue;
        }
        width += face->glyph->advance.x >> 6;
    }
    return width;
}

int FontRenderer::getTextHeight() {
    return fontSize;
}
