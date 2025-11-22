#include "StatusBar.h"
#include <algorithm>

StatusBar::StatusBar(int height,
                     uint32_t bgColor, uint32_t borderColor)
    : Widget(0, 0, 0, height), anchorBottom(true),
      backgroundColor(bgColor), borderColor(borderColor) {

    panel = new Panel(0, 0, 0, height);
    panel->setParent(this);
    panel->setBackgroundColor(backgroundColor);
    panel->setBorderColor(borderColor);
    panel->setDrawBorder(true);
}

StatusBar::~StatusBar() {
    delete panel;
}

void StatusBar::setFontRenderer(FontRenderer* renderer) {
    Widget::setFontRenderer(renderer);
    panel->setFontRenderer(renderer);
}

void StatusBar::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    panel->draw(buffer, bufferWidth, bufferHeight);
}

void StatusBar::handleMouseMove(int mouseX, int mouseY) {
    panel->handleMouseMove(mouseX, mouseY);
}

void StatusBar::onWindowResize(int windowWidth, int windowHeight) {
    width = windowWidth;
    panel->setSize(windowWidth, height);
    y = windowHeight - height;
    updateSectionPositions();
}

void StatusBar::addSection(int sectionWidth) {
    TextLabel* label = new TextLabel("", 0, 3);
    label->setAutoSize(false);
    panel->add(label);
    sections.push_back(label);
    sectionWidths.push_back(sectionWidth);
    updateSectionPositions();
}

void StatusBar::setSection(int index, const std::string& text) {
    if (index >= 0 && index < static_cast<int>(sections.size())) {
        sections[index]->setText(text);
    }
}

void StatusBar::setSectionColor(int index, uint32_t color) {
    if (index >= 0 && index < static_cast<int>(sections.size())) {
        sections[index]->setTextColor(color);
    }
}

void StatusBar::updateSectionPositions() {
    int currentX = 5;
    for (size_t i = 0; i < sections.size(); i++) {
        sections[i]->setPosition(currentX, 3);
        sections[i]->setSize(sectionWidths[i], height - 6);
        currentX += sectionWidths[i] + 10;
    }
}

void StatusBar::setBackgroundColor(uint32_t color) {
    backgroundColor = color;
    panel->setBackgroundColor(color);
}

void StatusBar::setBorderColor(uint32_t color) {
    borderColor = color;
    panel->setBorderColor(color);
}
