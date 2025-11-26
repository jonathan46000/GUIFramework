#include "TableGrid.h"
#include "MiniFB.h"
#include <algorithm>
#include <sstream>

TableGrid::TableGrid(int x, int y, int width, int height, int rows, int cols)
    : Widget(x, y, width, height), rows(rows), cols(cols),
      selectedRow(-1), selectedCol(-1), hoveredRow(-1), hoveredCol(-1),
      activeTextBox(nullptr), isEditing(false),
      scrollOffsetRow(0), scrollOffsetCol(0), visibleRows(0), visibleCols(0),
      rowHeight(30), headerHeight(30), headerWidth(50), defaultColumnWidth(100),
      scrollBarWidth(20),
      backgroundColor(0xFFF0F0F0), cellBackgroundColor(0xFFFFFFFF),
      headerBackgroundColor(0xFFE0E0E0), selectedBackgroundColor(MFB_RGB(0, 120, 215)),
      hoverBackgroundColor(0xFFD0E8FF), gridLineColor(0xFFC0C0C0),
      textColor(MFB_RGB(0, 0, 0)), headerTextColor(MFB_RGB(0, 0, 0)),
      selectedTextColor(MFB_RGB(255, 255, 255)), borderColor(0xFF808080),
      cellChangeCallback(nullptr) {

    // Initialize cell data
    cells.resize(rows);
    for (int r = 0; r < rows; r++) {
        cells[r].resize(cols, "");
    }

    // Initialize column widths
    columnWidths.resize(cols, defaultColumnWidth);

    // Calculate actual content dimensions first
    calculateVisibleCells();

    // Calculate actual content area dimensions
    int actualContentHeight = visibleRows * rowHeight;
    int actualContentWidth = 0;
    for (int c = 0; c < visibleCols; c++) {
        actualContentWidth += getColumnWidth(c);
    }

    // Resize widget to fit actual content plus headers, borders, and scrollbars
    int neededWidth = 1 + headerWidth + actualContentWidth + scrollBarWidth + 1;  // borders on both sides
    int neededHeight = 1 + headerHeight + actualContentHeight + scrollBarWidth + 1;  // borders on both sides
    setSize(neededWidth, neededHeight);

    // Create scrollbars positioned at the edge of actual content (accounting for borders)
    verticalScrollBar = new ScrollBar(1 + headerWidth + actualContentWidth, 1 + headerHeight, actualContentHeight, ScrollBarOrientation::VERTICAL);
    verticalScrollBar->setParent(this);
    verticalScrollBar->setRange(0, rows);
    verticalScrollBar->setVisibleAmount(1);
    verticalScrollBar->setValue(0);
    verticalScrollBar->setChangeCallback([this](double value) {
        if (isEditing) {
            commitCellEdit();
        }
        scrollOffsetRow = (int)value;
    });

    horizontalScrollBar = new ScrollBar(1 + headerWidth, 1 + headerHeight + actualContentHeight, actualContentWidth, ScrollBarOrientation::HORIZONTAL);
    horizontalScrollBar->setParent(this);
    horizontalScrollBar->setRange(0, cols);
    horizontalScrollBar->setVisibleAmount(1);
    horizontalScrollBar->setValue(0);
    horizontalScrollBar->setChangeCallback([this](double value) {
        if (isEditing) {
            commitCellEdit();
        }
        scrollOffsetCol = (int)value;
    });

    // Create TextBox for editing (initially hidden)
    activeTextBox = new TextBox(0, 0, 100, rowHeight);
    activeTextBox->setParent(this);

    updateScrollBars();
}

TableGrid::~TableGrid() {
    delete verticalScrollBar;
    delete horizontalScrollBar;
    delete activeTextBox;
}

void TableGrid::calculateVisibleCells() {
    int contentHeight = height - headerHeight - 2;  // -2 for borders
    int contentWidth = width - headerWidth - 2;  // -2 for borders

    visibleRows = contentHeight / rowHeight;

    visibleCols = 0;
    int currentWidth = 0;
    for (int c = scrollOffsetCol; c < cols; c++) {
        int colWidth = getColumnWidth(c);
        if (currentWidth + colWidth > contentWidth) break;
        currentWidth += colWidth;
        visibleCols++;
    }
}

void TableGrid::updateScrollBars() {
    calculateVisibleCells();

    if (rows > visibleRows) {
        verticalScrollBar->setRange(0, rows);
        verticalScrollBar->setVisibleAmount(visibleRows);
    } else {
        scrollOffsetRow = 0;
        verticalScrollBar->setValue(0);
    }

    if (cols > visibleCols) {
        horizontalScrollBar->setRange(0, cols);
        horizontalScrollBar->setVisibleAmount(visibleCols);
    } else {
        scrollOffsetCol = 0;
        horizontalScrollBar->setValue(0);
    }
}

int TableGrid::getCellX(int col) {
    int absX = getAbsoluteX();
    int x = absX + 1 + headerWidth;  // +1 for left border
    for (int c = scrollOffsetCol; c < col && c < cols; c++) {
        x += getColumnWidth(c);
    }
    return x;
}

int TableGrid::getCellY(int row) {
    int absY = getAbsoluteY();
    return absY + 1 + headerHeight + (row - scrollOffsetRow) * rowHeight;  // +1 for top border
}

int TableGrid::getColumnWidth(int col) {
    if (col >= 0 && col < (int)columnWidths.size()) {
        return columnWidths[col];
    }
    return defaultColumnWidth;
}

std::string TableGrid::getColumnLabel(int col) {
    std::string label;
    do {
        label = char('A' + (col % 26)) + label;
        col = col / 26 - 1;
    } while (col >= 0);
    return label;
}

void TableGrid::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int endX = std::min(absX + width, bufferWidth);
    int endY = std::min(absY + height, bufferHeight);

    // Draw background
    for (int py = absY; py < endY; py++) {
        for (int px = absX; px < endX; px++) {
            if (px >= 0 && py >= 0) {
                buffer[py * bufferWidth + px] = backgroundColor;
            }
        }
    }

    // Draw border
    for (int py = absY; py < endY; py++) {
        for (int px = absX; px < endX; px++) {
            if (px >= 0 && py >= 0) {
                if (py == absY || py == endY - 1 || px == absX || px == endX - 1) {
                    buffer[py * bufferWidth + px] = borderColor;
                }
            }
        }
    }

    // Draw row headers
    for (int r = scrollOffsetRow; r < rows && r < scrollOffsetRow + visibleRows; r++) {
        int headerY = getCellY(r);
        drawRowHeader(buffer, bufferWidth, bufferHeight, r, headerY);
    }

    // Draw column headers
    int colX = absX + 1 + headerWidth;  // +1 for left border
    for (int c = scrollOffsetCol; c < cols && c < scrollOffsetCol + visibleCols; c++) {
        int colWidth = getColumnWidth(c);
        drawColumnHeader(buffer, bufferWidth, bufferHeight, c, colX, colWidth);
        colX += colWidth;
    }

    // Draw cells
    for (int r = scrollOffsetRow; r < rows && r < scrollOffsetRow + visibleRows; r++) {
        int cellY = getCellY(r);
        int cellX = absX + 1 + headerWidth;  // +1 for left border

        for (int c = scrollOffsetCol; c < cols && c < scrollOffsetCol + visibleCols; c++) {
            int colWidth = getColumnWidth(c);
            drawCell(buffer, bufferWidth, bufferHeight, r, c, cellX, cellY, colWidth, rowHeight);
            cellX += colWidth;
        }
    }

    // Draw active TextBox if editing
    if (isEditing && activeTextBox) {
        activeTextBox->draw(buffer, bufferWidth, bufferHeight);
    }

    // Draw scrollbars
    if (rows > visibleRows) {
        verticalScrollBar->draw(buffer, bufferWidth, bufferHeight);
    }
    if (cols > visibleCols) {
        horizontalScrollBar->draw(buffer, bufferWidth, bufferHeight);
    }
}

void TableGrid::drawCell(uint32_t* buffer, int bufferWidth, int bufferHeight, int row, int col, int cellX, int cellY, int cellWidth, int cellHeight) {
    int endX = std::min(cellX + cellWidth, bufferWidth);
    int endY = std::min(cellY + cellHeight, bufferHeight);

    uint32_t bgColor = cellBackgroundColor;
    if (row == selectedRow && col == selectedCol && !isEditing) {
        bgColor = selectedBackgroundColor;
    } else if (row == hoveredRow && col == hoveredCol) {
        bgColor = hoverBackgroundColor;
    }

    // Draw cell background
    for (int py = cellY; py < endY; py++) {
        for (int px = cellX; px < endX; px++) {
            if (px >= 0 && py >= 0) {
                buffer[py * bufferWidth + px] = bgColor;
            }
        }
    }

    // Draw grid lines
    for (int px = cellX; px < endX; px++) {
        if (px >= 0 && cellY >= 0 && cellY < bufferHeight) {
            buffer[cellY * bufferWidth + px] = gridLineColor;
        }
        if (px >= 0 && endY - 1 >= 0 && endY - 1 < bufferHeight) {
            buffer[(endY - 1) * bufferWidth + px] = gridLineColor;
        }
    }
    for (int py = cellY; py < endY; py++) {
        if (py >= 0 && cellX >= 0 && cellX < bufferWidth) {
            buffer[py * bufferWidth + cellX] = gridLineColor;
        }
        if (py >= 0 && endX - 1 >= 0 && endX - 1 < bufferWidth) {
            buffer[py * bufferWidth + (endX - 1)] = gridLineColor;
        }
    }

    // Draw cell text (skip if editing this cell)
    if (fontRenderer && !(isEditing && row == selectedRow && col == selectedCol)) {
        const std::string& cellText = cells[row][col];
        if (!cellText.empty()) {
            uint32_t txtColor = (row == selectedRow && col == selectedCol) ? selectedTextColor : textColor;
            int textX = cellX + 5;
            int textY = cellY + (cellHeight + fontRenderer->getTextHeight()) / 2;
            fontRenderer->drawText(buffer, bufferWidth, bufferHeight, cellText, textX, textY, txtColor);
        }
    }
}

void TableGrid::drawRowHeader(uint32_t* buffer, int bufferWidth, int bufferHeight, int row, int headerY) {
    int absX = getAbsoluteX();
    int endX = std::min(absX + 1 + headerWidth, bufferWidth);  // +1 for left border
    int endY = std::min(headerY + rowHeight, bufferHeight);

    // Draw header background
    for (int py = headerY; py < endY; py++) {
        for (int px = absX + 1; px < endX; px++) {  // +1 to start after left border
            if (px >= 0 && py >= 0) {
                buffer[py * bufferWidth + px] = headerBackgroundColor;
            }
        }
    }

    // Draw grid lines
    for (int px = absX + 1; px < endX; px++) {  // +1 to start after left border
        if (px >= 0 && headerY >= 0 && headerY < bufferHeight) {
            buffer[headerY * bufferWidth + px] = gridLineColor;
        }
    }
    for (int py = headerY; py < endY; py++) {
        if (py >= 0 && endX - 1 >= 0 && endX - 1 < bufferWidth) {
            buffer[py * bufferWidth + (endX - 1)] = gridLineColor;
        }
    }

    // Draw row number
    if (fontRenderer) {
        std::string rowLabel = std::to_string(row + 1);
        int textX = absX + 1 + headerWidth / 2 - (fontRenderer->getTextWidth(rowLabel) / 2);  // +1 for border
        int textY = headerY + (rowHeight + fontRenderer->getTextHeight()) / 2;
        fontRenderer->drawText(buffer, bufferWidth, bufferHeight, rowLabel, textX, textY, headerTextColor);
    }
}

void TableGrid::drawColumnHeader(uint32_t* buffer, int bufferWidth, int bufferHeight, int col, int headerX, int colWidth) {
    int absY = getAbsoluteY();
    int endX = std::min(headerX + colWidth, bufferWidth);
    int endY = std::min(absY + 1 + headerHeight, bufferHeight);  // +1 for top border

    // Draw header background
    for (int py = absY + 1; py < endY; py++) {  // +1 to start after top border
        for (int px = headerX; px < endX; px++) {
            if (px >= 0 && py >= 0) {
                buffer[py * bufferWidth + px] = headerBackgroundColor;
            }
        }
    }

    // Draw grid lines
    for (int px = headerX; px < endX; px++) {
        if (px >= 0 && endY - 1 >= 0 && endY - 1 < bufferHeight) {
            buffer[(endY - 1) * bufferWidth + px] = gridLineColor;
        }
    }
    for (int py = absY + 1; py < endY; py++) {  // +1 to start after top border
        if (py >= 0 && endX - 1 >= 0 && endX - 1 < bufferWidth) {
            buffer[py * bufferWidth + (endX - 1)] = gridLineColor;
        }
    }

    // Draw column label
    if (fontRenderer) {
        std::string colLabel = getColumnLabel(col);
        int textX = headerX + colWidth / 2 - (fontRenderer->getTextWidth(colLabel) / 2);
        int textY = absY + 1 + (headerHeight + fontRenderer->getTextHeight()) / 2;  // +1 for border
        fontRenderer->drawText(buffer, bufferWidth, bufferHeight, colLabel, textX, textY, headerTextColor);
    }
}

void TableGrid::checkHover(int mouseX, int mouseY) {
    hoveredRow = -1;
    hoveredCol = -1;

    getCellAtPosition(mouseX, mouseY, hoveredRow, hoveredCol);

    if (rows > visibleRows) {
        verticalScrollBar->checkHover(mouseX, mouseY);
    }
    if (cols > visibleCols) {
        horizontalScrollBar->checkHover(mouseX, mouseY);
    }
}

bool TableGrid::getCellAtPosition(int mouseX, int mouseY, int& row, int& col) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    // Check row
    if (mouseY < absY + headerHeight || mouseY >= absY + height - scrollBarWidth) {
        return false;
    }
    int relativeY = mouseY - (absY + headerHeight);
    row = scrollOffsetRow + (relativeY / rowHeight);
    if (row < 0 || row >= rows) {
        return false;
    }

    // Check column
    if (mouseX < absX + headerWidth || mouseX >= absX + width - scrollBarWidth) {
        return false;
    }
    int relativeX = mouseX - (absX + headerWidth);
    int currentX = 0;
    col = -1;
    for (int c = scrollOffsetCol; c < cols; c++) {
        int colWidth = getColumnWidth(c);
        if (relativeX >= currentX && relativeX < currentX + colWidth) {
            col = c;
            break;
        }
        currentX += colWidth;
    }

    return (col >= 0 && col < cols);
}

void TableGrid::handleMouseButton(int mouseX, int mouseY, bool isPressed) {
    // Check scrollbars first - on release, check if dragging regardless of mouse position
    if (!isPressed) {
        if (verticalScrollBar && verticalScrollBar->isDragging()) {
            verticalScrollBar->handleMouseButton(mouseX, mouseY, isPressed);
            return;
        }
        if (horizontalScrollBar && horizontalScrollBar->isDragging()) {
            horizontalScrollBar->handleMouseButton(mouseX, mouseY, isPressed);
            return;
        }
    }

    // On press, check if clicking on scrollbar area
    if (isPressed) {
        if (rows > visibleRows && verticalScrollBar && verticalScrollBar->containsPoint(mouseX, mouseY)) {
            verticalScrollBar->handleMouseButton(mouseX, mouseY, isPressed);
            return;
        }
        if (cols > visibleCols && horizontalScrollBar && horizontalScrollBar->containsPoint(mouseX, mouseY)) {
            horizontalScrollBar->handleMouseButton(mouseX, mouseY, isPressed);
            return;
        }
    }

    if (isPressed) {
        int clickedRow, clickedCol;
        if (getCellAtPosition(mouseX, mouseY, clickedRow, clickedCol)) {
            if (isEditing) {
                commitCellEdit();
            }
            selectedRow = clickedRow;
            selectedCol = clickedCol;
        }
    } else {
        // Double-click to edit (simplified: just edit on second click)
        if (selectedRow >= 0 && selectedCol >= 0) {
            int clickedRow, clickedCol;
            if (getCellAtPosition(mouseX, mouseY, clickedRow, clickedCol)) {
                if (clickedRow == selectedRow && clickedCol == selectedCol) {
                    startCellEdit(selectedRow, selectedCol);
                }
            }
        }
    }

    // Forward to active TextBox if editing
    if (isEditing && activeTextBox) {
        activeTextBox->handleMouseButton(mouseX, mouseY, isPressed);
    }
}

void TableGrid::handleMouseMove(int mouseX, int mouseY) {
    if (verticalScrollBar && verticalScrollBar->isDragging()) {
        verticalScrollBar->handleMouseMove(mouseX, mouseY);
    }
    if (horizontalScrollBar && horizontalScrollBar->isDragging()) {
        horizontalScrollBar->handleMouseMove(mouseX, mouseY);
    }
    if (isEditing && activeTextBox) {
        activeTextBox->handleMouseMove(mouseX, mouseY);
    }
}

void TableGrid::handleChar(unsigned int charCode) {
    if (isEditing && activeTextBox) {
        activeTextBox->handleChar(charCode);
    } else if (selectedRow >= 0 && selectedCol >= 0 && charCode >= 32 && charCode <= 126) {
        // Start editing on printable character
        startCellEdit(selectedRow, selectedCol);
        cells[selectedRow][selectedCol] = std::string(1, (char)charCode);
        activeTextBox->setText(cells[selectedRow][selectedCol]);
    }
}

void TableGrid::handleKey(int key, bool isPressed) {
    if (!isPressed) return;

    if (isEditing && activeTextBox) {
        if (key == KB_KEY_ENTER || key == KB_KEY_TAB) {
            commitCellEdit();

            // Move selection
            if (key == KB_KEY_ENTER && selectedRow < rows - 1) {
                selectedRow++;
            } else if (key == KB_KEY_TAB && selectedCol < cols - 1) {
                selectedCol++;
            }
        } else if (key == KB_KEY_ESCAPE) {
            isEditing = false;
        } else {
            activeTextBox->handleKey(key, isPressed);
        }
    } else {
        // Navigation keys
        if (key == KB_KEY_UP && selectedRow > 0) {
            selectedRow--;
            if (selectedRow < scrollOffsetRow) {
                scrollOffsetRow = selectedRow;
                verticalScrollBar->setValue(scrollOffsetRow);
            }
        } else if (key == KB_KEY_DOWN && selectedRow < rows - 1) {
            selectedRow++;
            if (selectedRow >= scrollOffsetRow + visibleRows) {
                scrollOffsetRow = selectedRow - visibleRows + 1;
                verticalScrollBar->setValue(scrollOffsetRow);
            }
        } else if (key == KB_KEY_LEFT && selectedCol > 0) {
            selectedCol--;
            if (selectedCol < scrollOffsetCol) {
                scrollOffsetCol = selectedCol;
                horizontalScrollBar->setValue(scrollOffsetCol);
            }
        } else if (key == KB_KEY_RIGHT && selectedCol < cols - 1) {
            selectedCol++;
            calculateVisibleCells();
            if (selectedCol >= scrollOffsetCol + visibleCols) {
                scrollOffsetCol++;
                horizontalScrollBar->setValue(scrollOffsetCol);
            }
        } else if (key == KB_KEY_ENTER) {
            if (selectedRow >= 0 && selectedCol >= 0) {
                startCellEdit(selectedRow, selectedCol);
            }
        }
    }
}

void TableGrid::startCellEdit(int row, int col) {
    if (row < 0 || row >= rows || col < 0 || col >= cols) return;

    isEditing = true;
    selectedRow = row;
    selectedCol = col;

    positionTextBoxForCell(row, col);
    activeTextBox->setText(cells[row][col]);
    activeTextBox->setFocus(true);
}

void TableGrid::positionTextBoxForCell(int row, int col) {
    int cellX = getCellX(col) - getAbsoluteX();
    int cellY = getCellY(row) - getAbsoluteY();
    int colWidth = getColumnWidth(col);

    activeTextBox->setPosition(cellX + 1, cellY + 1);
    activeTextBox->setSize(colWidth - 2, rowHeight - 2);
}

void TableGrid::commitCellEdit() {
    if (!isEditing || selectedRow < 0 || selectedCol < 0) return;

    cells[selectedRow][selectedCol] = activeTextBox->getText();

    if (cellChangeCallback) {
        cellChangeCallback(selectedRow, selectedCol, cells[selectedRow][selectedCol]);
    }

    isEditing = false;
    activeTextBox->setFocus(false);
}

void TableGrid::setFontRenderer(FontRenderer* renderer) {
    Widget::setFontRenderer(renderer);
    if (verticalScrollBar) {
        verticalScrollBar->setFontRenderer(renderer);
    }
    if (horizontalScrollBar) {
        horizontalScrollBar->setFontRenderer(renderer);
    }
    if (activeTextBox) {
        activeTextBox->setFontRenderer(renderer);
    }
}

void TableGrid::copy() {
    if (isEditing && activeTextBox) {
        activeTextBox->copy();
    }
}

void TableGrid::cut() {
    if (isEditing && activeTextBox) {
        activeTextBox->cut();
    }
}

void TableGrid::paste() {
    if (isEditing && activeTextBox) {
        activeTextBox->paste();
    }
}

void TableGrid::selectAll() {
    if (isEditing && activeTextBox) {
        activeTextBox->selectAll();
    }
}

bool TableGrid::hasSelection() const {
    if (isEditing && activeTextBox) {
        return activeTextBox->hasSelection();
    }
    return false;
}

void TableGrid::setCellValue(int row, int col, const std::string& value) {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        cells[row][col] = value;
    }
}

std::string TableGrid::getCellValue(int row, int col) const {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        return cells[row][col];
    }
    return "";
}

void TableGrid::setRowCount(int newRows) {
    if (newRows < 1) return;

    cells.resize(newRows);
    for (int r = rows; r < newRows; r++) {
        cells[r].resize(cols, "");
    }
    rows = newRows;
    updateScrollBars();
}

void TableGrid::setColCount(int newCols) {
    if (newCols < 1) return;

    for (int r = 0; r < rows; r++) {
        cells[r].resize(newCols, "");
    }
    columnWidths.resize(newCols, defaultColumnWidth);
    cols = newCols;
    updateScrollBars();
}

void TableGrid::setSelectedCell(int row, int col) {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        selectedRow = row;
        selectedCol = col;
    }
}

void TableGrid::setColumnWidth(int col, int width) {
    if (col >= 0 && col < (int)columnWidths.size() && width > 0) {
        columnWidths[col] = width;
        updateScrollBars();
    }
}

void TableGrid::setDefaultColumnWidth(int width) {
    if (width > 0) {
        defaultColumnWidth = width;
    }
}

void TableGrid::setRowHeight(int height) {
    if (height > 0) {
        rowHeight = height;
        updateScrollBars();
    }
}

void TableGrid::setHeaderHeight(int height) {
    if (height > 0) {
        headerHeight = height;
        updateScrollBars();
    }
}

void TableGrid::setHeaderWidth(int width) {
    if (width > 0) {
        headerWidth = width;
        updateScrollBars();
    }
}

void TableGrid::setCellChangeCallback(std::function<void(int, int, const std::string&)> callback) {
    cellChangeCallback = callback;
}

void TableGrid::setBackgroundColor(uint32_t color) {
    backgroundColor = color;
}

void TableGrid::setCellBackgroundColor(uint32_t color) {
    cellBackgroundColor = color;
}

void TableGrid::setHeaderBackgroundColor(uint32_t color) {
    headerBackgroundColor = color;
}

void TableGrid::setSelectedBackgroundColor(uint32_t color) {
    selectedBackgroundColor = color;
}

void TableGrid::setHoverBackgroundColor(uint32_t color) {
    hoverBackgroundColor = color;
}

void TableGrid::setGridLineColor(uint32_t color) {
    gridLineColor = color;
}

void TableGrid::setTextColor(uint32_t color) {
    textColor = color;
}

void TableGrid::setHeaderTextColor(uint32_t color) {
    headerTextColor = color;
}

void TableGrid::setSelectedTextColor(uint32_t color) {
    selectedTextColor = color;
}

void TableGrid::setBorderColor(uint32_t color) {
    borderColor = color;
}
