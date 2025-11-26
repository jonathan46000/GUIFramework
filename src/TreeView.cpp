#include "TreeView.h"
#include "MiniFB.h"
#include <algorithm>
#include <iostream>

TreeView::TreeView(int x, int y, int width, int height)
    : Widget(x, y, width, height), root(new TreeNode("Root")), selectedNode(nullptr),
      hoveredNode(nullptr), scrollOffset(0), itemHeight(25), visibleItemCount(0),
      indentWidth(20), expandIconSize(9), scrollBarWidth(20),
      backgroundColor(0xFFF0F0F0), itemBackgroundColor(0xFFFFFFFF),
      selectedBackgroundColor(MFB_RGB(0, 120, 215)), hoverBackgroundColor(0xFFE0E0E0),
      textColor(MFB_RGB(0, 0, 0)), selectedTextColor(MFB_RGB(255, 255, 255)),
      borderColor(0xFF808080), lineColor(0xFFC0C0C0), expandIconColor(0xFF606060),
      selectionCallback(nullptr) {

    visibleItemCount = (height - 2) / itemHeight;

    scrollBar = new ScrollBar(width - scrollBarWidth, 0, height, ScrollBarOrientation::VERTICAL);
    scrollBar->setParent(this);
    scrollBar->setRange(0, 0);
    scrollBar->setVisibleAmount(visibleItemCount);
    scrollBar->setValue(0);
    scrollBar->setChangeCallback([this](double value) {
        scrollOffset = (int)value;
    });

    buildVisibleNodesList();
}

TreeView::~TreeView() {
    delete scrollBar;
    delete root;
}

void TreeView::buildVisibleNodesList() {
    visibleNodes.clear();
    int visualIndex = 0;

    // Build list starting from root's children (root itself is not displayed)
    for (TreeNode* child : root->children) {
        buildVisibleNodesRecursive(child, visualIndex);
    }

    updateScrollBar();
}

void TreeView::buildVisibleNodesRecursive(TreeNode* node, int& visualIndex) {
    visibleNodes.push_back({node, visualIndex});
    visualIndex++;

    if (node->isExpanded) {
        for (TreeNode* child : node->children) {
            buildVisibleNodesRecursive(child, visualIndex);
        }
    }
}

void TreeView::updateScrollBar() {
    if ((int)visibleNodes.size() > visibleItemCount) {
        scrollBar->setRange(0, visibleNodes.size());
        scrollBar->setVisibleAmount(visibleItemCount);
    } else {
        scrollOffset = 0;
        scrollBar->setValue(0);
    }
}

void TreeView::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int endX = std::min(absX + width, bufferWidth);
    int endY = std::min(absY + height, bufferHeight);

    // Draw border and background
    for (int py = absY; py < endY; py++) {
        for (int px = absX; px < endX; px++) {
            if (px >= 0 && py >= 0) {
                if (py == absY || py == endY - 1 || px == absX || px == endX - 1) {
                    buffer[py * bufferWidth + px] = borderColor;
                } else {
                    buffer[py * bufferWidth + px] = backgroundColor;
                }
            }
        }
    }

    int contentWidth = width - scrollBarWidth - 2;
    int drawY = absY + 1;

    // Draw visible nodes
    for (int i = scrollOffset; i < (int)visibleNodes.size() && i < scrollOffset + visibleItemCount; i++) {
        TreeNode* node = visibleNodes[i].node;
        int itemEndX = std::min(absX + 1 + contentWidth, bufferWidth);
        int itemEndY = std::min(drawY + itemHeight, absY + height - 1);

        uint32_t bgColor = itemBackgroundColor;

        if (node == selectedNode) {
            bgColor = selectedBackgroundColor;
        } else if (node == hoveredNode) {
            bgColor = hoverBackgroundColor;
        }

        // Draw item background
        for (int py = drawY; py < itemEndY; py++) {
            for (int px = absX + 1; px < itemEndX; px++) {
                if (px >= 0 && py >= 0) {
                    buffer[py * bufferWidth + px] = bgColor;
                }
            }
        }

        drawNode(buffer, bufferWidth, bufferHeight, node, drawY);
        drawY += itemHeight;
    }

    // Draw scrollbar if needed
    if ((int)visibleNodes.size() > visibleItemCount) {
        scrollBar->draw(buffer, bufferWidth, bufferHeight);
    }
}

void TreeView::drawNode(uint32_t* buffer, int bufferWidth, int bufferHeight, TreeNode* node, int drawY) {
    int absX = getAbsoluteX();
    int baseIndent = 5;
    int nodeIndent = baseIndent + (node->depth * indentWidth);

    // Draw tree lines
    if (node->depth > 0) {
        int lineX = absX + baseIndent + ((node->depth - 1) * indentWidth) + indentWidth / 2;
        int lineStartY = drawY;
        int lineEndY = drawY + itemHeight / 2;

        // Vertical line from parent
        drawTreeLine(buffer, bufferWidth, bufferHeight, lineX, lineStartY, lineX, lineEndY);

        // Horizontal line to node
        int lineEndX = absX + nodeIndent - 5;
        drawTreeLine(buffer, bufferWidth, bufferHeight, lineX, lineEndY, lineEndX, lineEndY);
    }

    // Draw expand/collapse icon if node has children
    if (node->hasChildren()) {
        int iconCenterX = absX + nodeIndent + expandIconSize / 2;
        int iconCenterY = drawY + itemHeight / 2;
        drawExpandIcon(buffer, bufferWidth, bufferHeight, iconCenterX, iconCenterY, node->isExpanded);
        nodeIndent += expandIconSize + 5;
    }

    // Draw node text
    if (fontRenderer) {
        int textX = absX + nodeIndent;
        int textY = drawY + (itemHeight + fontRenderer->getTextHeight()) / 2;
        uint32_t txtColor = (node == selectedNode) ? selectedTextColor : textColor;
        fontRenderer->drawText(buffer, bufferWidth, bufferHeight, node->text, textX, textY, txtColor);
    }
}

void TreeView::drawExpandIcon(uint32_t* buffer, int bufferWidth, int bufferHeight, int centerX, int centerY, bool isExpanded) {
    int halfSize = expandIconSize / 2;

    // Draw box outline
    for (int px = centerX - halfSize; px <= centerX + halfSize; px++) {
        if (px >= 0 && px < bufferWidth) {
            if (centerY - halfSize >= 0 && centerY - halfSize < bufferHeight) {
                buffer[(centerY - halfSize) * bufferWidth + px] = expandIconColor;
            }
            if (centerY + halfSize >= 0 && centerY + halfSize < bufferHeight) {
                buffer[(centerY + halfSize) * bufferWidth + px] = expandIconColor;
            }
        }
    }
    for (int py = centerY - halfSize; py <= centerY + halfSize; py++) {
        if (py >= 0 && py < bufferHeight) {
            if (centerX - halfSize >= 0 && centerX - halfSize < bufferWidth) {
                buffer[py * bufferWidth + (centerX - halfSize)] = expandIconColor;
            }
            if (centerX + halfSize >= 0 && centerX + halfSize < bufferWidth) {
                buffer[py * bufferWidth + (centerX + halfSize)] = expandIconColor;
            }
        }
    }

    // Draw horizontal line (minus or part of plus)
    for (int px = centerX - halfSize + 2; px <= centerX + halfSize - 2; px++) {
        if (px >= 0 && px < bufferWidth && centerY >= 0 && centerY < bufferHeight) {
            buffer[centerY * bufferWidth + px] = expandIconColor;
        }
    }

    // Draw vertical line for plus (if not expanded)
    if (!isExpanded) {
        for (int py = centerY - halfSize + 2; py <= centerY + halfSize - 2; py++) {
            if (py >= 0 && py < bufferHeight && centerX >= 0 && centerX < bufferWidth) {
                buffer[py * bufferWidth + centerX] = expandIconColor;
            }
        }
    }
}

void TreeView::drawTreeLine(uint32_t* buffer, int bufferWidth, int bufferHeight, int x1, int y1, int x2, int y2) {
    if (x1 == x2) {
        // Vertical line
        int startY = std::min(y1, y2);
        int endY = std::max(y1, y2);
        for (int py = startY; py <= endY; py++) {
            if (x1 >= 0 && x1 < bufferWidth && py >= 0 && py < bufferHeight) {
                buffer[py * bufferWidth + x1] = lineColor;
            }
        }
    } else {
        // Horizontal line
        int startX = std::min(x1, x2);
        int endX = std::max(x1, x2);
        for (int px = startX; px <= endX; px++) {
            if (px >= 0 && px < bufferWidth && y1 >= 0 && y1 < bufferHeight) {
                buffer[y1 * bufferWidth + px] = lineColor;
            }
        }
    }
}

void TreeView::checkHover(int mouseX, int mouseY) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    if (mouseX >= absX && mouseX < absX + width - scrollBarWidth &&
        mouseY >= absY && mouseY < absY + height) {
        bool clickedExpandIcon;
        hoveredNode = getNodeAtPosition(mouseX, mouseY, clickedExpandIcon);
    } else {
        hoveredNode = nullptr;
    }

    if ((int)visibleNodes.size() > visibleItemCount) {
        scrollBar->checkHover(mouseX, mouseY);
    }
}

void TreeView::handleMouseButton(int mouseX, int mouseY, bool isPressed) {
    int absX = getAbsoluteX();

    // Check if clicking on scrollbar
    if ((int)visibleNodes.size() > visibleItemCount &&
        mouseX >= absX + width - scrollBarWidth && mouseX < absX + width) {
        scrollBar->handleMouseButton(mouseX, mouseY, isPressed);
        return;
    }

    if (isPressed) {
        bool clickedExpandIcon = false;
        TreeNode* clickedNode = getNodeAtPosition(mouseX, mouseY, clickedExpandIcon);

        if (clickedNode) {
            if (clickedExpandIcon && clickedNode->hasChildren()) {
                // Toggle expand/collapse
                clickedNode->toggleExpanded();
                buildVisibleNodesList();
            } else {
                // Select node
                selectedNode = clickedNode;
                if (selectionCallback) {
                    selectionCallback(selectedNode);
                }
            }
        }
    }
}

void TreeView::handleMouseMove(int mouseX, int mouseY) {
    if (scrollBar && scrollBar->isDragging()) {
        scrollBar->handleMouseMove(mouseX, mouseY);
    }
}

TreeNode* TreeView::getNodeAtPosition(int mouseX, int mouseY, bool& clickedExpandIcon) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();

    clickedExpandIcon = false;

    if (mouseX < absX + 1 || mouseX >= absX + width - scrollBarWidth - 1 ||
        mouseY < absY + 1 || mouseY >= absY + height - 1) {
        return nullptr;
    }

    int relativeY = mouseY - (absY + 1);
    int itemIndex = scrollOffset + (relativeY / itemHeight);

    if (itemIndex >= 0 && itemIndex < (int)visibleNodes.size()) {
        TreeNode* node = visibleNodes[itemIndex].node;

        // Check if clicked on expand icon
        if (node->hasChildren()) {
            int baseIndent = 5;
            int nodeIndent = baseIndent + (node->depth * indentWidth);
            int iconStartX = absX + nodeIndent;
            int iconEndX = iconStartX + expandIconSize;

            int itemY = absY + 1 + (itemIndex - scrollOffset) * itemHeight;
            int iconStartY = itemY + (itemHeight - expandIconSize) / 2;
            int iconEndY = iconStartY + expandIconSize;

            if (mouseX >= iconStartX && mouseX < iconEndX &&
                mouseY >= iconStartY && mouseY < iconEndY) {
                clickedExpandIcon = true;
            }
        }

        return node;
    }

    return nullptr;
}

void TreeView::setFontRenderer(FontRenderer* renderer) {
    Widget::setFontRenderer(renderer);
    if (scrollBar) {
        scrollBar->setFontRenderer(renderer);
    }
}

TreeNode* TreeView::addRootChild(const std::string& text) {
    TreeNode* child = root->addChild(text);
    buildVisibleNodesList();
    return child;
}

void TreeView::setSelectedNode(TreeNode* node) {
    selectedNode = node;
}

void TreeView::setSelectionCallback(std::function<void(TreeNode*)> callback) {
    selectionCallback = callback;
}

void TreeView::setItemHeight(int height) {
    itemHeight = height;
    visibleItemCount = (this->height - 2) / itemHeight;
    updateScrollBar();
}

void TreeView::setIndentWidth(int width) {
    indentWidth = width;
}

void TreeView::setExpandIconSize(int size) {
    expandIconSize = size;
}

void TreeView::setBackgroundColor(uint32_t color) {
    backgroundColor = color;
}

void TreeView::setItemBackgroundColor(uint32_t color) {
    itemBackgroundColor = color;
}

void TreeView::setSelectedBackgroundColor(uint32_t color) {
    selectedBackgroundColor = color;
}

void TreeView::setHoverBackgroundColor(uint32_t color) {
    hoverBackgroundColor = color;
}

void TreeView::setTextColor(uint32_t color) {
    textColor = color;
}

void TreeView::setSelectedTextColor(uint32_t color) {
    selectedTextColor = color;
}

void TreeView::setBorderColor(uint32_t color) {
    borderColor = color;
}

void TreeView::setLineColor(uint32_t color) {
    lineColor = color;
}

void TreeView::setExpandIconColor(uint32_t color) {
    expandIconColor = color;
}
