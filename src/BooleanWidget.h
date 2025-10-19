#ifndef BOOLEANWIDGET_H
#define BOOLEANWIDGET_H

#include "Widget.h"
#include <string>
#include <functional>

class BooleanWidget : public Widget {
protected:
    std::string label;
    bool isChecked;
    bool isHovered;
    uint32_t backgroundColor;
    uint32_t hoverColor;
    uint32_t checkColor;
    uint32_t borderColor;
    uint32_t textColor;
    std::function<void(bool)> changeCallback;

public:
    BooleanWidget(const std::string& label, int x, int y, int width, int height);
    virtual ~BooleanWidget();

    virtual void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) = 0;
    void checkHover(int mouseX, int mouseY) override;

    virtual void toggle();
    void setChecked(bool checked);
    bool getChecked() const { return isChecked; }

    void setChangeCallback(std::function<void(bool)> callback);
    void setLabel(const std::string& newLabel);
    void setBackgroundColor(uint32_t color);
    void setHoverColor(uint32_t color);
    void setCheckColor(uint32_t color);
    void setBorderColor(uint32_t color);
    void setTextColor(uint32_t color);

    const std::string& getLabel() const { return label; }

protected:
    void notifyChange();
};

#endif
