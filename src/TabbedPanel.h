#ifndef TABBEDPANEL_H
#define TABBEDPANEL_H

#include "Widget.h"
#include "Panel.h"
#include <vector>
#include <string>

class TabbedPanel : public Widget {
private:
    std::vector<Panel*> contentPanels;
    std::vector<std::string> tabNames;
    int activeIndex;
    int headerHeight;
    uint32_t activeBgColor;
    uint32_t inactiveBgColor;
    uint32_t borderColor;
    uint32_t textColor;
    uint32_t contentBgColor;

    int getTabWidth() const;
    int getClickedTab(int mouseX, int mouseY) const;

public:
    TabbedPanel(int x, int y, int width, int height,
                int headerHeight = 30,
                uint32_t activeBg = 0xFFFFFFFF,
                uint32_t inactiveBg = 0xFFD0D0D0,
                uint32_t border = 0xFF808080,
                uint32_t text = 0xFF000000,
                uint32_t contentBg = 0xFFF0F0F0);
    ~TabbedPanel();

    void addTab(const std::string& name, Panel* panel);
    void switchToTab(int index);
    int getActiveTabIndex() const { return activeIndex; }
    Panel* getActivePanel() const;

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void handleMouseMove(int mouseX, int mouseY) override;
    void handleMouseButton(int mouseX, int mouseY, bool isPressed) override;
    void handleChar(unsigned int charCode) override;
    void handleKey(int key, bool isPressed) override;
    void setFontRenderer(FontRenderer* renderer) override;
};

#endif
