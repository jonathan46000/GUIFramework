#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include "Widget.h"
#include "MenuItem.h"
#include <vector>
#include <string>

class ContextMenu : public Widget {
protected:
    bool isOpen;
    uint32_t backgroundColor;
    uint32_t borderColor;
    std::vector<MenuItem*> items;
    int menuWidth;
    int itemHeight;

public:
    ContextMenu(int menuWidth = 150, int itemHeight = 25);
    virtual ~ContextMenu();

    virtual void add(MenuItem* item);
    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void handleMouseMove(int mouseX, int mouseY) override;
    void setFontRenderer(FontRenderer* renderer) override;

    void open(int x, int y);
    virtual void close();
    bool checkMenuClick(int mouseX, int mouseY);
    bool checkMenuArea(int mouseX, int mouseY);

    bool getIsOpen() const { return isOpen; }
    void setBackgroundColor(uint32_t color) { backgroundColor = color; }
    void setBorderColor(uint32_t color) { borderColor = color; }
    void setMenuWidth(int width) { menuWidth = width; }
};

#endif
