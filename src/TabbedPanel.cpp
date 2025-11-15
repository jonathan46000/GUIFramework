#include "TabbedPanel.h"
#include <algorithm>
#include <iostream>

TabbedPanel::TabbedPanel(int x, int y, int width, int height,
                         int headerHeight,
                         uint32_t activeBg,
                         uint32_t inactiveBg,
                         uint32_t border,
                         uint32_t text,
                         uint32_t contentBg)
    : Widget(x, y, width, height),
      activeIndex(-1),
      headerHeight(headerHeight),
      activeBgColor(activeBg),
      inactiveBgColor(inactiveBg),
      borderColor(border),
      textColor(text),
      contentBgColor(contentBg) {
}

TabbedPanel::~TabbedPanel() {
    for (Panel* panel : contentPanels) {
        delete panel;
    }
}

void TabbedPanel::addTab(const std::string& name, Panel* panel) {
    tabNames.push_back(name);
    contentPanels.push_back(panel);

    // Position panel in content area
    panel->setPosition(1, headerHeight + 1);
    panel->setSize(width - 2, height - headerHeight - 2);
    panel->setParent(this);

    if (fontRenderer) {
        panel->setFontRenderer(fontRenderer);
    }

    // First tab becomes active
    if (activeIndex == -1) {
        activeIndex = 0;
    }
}

void TabbedPanel::switchToTab(int index) {
    if (index >= 0 && index < static_cast<int>(contentPanels.size())) {
        activeIndex = index;
    }
}

Panel* TabbedPanel::getActivePanel() const {
    if (activeIndex >= 0 && activeIndex < static_cast<int>(contentPanels.size())) {
        return contentPanels[activeIndex];
    }
    return nullptr;
}

int TabbedPanel::getTabWidth() const {
    if (tabNames.empty()) return 0;
    return width / static_cast<int>(tabNames.size());
}

int TabbedPanel::getClickedTab(int mouseX, int mouseY) const {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    // Check if in header area
    if (mouseY < absY || mouseY >= absY + headerHeight) {
        return -1;
    }
    if (mouseX < absX || mouseX >= absX + width) {
        return -1;
    }

    int tabWidth = getTabWidth();
    if (tabWidth == 0) return -1;

    int index = (mouseX - absX) / tabWidth;
    if (index >= 0 && index < static_cast<int>(tabNames.size())) {
        return index;
    }
    return -1;
}

void TabbedPanel::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int endX = std::min(absX + width, bufferWidth);
    int endY = std::min(absY + height, bufferHeight);

    static bool printed = false;
    if (!printed) {
        std::cout << "TabbedPanel draw: absX=" << absX << " absY=" << absY
                  << " width=" << width << " height=" << height
                  << " endX=" << endX << " endY=" << endY << std::endl;
        printed = true;
    }

    // Draw content background
    for (int py = absY; py < endY; py++) {
        for (int px = absX; px < endX; px++) {
            if (px >= 0 && py >= 0) {
                if (py == absY || py == endY - 1 || px == absX || px == endX - 1) {
                    buffer[py * bufferWidth + px] = borderColor;
                } else {
                    buffer[py * bufferWidth + px] = contentBgColor;
                }
            }
        }
    }

    // Draw tab headers
    if (!tabNames.empty()) {
        int tabWidth = getTabWidth();

        for (size_t i = 0; i < tabNames.size(); i++) {
            int tabX = absX + i * tabWidth;
            int tabEndX = std::min(tabX + tabWidth, endX);
            int headerEndY = std::min(absY + headerHeight, bufferHeight);

            uint32_t bgColor = (static_cast<int>(i) == activeIndex) ? activeBgColor : inactiveBgColor;

            for (int py = absY; py < headerEndY; py++) {
                for (int px = tabX; px < tabEndX; px++) {
                    if (px >= 0 && py >= 0) {
                        // Draw borders
                        if (px == tabX || px == tabEndX - 1 || py == absY ||
                            (py == headerEndY - 1 && static_cast<int>(i) != activeIndex)) {
                            buffer[py * bufferWidth + px] = borderColor;
                        } else {
                            buffer[py * bufferWidth + px] = bgColor;
                        }
                    }
                }
            }

            // Draw tab text
            if (fontRenderer) {
                int textX = tabX + 10;
                int textY = absY + headerHeight - 8;
                fontRenderer->drawText(buffer, bufferWidth, bufferHeight,
                                     tabNames[i], textX, textY, textColor);
            }
        }
    }

    // Draw line below headers
    int lineY = absY + headerHeight;
    if (lineY >= 0 && lineY < bufferHeight) {
        for (int px = absX; px < endX; px++) {
            if (px >= 0) {
                buffer[lineY * bufferWidth + px] = borderColor;
            }
        }
    }

    // Draw active panel only
    if (activeIndex >= 0 && activeIndex < static_cast<int>(contentPanels.size())) {
        contentPanels[activeIndex]->draw(buffer, bufferWidth, bufferHeight);
    }
}

void TabbedPanel::handleMouseMove(int mouseX, int mouseY) {
    // Forward to active panel only
    if (activeIndex >= 0 && activeIndex < static_cast<int>(contentPanels.size())) {
        contentPanels[activeIndex]->handleMouseMove(mouseX, mouseY);
    }
}

void TabbedPanel::handleMouseButton(int mouseX, int mouseY, bool isPressed) {
    std::cout << "TabbedPanel::handleMouseButton: (" << mouseX << "," << mouseY << ") pressed=" << isPressed << std::endl;
    if (isPressed) {
        // Check for tab header click
        int clickedTab = getClickedTab(mouseX, mouseY);
        if (clickedTab != -1) {
            std::cout << "  Switching to tab " << clickedTab << std::endl;
            switchToTab(clickedTab);
            return;
        }
        std::cout << "  Forwarding to active panel (index " << activeIndex << ")" << std::endl;
    }

    // Forward to active panel
    if (activeIndex >= 0 && activeIndex < static_cast<int>(contentPanels.size())) {
        contentPanels[activeIndex]->handleMouseButton(mouseX, mouseY, isPressed);
    }
}

void TabbedPanel::handleChar(unsigned int charCode) {
    std::cout << "TabbedPanel::handleChar: " << charCode << " ('" << (char)charCode << "')" << std::endl;
    // Forward to active panel only
    if (activeIndex >= 0 && activeIndex < static_cast<int>(contentPanels.size())) {
        contentPanels[activeIndex]->handleChar(charCode);
    }
}

void TabbedPanel::handleKey(int key, bool isPressed) {
    std::cout << "TabbedPanel::handleKey: key=" << key << " isPressed=" << isPressed << std::endl;
    // Forward to active panel only
    if (activeIndex >= 0 && activeIndex < static_cast<int>(contentPanels.size())) {
        contentPanels[activeIndex]->handleKey(key, isPressed);
    }
}

void TabbedPanel::setFontRenderer(FontRenderer* renderer) {
    Widget::setFontRenderer(renderer);

    for (Panel* panel : contentPanels) {
        panel->setFontRenderer(renderer);
    }
}
