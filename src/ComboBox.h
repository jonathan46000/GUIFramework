#ifndef COMBOBOX_H
#define COMBOBOX_H

#include "Widget.h"
#include <vector>
#include <string>
#include <functional>

class ComboBox : public Widget {
private:
    std::string text;
    std::vector<std::string> items;
    bool isOpen;
    bool isFocused;
    bool isHovered;
    int cursorPosition;
    int textOffset;
    bool isSelecting;
    int selectionStart;
    int selectionEnd;
    int hoveredItemIndex;
    int selectedItemIndex;
    uint32_t backgroundColor;
    uint32_t focusedBackgroundColor;
    uint32_t borderColor;
    uint32_t focusedBorderColor;
    uint32_t textColor;
    uint32_t cursorColor;
    uint32_t selectionColor;
    uint32_t menuBackgroundColor;
    uint32_t menuBorderColor;
    uint32_t menuHoverColor;
    int buttonWidth;
    int maxVisibleItems;
    int scrollOffset;
    std::function<void(const std::string&)> changeCallback;
    std::function<void(int, const std::string&)> selectionCallback;
    int cursorBlinkCounter;
    bool showCursor;

    void adjustTextOffset();
    int getCursorPixelPosition();
    int getCharacterIndexAtPosition(int pixelX);
    void deleteSelection();
    void drawDropdownButton(uint32_t* buffer, int bufferWidth, int bufferHeight, int buttonX, int buttonY);
    int getItemAtPosition(int mouseX, int mouseY);
    int getMenuHeight() const;

public:
    ComboBox(int x, int y, int width, int height);

    void addItem(const std::string& item);
    void clearItems();
    void setSelectedIndex(int index);
    int getSelectedIndex() const { return selectedItemIndex; }
    const std::string& getText() const { return text; }
    void setText(const std::string& newText);

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void drawDropdownMenu(uint32_t* buffer, int bufferWidth, int bufferHeight);
    void checkHover(int mouseX, int mouseY) override;
    void handleChar(unsigned int charCode) override;
    void handleKey(int key, bool isPressed) override;
    void handleMouseButton(int mouseX, int mouseY, bool isPressed) override;
    void handleMouseMove(int mouseX, int mouseY) override;

    void setFocus(bool focused);
    void setChangeCallback(std::function<void(const std::string&)> callback);
    void setSelectionCallback(std::function<void(int, const std::string&)> callback);

    void toggle();
    void open();
    void close();
    bool getIsOpen() const { return isOpen; }
    bool isFocusedWidget() const { return isFocused; }

    void copy() override;
    void cut() override;
    void paste() override;
    void selectAll() override;
    bool hasSelection() const override;
    void stopSelecting();
};

#endif
