#ifndef LISTBOX_H
#define LISTBOX_H

#include "Widget.h"
#include "ScrollBar.h"
#include <vector>
#include <string>
#include <functional>

class ListBox : public Widget {
private:
    std::vector<std::string> items;
    int selectedIndex;
    int scrollOffset;
    int itemHeight;
    int visibleItemCount;
    ScrollBar* scrollBar;
    int scrollBarWidth;
    uint32_t backgroundColor;
    uint32_t itemBackgroundColor;
    uint32_t selectedBackgroundColor;
    uint32_t hoverBackgroundColor;
    uint32_t textColor;
    uint32_t selectedTextColor;
    uint32_t borderColor;
    int hoveredIndex;
    std::function<void(int, const std::string&)> selectionCallback;

    void updateScrollBar();
    int getItemIndexAtPosition(int mouseX, int mouseY);

public:
    ListBox(int x, int y, int width, int height);
    ~ListBox();

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void checkHover(int mouseX, int mouseY) override;
    void handleMouseButton(int mouseX, int mouseY, bool isPressed) override;
    void handleMouseMove(int mouseX, int mouseY) override;
    void setFontRenderer(FontRenderer* renderer) override;

    void addItem(const std::string& item);
    void removeItem(int index);
    void clearItems();
    void setSelectedIndex(int index);
    int getSelectedIndex() const { return selectedIndex; }
    std::string getSelectedItem() const;
    const std::vector<std::string>& getItems() const { return items; }

    void setSelectionCallback(std::function<void(int, const std::string&)> callback);
    void setItemHeight(int height);
    void setBackgroundColor(uint32_t color);
    void setItemBackgroundColor(uint32_t color);
    void setSelectedBackgroundColor(uint32_t color);
    void setHoverBackgroundColor(uint32_t color);
    void setTextColor(uint32_t color);
    void setSelectedTextColor(uint32_t color);
    void setBorderColor(uint32_t color);
};

#endif
