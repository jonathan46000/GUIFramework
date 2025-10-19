#ifndef DIALOGUEBOX_H
#define DIALOGUEBOX_H

#include "Panel.h"
#include "TextLabel.h"
#include "PushButton.h"
#include "TextBox.h"
#include "FontRenderer.h"
#include "MiniFB.h"
#include <X11/Xlib.h>
#include <string>
#include <vector>
#include <functional>
#include <set>
#include <thread>

class DialogueBox {
public:
    DialogueBox(int width, int height, const std::string& title);
    virtual ~DialogueBox();

    virtual void show();
    virtual void close();
    void setTitle(const std::string& title);
    void setModal(bool modal);
    void setCloseCallback(std::function<void()> callback);
    void setFontRenderer(FontRenderer* renderer);
    void setBackgroundColor(uint32_t color);
    void setTitleBackgroundColor(uint32_t color);
    void setBorderColor(uint32_t color);
    void setTitleTextColor(uint32_t color);
    void addButton(const std::string& label, std::function<void()> callback);
    void clearButtons();

    Panel* getContentPanel() { return contentPanel; }
    Panel* getButtonPanel() { return buttonPanel; }

protected:
    struct mfb_window* window;
    uint32_t* buffer;
    int width, height;
    Panel* contentPanel;
    Panel* buttonPanel;
    TextLabel* titleLabel;
    bool isModal;
    bool isOpen;
    bool shouldClose;
    bool drawBorder;
    uint32_t backgroundColor;
    uint32_t titleBackgroundColor;
    uint32_t borderColor;
    uint32_t titleTextColor;
    int titleBarHeight;
    int buttonPanelHeight;
    int borderWidth;
    int buttonSpacing;
    int mouseX, mouseY;
    PushButton* pressedButton;
    TextBox* focusedTextBox;
    TextBox* selectingTextBox;
    FontRenderer* fontRenderer;
    std::vector<PushButton*> buttons;
    std::function<void()> closeCallback;
    std::thread* windowThread;
    Display* x11Display;
    unsigned long x11Window;
    bool threadActive;
    std::set<mfb_key> keysPressed;

    void setupTitleBar();
    void setupContentPanel();
    void setupButtonPanel();
    void setupX11CloseProtocol();
    void runEventLoop();
    void updateWindow();
    static void mouse_button_callback(struct mfb_window* window, mfb_mouse_button button, mfb_key_mod mod, bool isPressed);
    static void mouse_move_callback(struct mfb_window* window, int x, int y);
    static void char_callback(struct mfb_window* window, unsigned int charCode);
    static void key_callback(struct mfb_window* window, mfb_key key, mfb_key_mod mod, bool isPressed);
    static bool close_callback(struct mfb_window* window);
    TextBox* findTextBox(Widget* widget);
    virtual void handleMouseButton(mfb_mouse_button button, mfb_key_mod mod, bool isPressed);
    virtual void handleMouseMove(int x, int y);
    virtual void handleChar(unsigned int charCode);
    virtual void handleKey(mfb_key key, mfb_key_mod mod, bool isPressed);
    void layoutButtons();
};

#endif
