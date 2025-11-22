#ifndef SPLITTER_H
#define SPLITTER_H

#include "Widget.h"
#include "Panel.h"

enum class SplitterOrientation {
    HORIZONTAL,
    VERTICAL
};

class Splitter : public Widget {
private:
    Panel* firstPanel;
    Panel* secondPanel;
    SplitterOrientation orientation;
    int dividerPosition;
    int dividerWidth;
    bool isDragging;
    int minPanelSize;
    uint32_t dividerColor;
    uint32_t dividerHoverColor;
    bool isHoveringDivider;

    void updatePanelSizes();
    bool isMouseOnDivider(int mouseX, int mouseY) const;

public:
    Splitter(int x, int y, int width, int height, SplitterOrientation orientation = SplitterOrientation::HORIZONTAL);
    ~Splitter();

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void handleMouseMove(int mouseX, int mouseY) override;
    void handleMouseButton(int mouseX, int mouseY, bool isPressed) override;
    void handleChar(unsigned int charCode) override;
    void handleKey(int key, bool isPressed) override;
    void setFontRenderer(FontRenderer* renderer) override;

    void setDividerPosition(int position);
    void setMinPanelSize(int size);
    void setDividerColor(uint32_t color);
    void setDividerHoverColor(uint32_t color);
    void setDividerWidth(int width);

    Panel* getLeftPanel() const { return orientation == SplitterOrientation::HORIZONTAL ? firstPanel : nullptr; }
    Panel* getRightPanel() const { return orientation == SplitterOrientation::HORIZONTAL ? secondPanel : nullptr; }
    Panel* getTopPanel() const { return orientation == SplitterOrientation::VERTICAL ? firstPanel : nullptr; }
    Panel* getBottomPanel() const { return orientation == SplitterOrientation::VERTICAL ? secondPanel : nullptr; }
    Panel* getFirstPanel() const { return firstPanel; }
    Panel* getSecondPanel() const { return secondPanel; }

    int getDividerPosition() const { return dividerPosition; }
    SplitterOrientation getOrientation() const { return orientation; }
};

#endif
