#ifndef RADIOBUTTON_H
#define RADIOBUTTON_H

#include "BooleanWidget.h"
#include <vector>

class RadioButton : public BooleanWidget {
private:
    int circleSize;
    std::vector<RadioButton*>* group;

public:
    RadioButton(const std::string& label, int x, int y);
    RadioButton(const std::string& label, int x, int y, int circleSize);

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void toggle() override;

    void setGroup(std::vector<RadioButton*>* radioGroup);
    void setCircleSize(int size);
    int getCircleSize() const { return circleSize; }
};

#endif
