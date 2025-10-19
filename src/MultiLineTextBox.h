#ifndef MULTILINETEXTBOX_H
#define MULTILINETEXTBOX_H

#include "Widget.h"
#include <string>
#include <vector>
#include <functional>

class MultiLineTextBox : public Widget {
private:
    std::string text;
    std::vector<std::string> lines;
    std::vector<bool> isWrappedLine;
    int cursorLine;
    int cursorColumn;
    int scrollOffset;
    bool isFocused;
    bool isHovered;
    bool isSelecting;
    int selectionStartLine;
    int selectionStartColumn;
    int selectionEndLine;
    int selectionEndColumn;
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
    int visibleLines;
    int lineHeight;

    void wrapText();
    void checkAndWrapCurrentLine();
    void insertCharAtCursor(char c);
    void deleteCharAtCursor();
    void moveCursorLeft(bool shift);
    void moveCursorRight(bool shift);
    void moveCursorUp(bool shift);
    void moveCursorDown(bool shift);
    void moveCursorToLineStart(bool shift);
    void moveCursorToLineEnd(bool shift);
    void updateScrollOffset();
    void rebuildTextFromLines();
    int getCharIndexFromPosition(int lineIndex, int pixelX);
    void clearSelection();
    void updateSelection();
    void deleteSelection();
    std::string getSelectedText() const;
    void getSelectionBounds(int& startLine, int& startCol, int& endLine, int& endCol) const;

public:
    MultiLineTextBox(int x, int y, int width, int height);

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

    void setBackgroundColor(uint32_t color) { backgroundColor = color; }
    void setFocusedBackgroundColor(uint32_t color) { focusedBackgroundColor = color; }
    void setBorderColor(uint32_t color) { borderColor = color; }
    void setFocusedBorderColor(uint32_t color) { focusedBorderColor = color; }
    void setTextColor(uint32_t color) { textColor = color; }
    void setCursorColor(uint32_t color) { cursorColor = color; }
    void setSelectionColor(uint32_t color) { selectionColor = color; }
};

#endif
