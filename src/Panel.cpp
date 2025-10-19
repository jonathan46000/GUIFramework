#include "Panel.h"
#include "ContextMenu.h"
#include "CascadeMenu.h"
#include "BooleanWidget.h"
#include "ScrollBar.h"
#include "ListBox.h"
#include "TextBox.h"
#include "MultiLineTextBox.h"
#include <algorithm>

Panel::Panel(int x, int y, int width, int height)
    : Widget(x, y, width, height), backgroundColor(0xFFF0F0F0),
      borderColor(0xFF808080), drawBorder(true), contextMenu(nullptr) {
}

Panel::~Panel() {
    for (Widget* widget : children) {
        delete widget;
    }
}

void Panel::add(Widget* widget) {
    widget->setParent(this);
    widget->setFontRenderer(fontRenderer);
    children.push_back(widget);
}

void Panel::draw(uint32_t* buffer, int bufferWidth, int bufferHeight) {
    int absX = getAbsoluteX();
    int absY = getAbsoluteY();
    int endX = std::min(absX + width, bufferWidth);
    int endY = std::min(absY + height, bufferHeight);

    for (int py = absY; py < endY; py++) {
        for (int px = absX; px < endX; px++) {
            if (px >= 0 && py >= 0) {
                if (drawBorder && (py == absY || py == endY - 1 || px == absX || px == endX - 1)) {
                    buffer[py * bufferWidth + px] = borderColor;
                } else {
                    buffer[py * bufferWidth + px] = backgroundColor;
                }
            }
        }
    }

    for (Widget* widget : children) {
        int childAbsX = widget->getAbsoluteX();
        int childAbsY = widget->getAbsoluteY();
        int childEndX = childAbsX + widget->getWidth();
        int childEndY = childAbsY + widget->getHeight();

        if (childAbsX < absX + width && childEndX > absX &&
            childAbsY < absY + height && childEndY > absY) {
            widget->draw(buffer, bufferWidth, bufferHeight);
        }
    }
}

void Panel::handleMouseMove(int mouseX, int mouseY) {
    if (contextMenu && contextMenu->getIsOpen()) {
        CascadeMenu* cascadeMenu = dynamic_cast<CascadeMenu*>(contextMenu);
        if (cascadeMenu) {
            cascadeMenu->handleMouseMove(mouseX, mouseY);
        } else {
            contextMenu->handleMouseMove(mouseX, mouseY);
        }
    }

    for (Widget* widget : children) {
        widget->handleMouseMove(mouseX, mouseY);
        widget->checkHover(mouseX, mouseY);
    }
}

void Panel::handleMouseButton(int mouseX, int mouseY, bool isPressed) {
    if (isPressed) {
        if (contextMenu && contextMenu->getIsOpen()) {
            if (contextMenu->checkMenuArea(mouseX, mouseY)) {
                CascadeMenu* cascadeMenu = dynamic_cast<CascadeMenu*>(contextMenu);
                if (cascadeMenu) {
                    cascadeMenu->checkMenuClick(mouseX, mouseY);
                } else {
                    contextMenu->checkMenuClick(mouseX, mouseY);
                }
                return;
            } else {
                contextMenu->close();
            }
        }

        for (Widget* widget : children) {
            if (widget->checkClick(mouseX, mouseY)) {
                MultiLineTextBox* multiLineTextBox = dynamic_cast<MultiLineTextBox*>(widget);
                if (multiLineTextBox) {
                    multiLineTextBox->handleMouseButton(mouseX, mouseY, true);
                    return;
                }

                TextBox* textBox = dynamic_cast<TextBox*>(widget);
                if (textBox) {
                    textBox->handleMouseButton(mouseX, mouseY, true);
                    return;
                }

                BooleanWidget* boolWidget = dynamic_cast<BooleanWidget*>(widget);
                if (boolWidget) {
                    boolWidget->toggle();
                    return;
                }

                ScrollBar* scrollBar = dynamic_cast<ScrollBar*>(widget);
                if (scrollBar) {
                    scrollBar->handleMouseButton(mouseX, mouseY, true);
                    return;
                }

                ListBox* listBox = dynamic_cast<ListBox*>(widget);
                if (listBox) {
                    listBox->handleMouseButton(mouseX, mouseY, true);
                    return;
                }
            }
        }
    } else {
        for (Widget* widget : children) {
            MultiLineTextBox* multiLineTextBox = dynamic_cast<MultiLineTextBox*>(widget);
            if (multiLineTextBox) {
                multiLineTextBox->handleMouseButton(mouseX, mouseY, false);
            }

            TextBox* textBox = dynamic_cast<TextBox*>(widget);
            if (textBox) {
                textBox->handleMouseButton(mouseX, mouseY, false);
            }
        }
    }

    for (Widget* widget : children) {
        widget->handleMouseButton(mouseX, mouseY, isPressed);
    }
}

void Panel::handleRightClick(int mouseX, int mouseY) {
    if (contextMenu && containsPoint(mouseX, mouseY)) {
        contextMenu->open(mouseX, mouseY);
    }
}

void Panel::handleChar(unsigned int charCode) {
    for (Widget* widget : children) {
        widget->handleChar(charCode);
    }
}

void Panel::handleKey(int key, bool isPressed) {
    for (Widget* widget : children) {
        widget->handleKey(key, isPressed);
    }
}

void Panel::setFontRenderer(FontRenderer* renderer) {
    Widget::setFontRenderer(renderer);
    for (Widget* widget : children) {
        widget->setFontRenderer(renderer);
    }
    if (contextMenu) {
        contextMenu->setFontRenderer(renderer);
    }
}

void Panel::setContextMenu(ContextMenu* menu) {
    contextMenu = menu;
    if (contextMenu) {
        contextMenu->setFontRenderer(fontRenderer);
    }
}

void Panel::setBackgroundColor(uint32_t color) {
    backgroundColor = color;
}

void Panel::setBorderColor(uint32_t color) {
    borderColor = color;
}

void Panel::setDrawBorder(bool draw) {
    drawBorder = draw;
}
