#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <cstdint>
#include <string>

class ImageLoader {
public:
    ImageLoader();
    ~ImageLoader();

    bool loadPNG(const char* filepath);
    bool loadGIF(const char* filepath);

    uint32_t* getPixelData() const { return pixelData; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void freePixelData();

private:
    uint32_t* pixelData;
    int width;
    int height;

    bool parsePNG(const uint8_t* data, size_t dataSize);
    bool parseGIF(const uint8_t* data, size_t dataSize);

    uint32_t readBigEndian32(const uint8_t* data);
    uint16_t readLittleEndian16(const uint8_t* data);

    bool decompressIDAT(const uint8_t* compressedData, size_t compressedSize,
                        uint8_t** decompressed, size_t* decompressedSize);
    void unfilterPNG(uint8_t* imageData, int width, int height, int bytesPerPixel);
    void convertToRGBA(const uint8_t* rawData, int colorType, int bitDepth);

    bool parseGIFColorTable(const uint8_t* data, int numColors, uint32_t* colorTable);
    bool parseGIFImageData(const uint8_t* data, size_t dataSize, size_t offset,
                           const uint32_t* colorTable, int colorTableSize);
    bool decompressLZW(const uint8_t* data, size_t dataSize, int minCodeSize,
                       uint8_t** output, size_t* outputSize);
};

#endif
