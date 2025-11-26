#ifndef TREEVIEW_H
#define TREEVIEW_H

#include "Widget.h"
#include "ScrollBar.h"
#include <vector>
#include <string>
#include <functional>

class TreeNode {
public:
    std::string text;
    std::vector<TreeNode*> children;
    TreeNode* parent;
    bool isExpanded;
    int depth;

    TreeNode(const std::string& text, TreeNode* parent = nullptr, int depth = 0)
        : text(text), parent(parent), isExpanded(false), depth(depth) {}

    ~TreeNode() {
        for (TreeNode* child : children) {
            delete child;
        }
    }

    TreeNode* addChild(const std::string& text) {
        TreeNode* child = new TreeNode(text, this, depth + 1);
        children.push_back(child);
        return child;
    }

    void removeChild(TreeNode* child) {
        for (auto it = children.begin(); it != children.end(); ++it) {
            if (*it == child) {
                delete *it;
                children.erase(it);
                return;
            }
        }
    }

    void toggleExpanded() {
        isExpanded = !isExpanded;
    }

    bool hasChildren() const {
        return !children.empty();
    }
};

class TreeView : public Widget {
private:
    TreeNode* root;
    TreeNode* selectedNode;
    TreeNode* hoveredNode;
    int scrollOffset;
    int itemHeight;
    int visibleItemCount;
    int indentWidth;
    int expandIconSize;
    ScrollBar* scrollBar;
    int scrollBarWidth;

    uint32_t backgroundColor;
    uint32_t itemBackgroundColor;
    uint32_t selectedBackgroundColor;
    uint32_t hoverBackgroundColor;
    uint32_t textColor;
    uint32_t selectedTextColor;
    uint32_t borderColor;
    uint32_t lineColor;
    uint32_t expandIconColor;

    std::function<void(TreeNode*)> selectionCallback;

    struct VisibleNode {
        TreeNode* node;
        int visualIndex;
    };
    std::vector<VisibleNode> visibleNodes;

    void buildVisibleNodesList();
    void buildVisibleNodesRecursive(TreeNode* node, int& visualIndex);
    void updateScrollBar();
    TreeNode* getNodeAtPosition(int mouseX, int mouseY, bool& clickedExpandIcon);
    void drawNode(uint32_t* buffer, int bufferWidth, int bufferHeight, TreeNode* node, int drawY);
    void drawExpandIcon(uint32_t* buffer, int bufferWidth, int bufferHeight, int centerX, int centerY, bool isExpanded);
    void drawTreeLine(uint32_t* buffer, int bufferWidth, int bufferHeight, int x1, int y1, int x2, int y2);

public:
    TreeView(int x, int y, int width, int height);
    ~TreeView();

    void draw(uint32_t* buffer, int bufferWidth, int bufferHeight) override;
    void checkHover(int mouseX, int mouseY) override;
    void handleMouseButton(int mouseX, int mouseY, bool isPressed) override;
    void handleMouseMove(int mouseX, int mouseY) override;
    void setFontRenderer(FontRenderer* renderer) override;

    TreeNode* getRoot() { return root; }
    TreeNode* addRootChild(const std::string& text);
    void setSelectedNode(TreeNode* node);
    TreeNode* getSelectedNode() const { return selectedNode; }
    void setSelectionCallback(std::function<void(TreeNode*)> callback);

    void setItemHeight(int height);
    void setIndentWidth(int width);
    void setExpandIconSize(int size);

    void setBackgroundColor(uint32_t color);
    void setItemBackgroundColor(uint32_t color);
    void setSelectedBackgroundColor(uint32_t color);
    void setHoverBackgroundColor(uint32_t color);
    void setTextColor(uint32_t color);
    void setSelectedTextColor(uint32_t color);
    void setBorderColor(uint32_t color);
    void setLineColor(uint32_t color);
    void setExpandIconColor(uint32_t color);
};

#endif
