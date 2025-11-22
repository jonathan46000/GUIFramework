#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include "Widget.h"

class ProgressBar : public Widget {
private:
    double value;
    double maxValue;
    uint32_t backgroundColor;
    uint32_t fillColor;
    uint32_t borderColor;

public:
    ProgressBar(int x, int y, int width, int height,
                uint32_t bgColor = 0xFFE0E0E0,
                uint32_t fillColor = 0xFF4CAF50,
                uint32_t borderColor = 0xFF808080);

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;

    void setValue(double val);
    void setMaxValue(double max);
    void setPercentage(double percent);

    double getValue() const { return value; }
    double getMaxValue() const { return maxValue; }
    double getPercentage() const;
};

#endif
