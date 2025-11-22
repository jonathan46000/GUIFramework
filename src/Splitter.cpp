#include "Splitter.h"
#include <algorithm>

Splitter::Splitter(int x, int y, int width, int height, SplitterOrientation orientation)
    : Widget(x, y, width, height), orientation(orientation), dividerWidth(6),
      isDragging(false), minPanelSize(50), dividerColor(0xFFC0C0C0),
      dividerHoverColor(0xFF909090), isHoveringDivider(false) {

    if (orientation == SplitterOrientation::HORIZONTAL) {
        dividerPosition = width / 2;
    } else {
        dividerPosition = height / 2;
    }

    firstPanel = new Panel(0, 0, 1, 1);
    secondPanel = new Panel(0, 0, 1, 1);

    firstPanel->setParent(this);
    secondPanel->setParent(this);

    firstPanel->setDrawBorder(true);
    secondPanel->setDrawBorder(true);

    updatePanelSizes();
}

Splitter::~Splitter() {
    delete firstPanel;
    delete secondPanel;
}

void Splitter::updatePanelSizes() {
    if (orientation == SplitterOrientation::HORIZONTAL) {
        firstPanel->setPosition(0, 0);
        firstPanel->setSize(dividerPosition, height);

        secondPanel->setPosition(dividerPosition + dividerWidth, 0);
        secondPanel->setSize(width - dividerPosition - dividerWidth, height);
    } else {
        firstPanel->setPosition(0, 0);
        firstPanel->setSize(width, dividerPosition);

        secondPanel->setPosition(0, dividerPosition + dividerWidth);
        secondPanel->setSize(width, height - dividerPosition - dividerWidth);
    }
}

bool Splitter::isMouseOnDivider(int mouseX, int mouseY) const {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    if (orientation == SplitterOrientation::HORIZONTAL) {
        int dividerAbsX = absX + dividerPosition;
        return mouseX >= dividerAbsX && mouseX < dividerAbsX + dividerWidth &&
               mouseY >= absY && mouseY < absY + height;
    } else {
        int dividerAbsY = absY + dividerPosition;
        return mouseY >= dividerAbsY && mouseY < dividerAbsY + dividerWidth &&
               mouseX >= absX && mouseX < absX + width;
    }
}

void Splitter::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    firstPanel->draw(buffer, bufferWidth, bufferHeight);
    secondPanel->draw(buffer, bufferWidth, bufferHeight);

    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    uint32_t color = isHoveringDivider ? dividerHoverColor : dividerColor;

    if (orientation == SplitterOrientation::HORIZONTAL) {
        int dividerAbsX = absX + dividerPosition;
        int endX = std::min(dividerAbsX + dividerWidth, bufferWidth);
        int endY = std::min(absY + height, bufferHeight);

        for (int py = absY; py < endY; py++) {
            for (int px = dividerAbsX; px < endX; px++) {
                if (px >= 0 && py >= 0 && px < bufferWidth && py < bufferHeight) {
                    buffer[py * bufferWidth + px] = color;
                }
            }
        }
    } else {
        int dividerAbsY = absY + dividerPosition;
        int endX = std::min(absX + width, bufferWidth);
        int endY = std::min(dividerAbsY + dividerWidth, bufferHeight);

        for (int py = dividerAbsY; py < endY; py++) {
            for (int px = absX; px < endX; px++) {
                if (px >= 0 && py >= 0 && px < bufferWidth && py < bufferHeight) {
                    buffer[py * bufferWidth + px] = color;
                }
            }
        }
    }
}

void Splitter::handleMouseMove(int mouseX, int mouseY) {
    isHoveringDivider = isMouseOnDivider(mouseX, mouseY);

    if (isDragging) {
        int absX = getAbsoluteX();
        int absY = getAbsoluteY();

        if (orientation == SplitterOrientation::HORIZONTAL) {
            int newPosition = mouseX - absX - dividerWidth / 2;
            newPosition = std::max(minPanelSize, newPosition);
            newPosition = std::min(width - dividerWidth - minPanelSize, newPosition);

            if (newPosition != dividerPosition) {
                dividerPosition = newPosition;
                updatePanelSizes();
            }
        } else {
            int newPosition = mouseY - absY - dividerWidth / 2;
            newPosition = std::max(minPanelSize, newPosition);
            newPosition = std::min(height - dividerWidth - minPanelSize, newPosition);

            if (newPosition != dividerPosition) {
                dividerPosition = newPosition;
                updatePanelSizes();
            }
        }
    }

    if (!isDragging) {
        firstPanel->handleMouseMove(mouseX, mouseY);
        secondPanel->handleMouseMove(mouseX, mouseY);
    }
}

void Splitter::handleMouseButton(int mouseX, int mouseY, bool isPressed) {
    if (isPressed) {
        if (isMouseOnDivider(mouseX, mouseY)) {
            isDragging = true;
            return;
        }
    } else {
        isDragging = false;
    }
}

void Splitter::handleChar(unsigned int charCode) {
    firstPanel->handleChar(charCode);
    secondPanel->handleChar(charCode);
}

void Splitter::handleKey(int key, bool isPressed) {
    firstPanel->handleKey(key, isPressed);
    secondPanel->handleKey(key, isPressed);
}

void Splitter::setFontRenderer(FontRenderer* renderer) {
    Widget::setFontRenderer(renderer);
    firstPanel->setFontRenderer(renderer);
    secondPanel->setFontRenderer(renderer);
}

void Splitter::setDividerPosition(int position) {
    if (orientation == SplitterOrientation::HORIZONTAL) {
        position = std::max(minPanelSize, position);
        position = std::min(width - dividerWidth - minPanelSize, position);
    } else {
        position = std::max(minPanelSize, position);
        position = std::min(height - dividerWidth - minPanelSize, position);
    }
    dividerPosition = position;
    updatePanelSizes();
}

void Splitter::setMinPanelSize(int size) {
    minPanelSize = size;
    updatePanelSizes();
}

void Splitter::setDividerColor(uint32_t color) {
    dividerColor = color;
}

void Splitter::setDividerHoverColor(uint32_t color) {
    dividerHoverColor = color;
}

void Splitter::setDividerWidth(int width) {
    dividerWidth = width;
    updatePanelSizes();
}
