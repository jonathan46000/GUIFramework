#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "Widget.h"
#include <string>
#include <functional>

class TextBox : public Widget {
private:
    std::string text;
    int cursorPosition;
    int textOffset;
    bool isFocused;
    bool isHovered;
    bool isSelecting;
    int selectionStart;
    int selectionEnd;
    uint32_t backgroundColor;
    uint32_t focusedBackgroundColor;
    uint32_t borderColor;
    uint32_t focusedBorderColor;
    uint32_t textColor;
    uint32_t cursorColor;
    uint32_t selectionColor;
    std::function<void(const std::string&)> changeCallback;
    int cursorBlinkCounter;
    bool showCursor;

    void adjustTextOffset();
    int getCursorPixelPosition();
    int getCharacterIndexAtPosition(int pixelX);
    void deleteSelection();
    std::string getSelectedText() const;
    int getSelectionStart() const;
    int getSelectionEnd() const;

public:
    TextBox(int x, int y, int width, int height);

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void checkHover(int mouseX, int mouseY) override;
    void handleChar(unsigned int charCode) override;
    void handleKey(int key, bool isPressed) override;
    void handleMouseButton(int mouseX, int mouseY, bool isPressed) override;
    void handleMouseMove(int mouseX, int mouseY) override;

    void setFocus(bool focused);
    void setChangeCallback(std::function<void(const std::string&)> callback);

    bool isFocusedWidget() const { return isFocused; }
    const std::string& getText() const { return text; }
    void setText(const std::string& newText);

    void copy() override;
    void cut() override;
    void paste() override;
    void selectAll() override;
    bool hasSelection() const override;
    void stopSelecting();
};

#endif
