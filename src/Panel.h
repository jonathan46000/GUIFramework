#ifndef PANEL_H
#define PANEL_H

#include "Widget.h"
#include <vector>

class ContextMenu;

class Panel : public Widget {
private:
    std::vector<Widget*> children;
    uint32_t backgroundColor;
    uint32_t borderColor;
    bool drawBorder;
    ContextMenu* contextMenu;

public:
    Panel(int x, int y, int width, int height);
    ~Panel();

    void add(Widget* widget);
    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void handleMouseMove(int mouseX, int mouseY) override;
    void handleMouseButton(int mouseX, int mouseY, bool isPressed) override;
    void handleChar(unsigned int charCode) override;
    void handleKey(int key, bool isPressed) override;
    void setFontRenderer(FontRenderer* renderer) override;

    void handleRightClick(int mouseX, int mouseY);
    void setContextMenu(ContextMenu* menu);

    void setBackgroundColor(uint32_t color);
    void setBorderColor(uint32_t color);
    void setDrawBorder(bool draw);

    const std::vector<Widget*>& getChildren() const { return children; }
    ContextMenu* getContextMenu() const { return contextMenu; }
};

#endif
