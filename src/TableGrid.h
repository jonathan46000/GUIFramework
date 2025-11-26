#ifndef TABLEGRID_H
#define TABLEGRID_H

#include "Widget.h"
#include "ScrollBar.h"
#include "TextBox.h"
#include <vector>
#include <string>
#include <functional>

class TableGrid : public Widget {
private:
    // Data storage
    std::vector<std::vector<std::string>> cells;
    int rows;
    int cols;

    // Selection and editing
    int selectedRow;
    int selectedCol;
    int hoveredRow;
    int hoveredCol;
    TextBox* activeTextBox;
    bool isEditing;

    // Scroll state
    int scrollOffsetRow;
    int scrollOffsetCol;
    int visibleRows;
    int visibleCols;

    // Dimensions
    int rowHeight;
    int headerHeight;
    int headerWidth;
    std::vector<int> columnWidths;
    int defaultColumnWidth;

    // Scrollbars
    ScrollBar* verticalScrollBar;
    ScrollBar* horizontalScrollBar;
    int scrollBarWidth;

    // Colors
    uint32_t backgroundColor;
    uint32_t cellBackgroundColor;
    uint32_t headerBackgroundColor;
    uint32_t selectedBackgroundColor;
    uint32_t hoverBackgroundColor;
    uint32_t gridLineColor;
    uint32_t textColor;
    uint32_t headerTextColor;
    uint32_t selectedTextColor;
    uint32_t borderColor;

    // Callbacks
    std::function<void(int, int, const std::string&)> cellChangeCallback;

    // Helper methods
    void updateScrollBars();
    void calculateVisibleCells();
    bool getCellAtPosition(int mouseX, int mouseY, int& row, int& col);
    void positionTextBoxForCell(int row, int col);
    void commitCellEdit();
    void startCellEdit(int row, int col);
    void drawCell(uint32_t* buffer, int bufferWidth, int bufferHeight, int row, int col, int cellX, int cellY, int cellWidth, int cellHeight);
    void drawRowHeader(uint32_t* buffer, int bufferWidth, int bufferHeight, int row, int headerY);
    void drawColumnHeader(uint32_t* buffer, int bufferWidth, int bufferHeight, int col, int headerX, int colWidth);
    int getCellX(int col);
    int getCellY(int row);
    int getColumnWidth(int col);
    std::string getColumnLabel(int col);

public:
    TableGrid(int x, int y, int width, int height, int rows, int cols);
    ~TableGrid();

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void checkHover(int mouseX, int mouseY) override;
    void handleMouseButton(int mouseX, int mouseY, bool isPressed) override;
    void handleMouseMove(int mouseX, int mouseY) override;
    void handleChar(unsigned int charCode) override;
    void handleKey(int key, bool isPressed) override;
    void setFontRenderer(FontRenderer* renderer) override;

    void copy() override;
    void cut() override;
    void paste() override;
    void selectAll() override;
    bool hasSelection() const override;

    // Data access
    void setCellValue(int row, int col, const std::string& value);
    std::string getCellValue(int row, int col) const;
    void setRowCount(int newRows);
    void setColCount(int newCols);
    int getRowCount() const { return rows; }
    int getColCount() const { return cols; }

    // Selection
    void setSelectedCell(int row, int col);
    int getSelectedRow() const { return selectedRow; }
    int getSelectedCol() const { return selectedCol; }

    // Layout
    void setColumnWidth(int col, int width);
    void setDefaultColumnWidth(int width);
    void setRowHeight(int height);
    void setHeaderHeight(int height);
    void setHeaderWidth(int width);

    // Callbacks
    void setCellChangeCallback(std::function<void(int, int, const std::string&)> callback);

    // Colors
    void setBackgroundColor(uint32_t color);
    void setCellBackgroundColor(uint32_t color);
    void setHeaderBackgroundColor(uint32_t color);
    void setSelectedBackgroundColor(uint32_t color);
    void setHoverBackgroundColor(uint32_t color);
    void setGridLineColor(uint32_t color);
    void setTextColor(uint32_t color);
    void setHeaderTextColor(uint32_t color);
    void setSelectedTextColor(uint32_t color);
    void setBorderColor(uint32_t color);
};

#endif
