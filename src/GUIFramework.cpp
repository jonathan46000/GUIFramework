#include "GUIFramework.h"
#include "DropDownMenu.h"
#include "ComboBox.h"
#include "CascadeMenu.h"
#include "TabbedPanel.h"
#include <X11/Xlib.h>
#include <fontconfig/fontconfig.h>
#include <iostream>

GUIFramework::GUIFramework(const char* title, int width, int height)
    : width(width),
      height(height),
      backgroundColor(MFB_RGB(240, 240, 240)),
      mouseX(0),
      mouseY(0),
      focusedWidget(nullptr),
      lastFocusedWidget(nullptr),
      selectingTextBox(nullptr),
      selectingMultiLineTextBox(nullptr),
      pressedButton(nullptr),
      draggedScrollBar(nullptr),
      draggedListBox(nullptr),
      loadedFontSize(12) {

    XInitThreads();

    buffer = new uint32_t[width * height];
    window = mfb_open_ex(title, width, height, WF_RESIZABLE);
    fontRenderer = new FontRenderer();

    if (window) {
        mfb_set_user_data(window, this);
        mfb_set_resize_callback(window, resize_callback);
        mfb_set_mouse_button_callback(window, mouse_button_callback);
        mfb_set_mouse_move_callback(window, mouse_move_callback);
        mfb_set_char_input_callback(window, char_callback);
        mfb_set_keyboard_callback(window, key_callback);
        mfb_set_close_callback(window, [](struct mfb_window*) -> bool {
            exit(0);
            return true;
        });
    }
}

GUIFramework::~GUIFramework() {
    for (Widget* widget : widgets) {
        delete widget;
    }
    for (ContextMenu* contextMenu : contextMenus) {
        delete contextMenu;
    }
    delete[] buffer;
    delete fontRenderer;
}

std::vector<std::string> GUIFramework::findSystemFonts() {
    std::vector<std::string> fonts;
    FcConfig* config = FcInitLoadConfigAndFonts();
    if (!config) return fonts;
    FcPattern* pattern = FcNameParse((const FcChar8*)"sans-serif");
    FcConfigSubstitute(config, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);
    FcResult result;
    FcFontSet* fontSet = FcFontSort(config, pattern, FcTrue, nullptr, &result);
    if (fontSet) {
        for (int i = 0; i < fontSet->nfont && fonts.size() < 10; i++) {
            FcChar8* file = nullptr;
            FcCharSet* charset = nullptr;
            if (FcPatternGetString(fontSet->fonts[i], FC_FILE, 0, &file) == FcResultMatch &&
                FcPatternGetCharSet(fontSet->fonts[i], FC_CHARSET, 0, &charset) == FcResultMatch) {
                if (FcCharSetHasChar(charset, 'A') && FcCharSetHasChar(charset, 'a')) {
                    fonts.push_back(std::string((char*)file));
                }
            }
        }
        FcFontSetDestroy(fontSet);
    }
    FcPatternDestroy(pattern);
    FcConfigDestroy(config);
    return fonts;
}

bool GUIFramework::tryLoadFont(int size) {
    std::vector<std::string> fonts = findSystemFonts();
    for (const std::string& fontPath : fonts) {
        if (fontRenderer->loadFont(fontPath.c_str(), size)) {
            loadedFontPath = fontPath;
            loadedFontSize = size;
            return true;
        }
    }
    return false;
}

bool GUIFramework::loadFont(const char* fontPath, int size) {
    bool result = fontRenderer->loadFont(fontPath, size);
    if (result) {
        loadedFontPath = fontPath;
        loadedFontSize = size;
    }
    return result;
}

bool GUIFramework::loadSystemFont(int size) {
    return tryLoadFont(size);
}

void GUIFramework::resize_callback(struct mfb_window* window, int width, int height) {
    GUIFramework* framework = (GUIFramework*)mfb_get_user_data(window);
    framework->handleResize(width, height);
}

void GUIFramework::mouse_button_callback(struct mfb_window* window, mfb_mouse_button button, mfb_key_mod mod, bool isPressed) {
    GUIFramework* framework = (GUIFramework*)mfb_get_user_data(window);
    framework->handleMouseButton(button, mod, isPressed);
}

void GUIFramework::mouse_move_callback(struct mfb_window* window, int x, int y) {
    GUIFramework* framework = (GUIFramework*)mfb_get_user_data(window);
    framework->handleMouseMove(x, y);
}

void GUIFramework::char_callback(struct mfb_window* window, unsigned int charCode) {
    GUIFramework* framework = (GUIFramework*)mfb_get_user_data(window);
    framework->handleChar(charCode);
}

void GUIFramework::key_callback(struct mfb_window* window, mfb_key key, mfb_key_mod mod, bool isPressed) {
    GUIFramework* framework = (GUIFramework*)mfb_get_user_data(window);
    framework->handleKey(key, mod, isPressed);
}

void GUIFramework::handleResize(int newWidth, int newHeight) {
    delete[] buffer;
    buffer = new uint32_t[newWidth * newHeight];
    width = newWidth;
    height = newHeight;
    for (MenuBar* menuBar : menuBars) {
        menuBar->onWindowResize(width, height);
    }
    for (StatusBar* statusBar : statusBars) {
        statusBar->onWindowResize(width, height);
    }
}

Widget* GUIFramework::getTargetWidget() {
    if (focusedWidget) return focusedWidget;
    return lastFocusedWidget;
}

void GUIFramework::copyFromTextBox() {
    Widget* target = getTargetWidget();
    if (target) {
        target->copy();
    }
}

void GUIFramework::cutFromTextBox() {
    Widget* target = getTargetWidget();
    if (target) {
        target->cut();
    }
}

void GUIFramework::pasteToTextBox() {
    Widget* target = getTargetWidget();
    if (target) {
        target->paste();
    }
}

void GUIFramework::selectAllInTextBox() {
    Widget* target = getTargetWidget();
    if (target) {
        target->selectAll();
    }
}

void GUIFramework::handleWidgetMouseButton(Widget* widget, int mouseX, int mouseY, bool isPressed) {
    if (isPressed) {
        MultiLineTextBox* multiLineTextBox = dynamic_cast<MultiLineTextBox*>(widget);
        if (multiLineTextBox && multiLineTextBox->checkClick(mouseX, mouseY)) {
            if (focusedWidget) {
                TextBox* oldTextBox = dynamic_cast<TextBox*>(focusedWidget);
                if (oldTextBox) oldTextBox->setFocus(false);
                MultiLineTextBox* oldMultiLine = dynamic_cast<MultiLineTextBox*>(focusedWidget);
                if (oldMultiLine && oldMultiLine != multiLineTextBox) oldMultiLine->setFocus(false);
            }
            multiLineTextBox->setFocus(true);
            multiLineTextBox->handleMouseButton(mouseX, mouseY, true);
            focusedWidget = multiLineTextBox;
            lastFocusedWidget = multiLineTextBox;
            selectingMultiLineTextBox = multiLineTextBox;
            return;
        }

        TextBox* textBox = dynamic_cast<TextBox*>(widget);
        if (textBox && textBox->checkClick(mouseX, mouseY)) {
            if (focusedWidget) {
                TextBox* oldTextBox = dynamic_cast<TextBox*>(focusedWidget);
                if (oldTextBox) oldTextBox->setFocus(false);
                MultiLineTextBox* oldMultiLine = dynamic_cast<MultiLineTextBox*>(focusedWidget);
                if (oldMultiLine) oldMultiLine->setFocus(false);
            }
            textBox->setFocus(true);
            textBox->handleMouseButton(mouseX, mouseY, true);
            focusedWidget = textBox;
            lastFocusedWidget = textBox;
            selectingTextBox = textBox;
            return;
        }
        PushButton* button = dynamic_cast<PushButton*>(widget);
        if (button && button->checkClick(mouseX, mouseY)) {
            button->setPressed(true);
            pressedButton = button;
            return;
        }
        BooleanWidget* boolWidget = dynamic_cast<BooleanWidget*>(widget);
        if (boolWidget && boolWidget->checkClick(mouseX, mouseY)) {
            boolWidget->toggle();
            return;
        }
        ListBox* listBox = dynamic_cast<ListBox*>(widget);
        if (listBox && listBox->checkClick(mouseX, mouseY)) {
            listBox->handleMouseButton(mouseX, mouseY, true);
            draggedListBox = listBox;
            return;
        }
        ScrollBar* scrollBar = dynamic_cast<ScrollBar*>(widget);
        if (scrollBar && scrollBar->checkClick(mouseX, mouseY)) {
            scrollBar->handleMouseButton(mouseX, mouseY, true);
            if (scrollBar->isDragging()) draggedScrollBar = scrollBar;
            return;
        }
        DropDownMenu* menu = dynamic_cast<DropDownMenu*>(widget);
        if (menu) {
            if (menu->checkClick(mouseX, mouseY)) menu->toggle();
            else if (menu->getIsOpen()) {
                if (menu->checkMenuArea(mouseX, mouseY)) menu->handleMenuClick(mouseX, mouseY);
                else menu->close();
            }
            return;
        }
        ComboBox* comboBox = dynamic_cast<ComboBox*>(widget);
        if (comboBox) {
            if (comboBox->checkClick(mouseX, mouseY) || comboBox->getIsOpen()) {
                if (focusedWidget && focusedWidget != comboBox) {
                    TextBox* oldTextBox = dynamic_cast<TextBox*>(focusedWidget);
                    if (oldTextBox) oldTextBox->setFocus(false);
                    MultiLineTextBox* oldMultiLine = dynamic_cast<MultiLineTextBox*>(focusedWidget);
                    if (oldMultiLine) oldMultiLine->setFocus(false);
                    ComboBox* oldComboBox = dynamic_cast<ComboBox*>(focusedWidget);
                    if (oldComboBox) oldComboBox->setFocus(false);
                }
                comboBox->setFocus(true);
                comboBox->handleMouseButton(mouseX, mouseY, isPressed);
                focusedWidget = comboBox;
                lastFocusedWidget = comboBox;
                return;
            }
            return;
        }
        Spinner* spinner = dynamic_cast<Spinner*>(widget);
        if (spinner && spinner->checkClick(mouseX, mouseY)) {
            spinner->handleMouseButton(mouseX, mouseY, isPressed);
            return;
        }
        Panel* panel = dynamic_cast<Panel*>(widget);
        if (panel) {
            for (Widget* child : panel->getChildren()) {
                handleWidgetMouseButton(child, mouseX, mouseY, isPressed);
            }
        }
        TabbedPanel* tabbedPanel = dynamic_cast<TabbedPanel*>(widget);
        if (tabbedPanel) {
            int absY = tabbedPanel->getAbsoluteY();
            int headerHeight = 30; // Default header height

            // Check if clicking tab header
            if (mouseY >= absY && mouseY < absY + headerHeight) {
                // Tab header click - let TabbedPanel switch tabs
                tabbedPanel->handleMouseButton(mouseX, mouseY, isPressed);
            } else {
                // Content area click - recursively check active panel's children
                // Don't call tabbedPanel->handleMouseButton to avoid double-toggle
                Panel* activePanel = tabbedPanel->getActivePanel();
                if (activePanel) {
                    for (Widget* child : activePanel->getChildren()) {
                        handleWidgetMouseButton(child, mouseX, mouseY, isPressed);
                    }
                }
            }
        }
    } else {
        if (selectingTextBox) {
            selectingTextBox->handleMouseButton(mouseX, mouseY, false);
            selectingTextBox = nullptr;
        }
        if (selectingMultiLineTextBox) {
            selectingMultiLineTextBox->handleMouseButton(mouseX, mouseY, false);
            selectingMultiLineTextBox = nullptr;
        }
        if (pressedButton) {
            if (pressedButton->checkClick(mouseX, mouseY)) pressedButton->onClick();
            pressedButton->setPressed(false);
            pressedButton = nullptr;
        }
    }
}

void GUIFramework::handleMouseButton(mfb_mouse_button button, mfb_key_mod /*mod*/, bool isPressed) {
    if (button == MOUSE_BTN_1) {
        if (isPressed) {
            bool widgetClicked = false;
            for (ContextMenu* contextMenu : contextMenus) {
                if (contextMenu->getIsOpen()) {
                    if (contextMenu->checkMenuArea(mouseX, mouseY)) {
                        CascadeMenu* cascadeMenu = dynamic_cast<CascadeMenu*>(contextMenu);
                        if (cascadeMenu) {
                            cascadeMenu->checkMenuClick(mouseX, mouseY);
                        } else {
                            contextMenu->checkMenuClick(mouseX, mouseY);
                        }
                        widgetClicked = true;
                        break;
                    } else {
                        contextMenu->close();
                    }
                }
            }
            for (Widget* widget : widgets) {
                Panel* panel = dynamic_cast<Panel*>(widget);
                if (panel && panel->getContextMenu() && panel->getContextMenu()->getIsOpen()) {
                    if (panel->getContextMenu()->checkMenuArea(mouseX, mouseY)) {
                        CascadeMenu* cascadeMenu = dynamic_cast<CascadeMenu*>(panel->getContextMenu());
                        if (cascadeMenu) {
                            cascadeMenu->checkMenuClick(mouseX, mouseY);
                        } else {
                            panel->getContextMenu()->checkMenuClick(mouseX, mouseY);
                        }
                        widgetClicked = true;
                        break;
                    } else {
                        panel->getContextMenu()->close();
                    }
                }
            }
            if (!widgetClicked) {
                for (Widget* widget : widgets) {
                    MultiLineTextBox* multiLineTextBox = dynamic_cast<MultiLineTextBox*>(widget);
                    if (multiLineTextBox && multiLineTextBox->checkClick(mouseX, mouseY)) {
                        if (focusedWidget) {
                            TextBox* oldTextBox = dynamic_cast<TextBox*>(focusedWidget);
                            if (oldTextBox) oldTextBox->setFocus(false);
                            MultiLineTextBox* oldMultiLine = dynamic_cast<MultiLineTextBox*>(focusedWidget);
                            if (oldMultiLine && oldMultiLine != multiLineTextBox) oldMultiLine->setFocus(false);
                        }
                        multiLineTextBox->setFocus(true);
                        multiLineTextBox->handleMouseButton(mouseX, mouseY, true);
                        focusedWidget = multiLineTextBox;
                        lastFocusedWidget = multiLineTextBox;
                        selectingMultiLineTextBox = multiLineTextBox;
                        widgetClicked = true;
                        break;
                    }

                    TextBox* textBox = dynamic_cast<TextBox*>(widget);
                    if (textBox && textBox->checkClick(mouseX, mouseY)) {
                        if (focusedWidget) {
                            TextBox* oldTextBox = dynamic_cast<TextBox*>(focusedWidget);
                            if (oldTextBox && oldTextBox != textBox) oldTextBox->setFocus(false);
                            MultiLineTextBox* oldMultiLine = dynamic_cast<MultiLineTextBox*>(focusedWidget);
                            if (oldMultiLine) oldMultiLine->setFocus(false);
                        }
                        textBox->setFocus(true);
                        textBox->handleMouseButton(mouseX, mouseY, true);
                        focusedWidget = textBox;
                        lastFocusedWidget = textBox;
                        selectingTextBox = textBox;
                        widgetClicked = true;
                        break;
                    }
                    PushButton* pushButton = dynamic_cast<PushButton*>(widget);
                    if (pushButton && pushButton->checkClick(mouseX, mouseY)) {
                        pushButton->setPressed(true);
                        pressedButton = pushButton;
                        widgetClicked = true;
                        break;
                    }
                    BooleanWidget* boolWidget = dynamic_cast<BooleanWidget*>(widget);
                    if (boolWidget && boolWidget->checkClick(mouseX, mouseY)) {
                        boolWidget->toggle();
                        widgetClicked = true;
                        break;
                    }
                    ScrollBar* scrollBar = dynamic_cast<ScrollBar*>(widget);
                    if (scrollBar && scrollBar->checkClick(mouseX, mouseY)) {
                        scrollBar->handleMouseButton(mouseX, mouseY, true);
                        if (scrollBar->isDragging()) draggedScrollBar = scrollBar;
                        widgetClicked = true;
                        break;
                    }
                    ListBox* listBox = dynamic_cast<ListBox*>(widget);
                    if (listBox && listBox->checkClick(mouseX, mouseY)) {
                        listBox->handleMouseButton(mouseX, mouseY, true);
                        draggedListBox = listBox;
                        widgetClicked = true;
                        break;
                    }
                    DropDownMenu* dropdown = dynamic_cast<DropDownMenu*>(widget);
                    if (dropdown) {
                        if (dropdown->checkClick(mouseX, mouseY)) {
                            dropdown->toggle();
                            widgetClicked = true;
                            break;
                        } else if (dropdown->getIsOpen()) {
                            if (dropdown->checkMenuArea(mouseX, mouseY)) {
                                dropdown->handleMenuClick(mouseX, mouseY);
                                widgetClicked = true;
                                break;
                            } else {
                                dropdown->close();
                            }
                        }
                    }
                    ComboBox* comboBox = dynamic_cast<ComboBox*>(widget);
                    if (comboBox && (comboBox->checkClick(mouseX, mouseY) || comboBox->getIsOpen())) {
                        handleWidgetMouseButton(comboBox, mouseX, mouseY, isPressed);
                        widgetClicked = true;
                        break;
                    }
                    TabbedPanel* tabbedPanel = dynamic_cast<TabbedPanel*>(widget);
                    if (tabbedPanel && tabbedPanel->containsPoint(mouseX, mouseY)) {
                        handleWidgetMouseButton(tabbedPanel, mouseX, mouseY, isPressed);
                        widgetClicked = true;
                        break;
                    }
                    Panel* panel = dynamic_cast<Panel*>(widget);
                    if (panel && panel->containsPoint(mouseX, mouseY)) {
                        handleWidgetMouseButton(panel, mouseX, mouseY, isPressed);
                        widgetClicked = true;
                        break;
                    }
                }
            }
            if (!widgetClicked && focusedWidget) {
                TextBox* oldTextBox = dynamic_cast<TextBox*>(focusedWidget);
                if (oldTextBox) oldTextBox->setFocus(false);
                MultiLineTextBox* oldMultiLine = dynamic_cast<MultiLineTextBox*>(focusedWidget);
                if (oldMultiLine) oldMultiLine->setFocus(false);
                focusedWidget = nullptr;
            }
            for (MenuBar* menuBar : menuBars) {
                menuBar->handleMouseClick(mouseX, mouseY);
            }
        } else {
            if (draggedScrollBar) {
                draggedScrollBar->handleMouseButton(mouseX, mouseY, false);
                draggedScrollBar = nullptr;
            }
            if (draggedListBox) {
                draggedListBox->handleMouseButton(mouseX, mouseY, false);
                draggedListBox = nullptr;
            }
            if (selectingTextBox) {
                selectingTextBox->handleMouseButton(mouseX, mouseY, false);
                selectingTextBox = nullptr;
            }
            if (selectingMultiLineTextBox) {
                selectingMultiLineTextBox->handleMouseButton(mouseX, mouseY, false);
                selectingMultiLineTextBox = nullptr;
            }
            if (pressedButton) {
                if (pressedButton->checkClick(mouseX, mouseY)) pressedButton->onClick();
                pressedButton->setPressed(false);
                pressedButton = nullptr;
            }
        }
    } else if (button == MOUSE_BTN_2) {
        if (isPressed) {
            if (selectingTextBox) {
                selectingTextBox->stopSelecting();
                selectingTextBox = nullptr;
            }
            bool panelHandled = false;
            for (Widget* widget : widgets) {
                Panel* panel = dynamic_cast<Panel*>(widget);
                if (panel && panel->containsPoint(mouseX, mouseY)) {
                    if (panel->getContextMenu()) {
                        panel->handleRightClick(mouseX, mouseY);
                        panelHandled = true;
                        break;
                    }
                }
            }
            if (!panelHandled) {
                for (ContextMenu* contextMenu : contextMenus) {
                    contextMenu->open(mouseX, mouseY);
                }
            }
        }
    }
}

void GUIFramework::handleMouseMove(int x, int y) {
    mouseX = x;
    mouseY = y;
    for (MenuBar* menuBar : menuBars) menuBar->handleMouseMove(mouseX, mouseY);
    for (ContextMenu* contextMenu : contextMenus) {
        if (contextMenu->getIsOpen()) {
            CascadeMenu* cascadeMenu = dynamic_cast<CascadeMenu*>(contextMenu);
            if (cascadeMenu) {
                cascadeMenu->handleMouseMove(mouseX, mouseY);
            } else {
                contextMenu->handleMouseMove(mouseX, mouseY);
            }
        }
    }
    for (Widget* widget : widgets) {
        widget->handleMouseMove(mouseX, mouseY);
        widget->checkHover(mouseX, mouseY);
        DropDownMenu* dropdown = dynamic_cast<DropDownMenu*>(widget);
        if (dropdown && dropdown->getIsOpen()) dropdown->handleMouseMove(mouseX, mouseY);
        Panel* panel = dynamic_cast<Panel*>(widget);
        if (panel && panel->getContextMenu() && panel->getContextMenu()->getIsOpen()) {
            CascadeMenu* cascadeMenu = dynamic_cast<CascadeMenu*>(panel->getContextMenu());
            if (cascadeMenu) {
                cascadeMenu->handleMouseMove(mouseX, mouseY);
            } else {
                panel->getContextMenu()->handleMouseMove(mouseX, mouseY);
            }
        }
    }
    if (pressedButton) pressedButton->setPressed(pressedButton->checkClick(mouseX, mouseY));
    if (draggedScrollBar) draggedScrollBar->handleMouseMove(mouseX, mouseY);
    if (draggedListBox) draggedListBox->handleMouseMove(mouseX, mouseY);
    if (selectingTextBox) selectingTextBox->handleMouseMove(mouseX, mouseY);
    if (selectingMultiLineTextBox) selectingMultiLineTextBox->handleMouseMove(mouseX, mouseY);
}

void GUIFramework::handleChar(unsigned int charCode) {
    if (focusedWidget && charCode >= 32 && charCode <= 126 && !keysPressed.empty()) {
        if (keysPressed.find(KB_KEY_LEFT_CONTROL) == keysPressed.end() &&
            keysPressed.find(KB_KEY_RIGHT_CONTROL) == keysPressed.end()) {
            focusedWidget->handleChar(charCode);
        }
        keysPressed.clear();
    }
}

void GUIFramework::handleKey(mfb_key key, mfb_key_mod mod, bool isPressed) {
    if (isPressed) {
        if (focusedWidget) {
            if ((mod & KB_MOD_CONTROL) && key == KB_KEY_C) {
                copyFromTextBox();
                keysPressed.clear();
                return;
            } else if ((mod & KB_MOD_CONTROL) && key == KB_KEY_X) {
                cutFromTextBox();
                keysPressed.clear();
                return;
            } else if ((mod & KB_MOD_CONTROL) && key == KB_KEY_V) {
                pasteToTextBox();
                keysPressed.clear();
                return;
            } else if ((mod & KB_MOD_CONTROL) && key == KB_KEY_A) {
                selectAllInTextBox();
                keysPressed.clear();
                return;
            } else if (key == KB_KEY_BACKSPACE || key == KB_KEY_DELETE ||
                key == KB_KEY_LEFT || key == KB_KEY_RIGHT ||
                key == KB_KEY_UP || key == KB_KEY_DOWN ||
                key == KB_KEY_HOME || key == KB_KEY_END || key == KB_KEY_ENTER) {
                focusedWidget->handleKey(key, isPressed);
            } else {
                keysPressed.insert(key);
            }
        } else {
            keysPressed.insert(key);
        }
    } else {
        keysPressed.erase(key);
    }
}

void GUIFramework::add(Widget* widget) {
    widget->setFontRenderer(fontRenderer);
    MenuBar* menuBar = dynamic_cast<MenuBar*>(widget);
    if (menuBar) {
        menuBars.push_back(menuBar);
        menuBar->onWindowResize(width, height);
    }
    StatusBar* statusBar = dynamic_cast<StatusBar*>(widget);
    if (statusBar) {
        statusBars.push_back(statusBar);
        statusBar->onWindowResize(width, height);
    }
    widgets.push_back(widget);
}

void GUIFramework::addContextMenu(ContextMenu* contextMenu) {
    contextMenu->setFontRenderer(fontRenderer);
    contextMenus.push_back(contextMenu);
}

DialogueBox* GUIFramework::createDialogueBox(int width, int height, const std::string& title) {
    DialogueBox* dialog = new DialogueBox(width, height, title);
    if (fontRenderer && !loadedFontPath.empty()) {
        FontRenderer* dialogFont = new FontRenderer();
        if (dialogFont->loadFont(loadedFontPath.c_str(), loadedFontSize)) {
            dialog->setFontRenderer(dialogFont);
        } else {
            delete dialogFont;
        }
    }
    return dialog;
}

FileDialog* GUIFramework::createFileDialog(FileDialogMode mode, const std::string& title, int width, int height, const std::string& initialPath) {
    FileDialog* dialog = new FileDialog(mode, title, width, height, initialPath);
    if (fontRenderer && !loadedFontPath.empty()) {
        FontRenderer* dialogFont = new FontRenderer();
        if (dialogFont->loadFont(loadedFontPath.c_str(), loadedFontSize)) {
            dialog->setFontRenderer(dialogFont);
        } else {
            delete dialogFont;
        }
    }
    return dialog;
}

void GUIFramework::setBackgroundColor(uint32_t color) {
    backgroundColor = color;
}

void GUIFramework::run() {
    if (!window) return;
    do {
        for (int i = 0; i < width * height; i++) buffer[i] = backgroundColor;
        for (Widget* widget : widgets) {
            MenuBar* menuBar = dynamic_cast<MenuBar*>(widget);
            DropDownMenu* dropdown = dynamic_cast<DropDownMenu*>(widget);
            ComboBox* comboBox = dynamic_cast<ComboBox*>(widget);
            if (!menuBar && !dropdown && !(comboBox && comboBox->getIsOpen())) widget->draw(buffer, width, height);
        }
        for (Widget* widget : widgets) {
            DropDownMenu* dropdown = dynamic_cast<DropDownMenu*>(widget);
            if (dropdown && !dropdown->getIsOpen()) dropdown->draw(buffer, width, height);
        }
        for (MenuBar* menuBar : menuBars) menuBar->draw(buffer, width, height);
        for (ContextMenu* contextMenu : contextMenus) contextMenu->draw(buffer, width, height);
        for (Widget* widget : widgets) {
            Panel* panel = dynamic_cast<Panel*>(widget);
            if (panel && panel->getContextMenu()) panel->getContextMenu()->draw(buffer, width, height);
        }
        for (Widget* widget : widgets) {
            DropDownMenu* dropdown = dynamic_cast<DropDownMenu*>(widget);
            if (dropdown && dropdown->getIsOpen()) dropdown->draw(buffer, width, height);
        }
        for (Widget* widget : widgets) {
            ComboBox* comboBox = dynamic_cast<ComboBox*>(widget);
            if (comboBox && comboBox->getIsOpen()) comboBox->drawDropdownMenu(buffer, width, height);
        }
        // Also check nested ComboBoxes in panels/tabs
        for (Widget* widget : widgets) {
            Panel* panel = dynamic_cast<Panel*>(widget);
            if (panel) {
                for (Widget* child : panel->getChildren()) {
                    ComboBox* comboBox = dynamic_cast<ComboBox*>(child);
                    if (comboBox && comboBox->getIsOpen()) comboBox->drawDropdownMenu(buffer, width, height);
                }
            }
            TabbedPanel* tabPanel = dynamic_cast<TabbedPanel*>(widget);
            if (tabPanel) {
                Panel* activePanel = tabPanel->getActivePanel();
                if (activePanel) {
                    for (Widget* child : activePanel->getChildren()) {
                        ComboBox* comboBox = dynamic_cast<ComboBox*>(child);
                        if (comboBox && comboBox->getIsOpen()) comboBox->drawDropdownMenu(buffer, width, height);
                    }
                }
            }
        }

        mfb_update_state state = mfb_update_ex(window, buffer, width, height);
        if (state != STATE_OK) break;

    } while(mfb_wait_sync(window));
}
