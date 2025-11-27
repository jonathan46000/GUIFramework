#ifndef GUI_FRAMEWORK_H
#define GUI_FRAMEWORK_H

#include "MiniFB.h"
#include "Widget.h"
#include "MenuBar.h"
#include "Panel.h"
#include "TextBox.h"
#include "MultiLineTextBox.h"
#include "PushButton.h"
#include "TextLabel.h"
#include "BooleanWidget.h"
#include "CheckBox.h"
#include "RadioButton.h"
#include "ScrollBar.h"
#include "ListBox.h"
#include "ContextMenu.h"
#include "CascadeMenu.h"
#include "DialogueBox.h"
#include "FileDialog.h"
#include "FontRenderer.h"
#include "DialogManager.h"
#include "ImageWidget.h"
#include "TabbedPanel.h"
#include "ComboBox.h"
#include "StatusBar.h"
#include "ProgressBar.h"
#include "Spinner.h"
#include "Splitter.h"
#include "TreeView.h"
#include "TableGrid.h"
#include "Canvas.h"
#include <vector>
#include <string>
#include <set>

class GUIFramework {
private:
    struct mfb_window* window;
    uint32_t* buffer;
    int width, height;
    uint32_t backgroundColor;
    int mouseX, mouseY;
    Widget* focusedWidget;
    Widget* lastFocusedWidget;
    TextBox* selectingTextBox;
    MultiLineTextBox* selectingMultiLineTextBox;
    PushButton* pressedButton;
    ScrollBar* draggedScrollBar;
    ListBox* draggedListBox;
    Splitter* draggedSplitter;
    TreeView* draggedTreeView;
    TableGrid* draggedTableGrid;
    Canvas* draggedCanvas;
    std::vector<Widget*> widgets;
    std::vector<MenuBar*> menuBars;
    std::vector<StatusBar*> statusBars;
    std::vector<ContextMenu*> contextMenus;
    FontRenderer* fontRenderer;
    std::string loadedFontPath;
    int loadedFontSize;
    std::set<mfb_key> keysPressed;

    static void resize_callback(struct mfb_window* window, int width, int height);
    static void mouse_button_callback(struct mfb_window* window, mfb_mouse_button button, mfb_key_mod mod, bool isPressed);
    static void mouse_move_callback(struct mfb_window* window, int x, int y);
    static void char_callback(struct mfb_window* window, unsigned int charCode);
    static void key_callback(struct mfb_window* window, mfb_key key, mfb_key_mod mod, bool isPressed);

    void handleResize(int width, int height);
    void handleMouseButton(mfb_mouse_button button, mfb_key_mod mod, bool isPressed);
    void handleMouseMove(int x, int y);
    void handleChar(unsigned int charCode);
    void handleKey(mfb_key key, mfb_key_mod mod, bool isPressed);

    std::vector<std::string> findSystemFonts();
    bool tryLoadFont(int size);

    void handleWidgetMouseButton(Widget* widget, int mouseX, int mouseY, bool isPressed);
    Widget* getTargetWidget();

public:
    GUIFramework(const char* title, int width, int height);
    ~GUIFramework();

    void add(Widget* widget);
    void addContextMenu(ContextMenu* contextMenu);
    DialogueBox* createDialogueBox(int width, int height, const std::string& title);
    FileDialog* createFileDialog(FileDialogMode mode, const std::string& title, int width, int height, const std::string& initialPath = ".");
    void setBackgroundColor(uint32_t color);
    bool loadFont(const char* fontPath, int size);
    bool loadSystemFont(int size);
    FontRenderer* getFontRenderer() { return fontRenderer; }

    void copyFromTextBox();
    void cutFromTextBox();
    void pasteToTextBox();
    void selectAllInTextBox();

    void run();
};

#endif
