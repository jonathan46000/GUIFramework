#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "BooleanWidget.h"

class CheckBox : public BooleanWidget {
private:
    int boxSize;

public:
    CheckBox(const std::string& label, int x, int y);
    CheckBox(const std::string& label, int x, int y, int boxSize);

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;

    void setBoxSize(int size);
    int getBoxSize() const { return boxSize; }
};

#endif
