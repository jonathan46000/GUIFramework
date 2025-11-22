#ifndef SPINNER_H
#define SPINNER_H

#include "Widget.h"
#include "TextBox.h"
#include "PushButton.h"
#include <functional>

class Spinner : public Widget {
private:
    TextBox* textBox;
    PushButton* upButton;
    PushButton* downButton;
    double value;
    double minValue;
    double maxValue;
    double step;
    std::function<void(double)> changeCallback;

    void updateDisplay();
    void incrementValue();
    void decrementValue();

public:
    Spinner(int x, int y, int width, int height);
    ~Spinner();

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void handleMouseMove(int mouseX, int mouseY) override;
    void handleMouseButton(int mouseX, int mouseY, bool isPressed) override;
    void handleChar(unsigned int charCode) override;
    void handleKey(int key, bool isPressed) override;
    void setFontRenderer(FontRenderer* renderer) override;

    void setValue(double val);
    void setMinValue(double min);
    void setMaxValue(double max);
    void setStep(double s);
    void setChangeCallback(std::function<void(double)> callback);

    double getValue() const { return value; }
    double getMinValue() const { return minValue; }
    double getMaxValue() const { return maxValue; }
    double getStep() const { return step; }

    TextBox* getTextBox() const { return textBox; }
    PushButton* getUpButton() const { return upButton; }
    PushButton* getDownButton() const { return downButton; }
};

#endif
