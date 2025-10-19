#include "ImageLoader.h"
#include <fstream>
#include <vector>
#include <cstring>
#include <zlib.h>
#include <iostream>

ImageLoader::ImageLoader() : pixelData(nullptr), width(0), height(0) {}

ImageLoader::~ImageLoader() {
    freePixelData();
}

void ImageLoader::freePixelData() {
    if (pixelData) {
        delete[] pixelData;
        pixelData = nullptr;
    }
    width = 0;
    height = 0;
}

bool ImageLoader::loadPNG(const char* filepath) {
    freePixelData();

    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open PNG file: " << filepath << std::endl;
        return false;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> fileData(fileSize);
    if (!file.read(reinterpret_cast<char*>(fileData.data()), fileSize)) {
        std::cerr << "Failed to read PNG file" << std::endl;
        return false;
    }

    return parsePNG(fileData.data(), fileData.size());
}

bool ImageLoader::loadGIF(const char* filepath) {
    freePixelData();

    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open GIF file: " << filepath << std::endl;
        return false;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> fileData(fileSize);
    if (!file.read(reinterpret_cast<char*>(fileData.data()), fileSize)) {
        std::cerr << "Failed to read GIF file" << std::endl;
        return false;
    }

    return parseGIF(fileData.data(), fileData.size());
}

uint32_t ImageLoader::readBigEndian32(const uint8_t* data) {
    return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
}

uint16_t ImageLoader::readLittleEndian16(const uint8_t* data) {
    return data[0] | (data[1] << 8);
}

bool ImageLoader::decompressIDAT(const uint8_t* compressedData, size_t compressedSize,
                                  uint8_t** decompressed, size_t* decompressedSize) {
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = compressedSize;
    stream.next_in = const_cast<uint8_t*>(compressedData);

    if (inflateInit(&stream) != Z_OK) {
        std::cerr << "Failed to initialize zlib" << std::endl;
        return false;
    }

    std::vector<uint8_t> output;
    uint8_t buffer[32768];

    int ret;
    do {
        stream.avail_out = sizeof(buffer);
        stream.next_out = buffer;
        ret = inflate(&stream, Z_NO_FLUSH);

        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
            inflateEnd(&stream);
            std::cerr << "Zlib decompression error: " << ret << std::endl;
            return false;
        }

        size_t have = sizeof(buffer) - stream.avail_out;
        output.insert(output.end(), buffer, buffer + have);
    } while (ret != Z_STREAM_END);

    inflateEnd(&stream);

    *decompressedSize = output.size();
    *decompressed = new uint8_t[*decompressedSize];
    std::memcpy(*decompressed, output.data(), *decompressedSize);

    return true;
}

void ImageLoader::unfilterPNG(uint8_t* imageData, int width, int height, int bytesPerPixel) {
    int stride = width * bytesPerPixel;

    for (int y = 0; y < height; y++) {
        int rowOffset = y * (stride + 1);
        uint8_t filterType = imageData[rowOffset];
        uint8_t* row = &imageData[rowOffset + 1];

        for (int x = 0; x < stride; x++) {
            uint8_t left = (x >= bytesPerPixel) ? row[x - bytesPerPixel] : 0;
            uint8_t above = (y > 0) ? imageData[(y - 1) * (stride + 1) + 1 + x] : 0;
            uint8_t upperLeft = (x >= bytesPerPixel && y > 0) ?
                                imageData[(y - 1) * (stride + 1) + 1 + x - bytesPerPixel] : 0;

            switch (filterType) {
                case 0:
                    break;
                case 1:
                    row[x] += left;
                    break;
                case 2:
                    row[x] += above;
                    break;
                case 3:
                    row[x] += (left + above) / 2;
                    break;
                case 4: {
                    int p = left + above - upperLeft;
                    int pa = abs(p - left);
                    int pb = abs(p - above);
                    int pc = abs(p - upperLeft);
                    if (pa <= pb && pa <= pc)
                        row[x] += left;
                    else if (pb <= pc)
                        row[x] += above;
                    else
                        row[x] += upperLeft;
                    break;
                }
            }
        }
    }
}

void ImageLoader::convertToRGBA(const uint8_t* rawData, int colorType, int bitDepth) {
    (void)bitDepth;

    pixelData = new uint32_t[width * height];

    int stride = width * (colorType == 6 ? 4 : 3);

    for (int y = 0; y < height; y++) {
        int rowOffset = y * (stride + 1) + 1;
        for (int x = 0; x < width; x++) {
            int pixelIdx = y * width + x;

            if (colorType == 6) {
                int dataIdx = rowOffset + x * 4;
                uint8_t r = rawData[dataIdx];
                uint8_t g = rawData[dataIdx + 1];
                uint8_t b = rawData[dataIdx + 2];
                uint8_t a = rawData[dataIdx + 3];
                pixelData[pixelIdx] = (a << 24) | (r << 16) | (g << 8) | b;
            } else if (colorType == 2) {
                int dataIdx = rowOffset + x * 3;
                uint8_t r = rawData[dataIdx];
                uint8_t g = rawData[dataIdx + 1];
                uint8_t b = rawData[dataIdx + 2];
                pixelData[pixelIdx] = 0xFF000000 | (r << 16) | (g << 8) | b;
            }
        }
    }
}

bool ImageLoader::parsePNG(const uint8_t* data, size_t dataSize) {
    if (dataSize < 8) return false;

    const uint8_t pngSignature[] = {137, 80, 78, 71, 13, 10, 26, 10};
    if (std::memcmp(data, pngSignature, 8) != 0) {
        std::cerr << "Invalid PNG signature" << std::endl;
        return false;
    }

    size_t offset = 8;
    std::vector<uint8_t> idatData;
    int colorType = 0;
    int bitDepth = 0;

    while (offset + 12 <= dataSize) {
        uint32_t chunkLength = readBigEndian32(data + offset);
        offset += 4;

        if (offset + 4 + chunkLength + 4 > dataSize) break;

        char chunkType[5] = {0};
        std::memcpy(chunkType, data + offset, 4);
        offset += 4;

        if (std::strcmp(chunkType, "IHDR") == 0) {
            if (chunkLength < 13) return false;
            width = readBigEndian32(data + offset);
            height = readBigEndian32(data + offset + 4);
            bitDepth = data[offset + 8];
            colorType = data[offset + 9];
        } else if (std::strcmp(chunkType, "IDAT") == 0) {
            idatData.insert(idatData.end(), data + offset, data + offset + chunkLength);
        } else if (std::strcmp(chunkType, "IEND") == 0) {
            break;
        }

        offset += chunkLength + 4;
    }

    if (width == 0 || height == 0 || idatData.empty()) {
        std::cerr << "Invalid PNG data" << std::endl;
        return false;
    }

    uint8_t* decompressed = nullptr;
    size_t decompressedSize = 0;
    if (!decompressIDAT(idatData.data(), idatData.size(), &decompressed, &decompressedSize)) {
        return false;
    }

    int bytesPerPixel = (colorType == 6) ? 4 : 3;
    unfilterPNG(decompressed, width, height, bytesPerPixel);
    convertToRGBA(decompressed, colorType, bitDepth);

    delete[] decompressed;
    return true;
}

bool ImageLoader::parseGIFColorTable(const uint8_t* data, int numColors, uint32_t* colorTable) {
    for (int i = 0; i < numColors; i++) {
        uint8_t r = data[i * 3];
        uint8_t g = data[i * 3 + 1];
        uint8_t b = data[i * 3 + 2];
        colorTable[i] = 0xFF000000 | (r << 16) | (g << 8) | b;
    }
    return true;
}

bool ImageLoader::decompressLZW(const uint8_t* data, size_t dataSize, int minCodeSize,
                                uint8_t** output, size_t* outputSize) {
    std::vector<uint8_t> result;

    int clearCode = 1 << minCodeSize;
    int endCode = clearCode + 1;
    int nextCode = endCode + 1;
    int codeSize = minCodeSize + 1;
    int maxCode = (1 << codeSize) - 1;

    std::vector<std::vector<uint8_t>> table(4096);
    for (int i = 0; i < clearCode; i++) {
        table[i].push_back(i);
    }

    uint32_t bits = 0;
    int bitsAvail = 0;
    size_t bytePos = 0;
    int prevCode = -1;

    while (bytePos < dataSize) {
        while (bitsAvail < codeSize && bytePos < dataSize) {
            bits |= (data[bytePos++] << bitsAvail);
            bitsAvail += 8;
        }

        if (bitsAvail < codeSize) break;

        int code = bits & ((1 << codeSize) - 1);
        bits >>= codeSize;
        bitsAvail -= codeSize;

        if (code == endCode) break;

        if (code == clearCode) {
            nextCode = endCode + 1;
            codeSize = minCodeSize + 1;
            maxCode = (1 << codeSize) - 1;
            prevCode = -1;
            continue;
        }

        if (code < nextCode) {
            result.insert(result.end(), table[code].begin(), table[code].end());

            if (prevCode >= 0 && nextCode < 4096) {
                table[nextCode] = table[prevCode];
                table[nextCode].push_back(table[code][0]);
                nextCode++;
            }
        } else if (code == nextCode) {
            if (prevCode >= 0 && nextCode < 4096) {
                table[nextCode] = table[prevCode];
                table[nextCode].push_back(table[prevCode][0]);
                result.insert(result.end(), table[nextCode].begin(), table[nextCode].end());
                nextCode++;
            }
        }

        prevCode = code;

        if (nextCode > maxCode && codeSize < 12) {
            codeSize++;
            maxCode = (1 << codeSize) - 1;
        }
    }

    *outputSize = result.size();
    *output = new uint8_t[*outputSize];
    std::memcpy(*output, result.data(), *outputSize);

    return true;
}

bool ImageLoader::parseGIFImageData(const uint8_t* data, size_t dataSize, size_t offset,
                                     const uint32_t* colorTable, int colorTableSize) {
    if (offset + 10 > dataSize) return false;

    uint16_t left = readLittleEndian16(data + offset);
    uint16_t top = readLittleEndian16(data + offset + 2);
    uint16_t imgWidth = readLittleEndian16(data + offset + 4);
    uint16_t imgHeight = readLittleEndian16(data + offset + 6);
    uint8_t flags = data[offset + 8];

    (void)left;
    (void)top;
    (void)flags;

    offset += 9;

    if (offset >= dataSize) return false;
    uint8_t lzwMinCodeSize = data[offset++];

    std::vector<uint8_t> imageData;
    while (offset < dataSize) {
        uint8_t blockSize = data[offset++];
        if (blockSize == 0) break;
        if (offset + blockSize > dataSize) return false;
        imageData.insert(imageData.end(), data + offset, data + offset + blockSize);
        offset += blockSize;
    }

    uint8_t* decompressed = nullptr;
    size_t decompressedSize = 0;
    if (!decompressLZW(imageData.data(), imageData.size(), lzwMinCodeSize, &decompressed, &decompressedSize)) {
        std::cerr << "LZW decompression failed" << std::endl;
        return false;
    }

    pixelData = new uint32_t[imgWidth * imgHeight];
    for (int i = 0; i < imgWidth * imgHeight && i < (int)decompressedSize; i++) {
        uint8_t colorIndex = decompressed[i];
        if (colorIndex < colorTableSize) {
            pixelData[i] = colorTable[colorIndex];
        } else {
            pixelData[i] = 0xFF000000;
        }
    }

    delete[] decompressed;
    return true;
}

bool ImageLoader::parseGIF(const uint8_t* data, size_t dataSize) {
    if (dataSize < 13) return false;

    if (std::memcmp(data, "GIF87a", 6) != 0 && std::memcmp(data, "GIF89a", 6) != 0) {
        std::cerr << "Invalid GIF signature" << std::endl;
        return false;
    }

    width = readLittleEndian16(data + 6);
    height = readLittleEndian16(data + 8);
    uint8_t flags = data[10];

    size_t offset = 13;

    uint32_t globalColorTable[256] = {0};
    int globalColorTableSize = 0;

    if (flags & 0x80) {
        globalColorTableSize = 1 << ((flags & 0x07) + 1);
        if (offset + globalColorTableSize * 3 > dataSize) return false;
        parseGIFColorTable(data + offset, globalColorTableSize, globalColorTable);
        offset += globalColorTableSize * 3;
    }

    while (offset < dataSize) {
        uint8_t separator = data[offset++];

        if (separator == 0x2C) {
            return parseGIFImageData(data, dataSize, offset, globalColorTable, globalColorTableSize);
        } else if (separator == 0x21) {
            if (offset >= dataSize) break;
            uint8_t label = data[offset++];
            (void)label;

            while (offset < dataSize) {
                uint8_t blockSize = data[offset++];
                if (blockSize == 0) break;
                offset += blockSize;
            }
        } else if (separator == 0x3B) {
            break;
        }
    }

    std::cerr << "No image data found in GIF" << std::endl;
    return false;
}
