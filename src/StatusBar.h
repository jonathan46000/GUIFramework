#ifndef STATUSBAR_H
#define STATUSBAR_H

#include "Widget.h"
#include "Panel.h"
#include "TextLabel.h"
#include <vector>
#include <string>

class StatusBar : public Widget {
private:
    Panel* panel;
    std::vector<TextLabel*> sections;
    std::vector<int> sectionWidths;
    bool anchorBottom;
    uint32_t backgroundColor;
    uint32_t borderColor;

    void updateSectionPositions();

public:
    StatusBar(int height = 25,
              uint32_t bgColor = 0xFFD0D0D0,
              uint32_t borderColor = 0xFF808080);
    ~StatusBar();

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void handleMouseMove(int mouseX, int mouseY) override;
    void setFontRenderer(FontRenderer* renderer) override;

    void addSection(int width);
    void setSection(int index, const std::string& text);
    void setSectionColor(int index, uint32_t color);

    void onWindowResize(int windowWidth, int windowHeight);
    void setBackgroundColor(uint32_t color);
    void setBorderColor(uint32_t color);
};

#endif
