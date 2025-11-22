#include "Spinner.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

Spinner::Spinner(int x, int y, int width, int height)
    : Widget(x, y, width, height), value(0.0), minValue(0.0),
      maxValue(100.0), step(1.0), changeCallback(nullptr) {

    int buttonWidth = 25;
    int textBoxWidth = width - buttonWidth;

    textBox = new TextBox(0, 0, textBoxWidth, height);
    textBox->setParent(this);

    upButton = new PushButton("+", textBoxWidth, 0, buttonWidth, height / 2);
    upButton->setParent(this);

    downButton = new PushButton("-", textBoxWidth, height / 2, buttonWidth, height / 2);
    downButton->setParent(this);

    upButton->setClickCallback([this]() {
        incrementValue();
    });

    downButton->setClickCallback([this]() {
        decrementValue();
    });

    textBox->setChangeCallback([this](const std::string& text) {
        try {
            double newValue = std::stod(text);
            newValue = std::max(minValue, std::min(maxValue, newValue));
            if (std::abs(value - newValue) > 0.0001) {
                value = newValue;
                if (changeCallback) {
                    changeCallback(value);
                }
            }
        } catch (...) {
            updateDisplay();
        }
    });

    updateDisplay();
}

Spinner::~Spinner() {
    delete textBox;
    delete upButton;
    delete downButton;
}

void Spinner::setFontRenderer(FontRenderer* renderer) {
    Widget::setFontRenderer(renderer);
    textBox->setFontRenderer(renderer);
    upButton->setFontRenderer(renderer);
    downButton->setFontRenderer(renderer);
}

void Spinner::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    textBox->draw(buffer, bufferWidth, bufferHeight);
    upButton->draw(buffer, bufferWidth, bufferHeight);
    downButton->draw(buffer, bufferWidth, bufferHeight);
}

void Spinner::handleMouseMove(int mouseX, int mouseY) {
    textBox->handleMouseMove(mouseX, mouseY);
    upButton->handleMouseMove(mouseX, mouseY);
    downButton->handleMouseMove(mouseX, mouseY);
}

void Spinner::handleMouseButton(int mouseX, int mouseY, bool isPressed) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int buttonWidth = 25;
    int textBoxWidth = width - buttonWidth;
    int buttonX = absX + textBoxWidth;

    if (isPressed) {
        // Check if clicking up button
        if (mouseX >= buttonX && mouseX < absX + width &&
            mouseY >= absY && mouseY < absY + height / 2) {
            incrementValue();
            return;
        }

        // Check if clicking down button
        if (mouseX >= buttonX && mouseX < absX + width &&
            mouseY >= absY + height / 2 && mouseY < absY + height) {
            decrementValue();
            return;
        }

        // Check if clicking text box
        if (textBox->checkClick(mouseX, mouseY)) {
            textBox->handleMouseButton(mouseX, mouseY, isPressed);
        }
    } else {
        textBox->handleMouseButton(mouseX, mouseY, isPressed);
    }
}

void Spinner::handleChar(unsigned int charCode) {
    textBox->handleChar(charCode);
}

void Spinner::handleKey(int key, bool isPressed) {
    textBox->handleKey(key, isPressed);
}

void Spinner::updateDisplay() {
    std::ostringstream oss;
    if (step >= 1.0) {
        oss << static_cast<int>(std::round(value));
    } else {
        int precision = static_cast<int>(std::ceil(-std::log10(step)));
        oss << std::fixed << std::setprecision(precision) << value;
    }
    textBox->setText(oss.str());
}

void Spinner::incrementValue() {
    double newValue = value + step;
    if (newValue <= maxValue) {
        value = newValue;
        updateDisplay();
        if (changeCallback) {
            changeCallback(value);
        }
    }
}

void Spinner::decrementValue() {
    double newValue = value - step;
    if (newValue >= minValue) {
        value = newValue;
        updateDisplay();
        if (changeCallback) {
            changeCallback(value);
        }
    }
}

void Spinner::setValue(double val) {
    value = std::max(minValue, std::min(maxValue, val));
    updateDisplay();
}

void Spinner::setMinValue(double min) {
    minValue = min;
    if (value < minValue) {
        value = minValue;
        updateDisplay();
    }
}

void Spinner::setMaxValue(double max) {
    maxValue = max;
    if (value > maxValue) {
        value = maxValue;
        updateDisplay();
    }
}

void Spinner::setStep(double s) {
    step = s;
}

void Spinner::setChangeCallback(std::function<void(double)> callback) {
    changeCallback = callback;
}
