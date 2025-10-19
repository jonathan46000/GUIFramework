#include "DialogueBox.h"
#include <algorithm>
#include <X11/Xlib.h>
#include <iostream>
#include <thread>
#include <chrono>

DialogueBox::DialogueBox(int width, int height, const std::string& title)
    : window(nullptr),
      buffer(nullptr),
      width(width),
      height(height),
      contentPanel(nullptr),
      buttonPanel(nullptr),
      titleLabel(nullptr),
      isModal(true),
      isOpen(false),
      shouldClose(false),
      drawBorder(true),
      backgroundColor(0xFFF0F0F0),
      titleBackgroundColor(MFB_RGB(0, 120, 215)),
      borderColor(0xFF808080),
      titleTextColor(MFB_RGB(255, 255, 255)),
      titleBarHeight(30),
      buttonPanelHeight(50),
      borderWidth(2),
      buttonSpacing(10),
      mouseX(0),
      mouseY(0),
      pressedButton(nullptr),
      focusedTextBox(nullptr),
      selectingTextBox(nullptr),
      fontRenderer(nullptr),
      closeCallback(nullptr),
      windowThread(nullptr),
      x11Display(nullptr),
      x11Window(0),
      threadActive(false) {
    buffer = new uint32_t[width * height];
    fontRenderer = new FontRenderer();
    setupTitleBar();
    setupContentPanel();
    setupButtonPanel();
    setTitle(title);
}

DialogueBox::~DialogueBox() {
    if (threadActive && windowThread) {
        shouldClose = true;
        if (window) {
            mfb_set_user_data(window, nullptr);
            mfb_close(window);
            if (x11Display && x11Window) {
                XUnmapWindow(x11Display, x11Window);
                XDestroyWindow(x11Display, x11Window);
                XFlush(x11Display);
                std::cout << "DialogueBox: X11 window destroyed in destructor" << std::endl;
            }
            window = nullptr;
            std::cout << "DialogueBox: MiniFB window closed in destructor" << std::endl;
        }
        if (x11Display) {
            XCloseDisplay(x11Display);
            x11Display = nullptr;
            std::cout << "DialogueBox: X11 display closed in destructor" << std::endl;
        }
        if (windowThread->joinable()) {
            if (std::this_thread::get_id() != windowThread->get_id()) {
                try {
                    windowThread->join();
                    std::cout << "DialogueBox: windowThread joined in destructor" << std::endl;
                } catch (const std::system_error& e) {
                    std::cerr << "DialogueBox: Failed to join windowThread in destructor: " << e.what() << std::endl;
                    windowThread->detach();
                    std::cout << "DialogueBox: windowThread detached in destructor" << std::endl;
                }
            } else {
                windowThread->detach();
                std::cout << "DialogueBox: windowThread detached in destructor (self-delete)" << std::endl;
            }
        }
        delete windowThread;
        windowThread = nullptr;
        threadActive = false;
        std::cout << "DialogueBox: windowThread reset in destructor" << std::endl;
    }
    if (contentPanel) {
        for (Widget* widget : contentPanel->getChildren()) {
            delete widget;
        }
        delete contentPanel;
        contentPanel = nullptr;
    }
    if (buttonPanel) {
        for (Widget* widget : buttonPanel->getChildren()) {
            delete widget;
        }
        delete buttonPanel;
        buttonPanel = nullptr;
    }
    for (PushButton* button : buttons) {
        delete button;
    }
    buttons.clear();
    delete titleLabel;
    titleLabel = nullptr;
    delete fontRenderer;
    fontRenderer = nullptr;
    delete[] buffer;
    buffer = nullptr;
    if (closeCallback) {
        std::cout << "DialogueBox: Calling closeCallback" << std::endl;
        closeCallback();
    }
}

void DialogueBox::setupTitleBar() {
    titleLabel = new TextLabel("", borderWidth + 5, borderWidth + 5,
                               width - 2 * borderWidth - 10, titleBarHeight - 10);
    titleLabel->setAutoSize(false);
    titleLabel->setTextColor(titleTextColor);
    if (fontRenderer) {
        titleLabel->setFontRenderer(fontRenderer);
    }
}

void DialogueBox::setupContentPanel() {
    int contentY = borderWidth + titleBarHeight;
    int contentHeight = height - titleBarHeight - buttonPanelHeight - 2 * borderWidth;
    contentPanel = new Panel(borderWidth, contentY,
                            width - 2 * borderWidth, contentHeight);
    contentPanel->setBackgroundColor(backgroundColor);
    contentPanel->setDrawBorder(false);
    if (fontRenderer) {
        contentPanel->setFontRenderer(fontRenderer);
    }
}

void DialogueBox::setupButtonPanel() {
    int buttonY = height - buttonPanelHeight - borderWidth;
    buttonPanel = new Panel(borderWidth, buttonY,
                           width - 2 * borderWidth, buttonPanelHeight);
    buttonPanel->setBackgroundColor(backgroundColor);
    if (fontRenderer) {
        buttonPanel->setFontRenderer(fontRenderer);
    }
}

void DialogueBox::setupX11CloseProtocol() {
    if (!window) {
        std::cerr << "DialogueBox: No window for X11 setup" << std::endl;
        return;
    }
    x11Display = XOpenDisplay(nullptr);
    if (!x11Display) {
        std::cerr << "DialogueBox: Failed to open X11 display" << std::endl;
        return;
    }
    x11Window = 0;
    if (!titleLabel) {
        std::cerr << "DialogueBox: titleLabel is null in setupX11CloseProtocol" << std::endl;
        XCloseDisplay(x11Display);
        x11Display = nullptr;
        return;
    }
    for (int retry = 0; retry < 3 && !x11Window; ++retry) {
        Window root = DefaultRootWindow(x11Display);
        Window parent;
        Window* children = nullptr;
        unsigned int nchildren;
        if (XQueryTree(x11Display, root, &root, &parent, &children, &nchildren)) {
            for (unsigned int i = 0; i < nchildren; i++) {
                char* window_name = nullptr;
                if (XFetchName(x11Display, children[i], &window_name)) {
                    if (window_name && titleLabel->getText() == window_name) {
                        x11Window = children[i];
                        XFree(window_name);
                        break;
                    }
                    if (window_name) XFree(window_name);
                }
            }
            if (children) XFree(children);
        }
        if (!x11Window) {
            std::cerr << "DialogueBox: Retry " << retry + 1 << " failed to find X11 window handle" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    if (!x11Window) {
        std::cerr << "DialogueBox: Failed to find X11 window handle after retries" << std::endl;
    } else {
        std::cout << "DialogueBox: X11 close protocol set up successfully" << std::endl;
    }
}

void DialogueBox::runEventLoop() {
    if (!titleLabel) {
        std::cerr << "DialogueBox: titleLabel is null in runEventLoop" << std::endl;
        isOpen = false;
        threadActive = false;
        return;
    }
    window = mfb_open_ex(titleLabel->getText().c_str(), width, height, 0);
    if (!window) {
        std::cerr << "DialogueBox: Failed to open window" << std::endl;
        isOpen = false;
        threadActive = false;
        return;
    }
    mfb_set_user_data(window, this);
    mfb_set_mouse_button_callback(window, mouse_button_callback);
    mfb_set_mouse_move_callback(window, mouse_move_callback);
    mfb_set_char_input_callback(window, char_callback);
    mfb_set_keyboard_callback(window, key_callback);
    mfb_set_close_callback(window, close_callback);
    setupX11CloseProtocol();
    isOpen = true;
    shouldClose = false;
    threadActive = true;
    std::cout << "DialogueBox: Event loop started" << std::endl;
    do {
        if (shouldClose) {
            std::cout << "DialogueBox: shouldClose triggered" << std::endl;
            break;
        }
        updateWindow();
        mfb_update_state state = mfb_update_ex(window, buffer, width, height);
        if (state != STATE_OK) {
            std::cerr << "DialogueBox: mfb_update_ex failed, state: " << state << std::endl;
            break;
        }
    } while (mfb_wait_sync(window));
    isOpen = false;
    std::cout << "DialogueBox: Closing window" << std::endl;
    if (window) {
        mfb_set_user_data(window, nullptr);
        mfb_close(window);
        window = nullptr;
        std::cout << "DialogueBox: MiniFB window closed" << std::endl;
    }
    if (x11Display && x11Window) {
        XUnmapWindow(x11Display, x11Window);
        XDestroyWindow(x11Display, x11Window);
        XFlush(x11Display);
        x11Window = 0;
        std::cout << "DialogueBox: X11 window destroyed" << std::endl;
    }
    if (x11Display) {
        XCloseDisplay(x11Display);
        x11Display = nullptr;
        std::cout << "DialogueBox: X11 display closed" << std::endl;
    }
    threadActive = false;
    std::cout << "DialogueBox: Event loop ended" << std::endl;
}

void DialogueBox::show() {
    if (windowThread && threadActive) {
        std::cerr << "DialogueBox: windowThread already active" << std::endl;
        if (windowThread->joinable()) {
            try {
                windowThread->join();
                std::cout << "DialogueBox: Cleared stale windowThread" << std::endl;
            } catch (const std::system_error& e) {
                std::cerr << "DialogueBox: Failed to join stale windowThread: " << e.what() << std::endl;
                windowThread->detach();
                std::cout << "DialogueBox: Stale windowThread detached" << std::endl;
            }
            delete windowThread;
            windowThread = nullptr;
            threadActive = false;
        }
    }
    if (window) {
        std::cerr << "DialogueBox: Clearing stale window" << std::endl;
        mfb_close(window);
        window = nullptr;
    }
    windowThread = new std::thread(&DialogueBox::runEventLoop, this);
    std::cout << "DialogueBox: New windowThread created" << std::endl;
}

void DialogueBox::close() {
    shouldClose = true;
    if (window) {
        mfb_set_user_data(window, nullptr);
    }
    std::cout << "DialogueBox: close() called, setting shouldClose flag" << std::endl;
}

void DialogueBox::setTitle(const std::string& title) {
    if (titleLabel) {
        titleLabel->setText(title);
    }
}

void DialogueBox::setModal(bool modal) {
    isModal = modal;
}

void DialogueBox::setCloseCallback(std::function<void()> callback) {
    closeCallback = callback;
}

void DialogueBox::setFontRenderer(FontRenderer* renderer) {
    if (renderer && fontRenderer) {
        delete fontRenderer;
    }
    fontRenderer = renderer;
    if (fontRenderer) {
        if (titleLabel) {
            titleLabel->setFontRenderer(fontRenderer);
        }
        if (contentPanel) {
            contentPanel->setFontRenderer(fontRenderer);
        }
        if (buttonPanel) {
            buttonPanel->setFontRenderer(fontRenderer);
        }
        for (PushButton* button : buttons) {
            if (button) {
                button->setFontRenderer(fontRenderer);
            }
        }
    }
}

void DialogueBox::setBackgroundColor(uint32_t color) {
    backgroundColor = color;
    if (contentPanel) {
        contentPanel->setBackgroundColor(color);
    }
    if (buttonPanel) {
        buttonPanel->setBackgroundColor(color);
    }
}

void DialogueBox::setTitleBackgroundColor(uint32_t color) {
    titleBackgroundColor = color;
}

void DialogueBox::setBorderColor(uint32_t color) {
    borderColor = color;
}

void DialogueBox::setTitleTextColor(uint32_t color) {
    titleTextColor = color;
    if (titleLabel) {
        titleLabel->setTextColor(color);
    }
}

void DialogueBox::addButton(const std::string& label, std::function<void()> callback) {
    int buttonWidth = 80;
    int buttonHeight = 30;
    PushButton* button = new PushButton(label, 0, 0, buttonWidth, buttonHeight);
    button->setParent(buttonPanel);
    button->setClickCallback(callback);
    if (fontRenderer) {
        button->setFontRenderer(fontRenderer);
    }
    buttons.push_back(button);
    layoutButtons();
}

void DialogueBox::clearButtons() {
    for (PushButton* button : buttons) {
        delete button;
    }
    buttons.clear();
}

void DialogueBox::layoutButtons() {
    if (buttons.empty()) {
        return;
    }
    int buttonWidth = 80;
    int totalButtonWidth = buttons.size() * buttonWidth + (buttons.size() - 1) * buttonSpacing;
    int startX = (buttonPanel->getWidth() - totalButtonWidth) / 2;
    int buttonY = (buttonPanelHeight - 30) / 2;
    for (size_t i = 0; i < buttons.size(); i++) {
        int buttonX = startX + i * (buttonWidth + buttonSpacing);
        buttons[i]->setPosition(buttonX, buttonY);
    }
}

void DialogueBox::mouse_button_callback(struct mfb_window* window, mfb_mouse_button button, mfb_key_mod mod, bool isPressed) {
    DialogueBox* dialog = static_cast<DialogueBox*>(mfb_get_user_data(window));
    if (dialog) {
        dialog->handleMouseButton(button, mod, isPressed);
    }
}

void DialogueBox::mouse_move_callback(struct mfb_window* window, int x, int y) {
    DialogueBox* dialog = static_cast<DialogueBox*>(mfb_get_user_data(window));
    if (dialog) {
        dialog->handleMouseMove(x, y);
    }
}

void DialogueBox::char_callback(struct mfb_window* window, unsigned int charCode) {
    DialogueBox* dialog = static_cast<DialogueBox*>(mfb_get_user_data(window));
    if (dialog) {
        dialog->handleChar(charCode);
    }
}

void DialogueBox::key_callback(struct mfb_window* window, mfb_key key, mfb_key_mod mod, bool isPressed) {
    DialogueBox* dialog = static_cast<DialogueBox*>(mfb_get_user_data(window));
    if (dialog) {
        dialog->handleKey(key, mod, isPressed);
    }
}

bool DialogueBox::close_callback(struct mfb_window* window) {
    DialogueBox* dialog = static_cast<DialogueBox*>(mfb_get_user_data(window));
    if (dialog) {
        dialog->close();
    }
    return true;
}

void DialogueBox::updateWindow() {
    if (!buffer || !window) return;
    for (int i = 0; i < width * height; ++i) {
        buffer[i] = backgroundColor;
    }
    if (drawBorder) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (x < borderWidth || x >= width - borderWidth || y < borderWidth || y >= height - borderWidth) {
                    buffer[y * width + x] = borderColor;
                }
            }
        }
    }
    for (int y = borderWidth; y < borderWidth + titleBarHeight; ++y) {
        for (int x = borderWidth; x < width - borderWidth; ++x) {
            buffer[y * width + x] = titleBackgroundColor;
        }
    }
    if (titleLabel) titleLabel->draw(buffer, width, height);
    if (contentPanel) contentPanel->draw(buffer, width, height);
    if (buttonPanel) buttonPanel->draw(buffer, width, height);

    for (PushButton* button : buttons) {
        button->draw(buffer, width, height);
    }
}

TextBox* DialogueBox::findTextBox(Widget* widget) {
    TextBox* textBox = dynamic_cast<TextBox*>(widget);
    if (textBox && textBox->isFocusedWidget()) return textBox;
    Panel* panel = dynamic_cast<Panel*>(widget);
    if (panel) {
        for (Widget* child : panel->getChildren()) {
            TextBox* found = findTextBox(child);
            if (found) return found;
        }
    }
    return nullptr;
}

void DialogueBox::handleMouseButton(mfb_mouse_button button, mfb_key_mod mod, bool isPressed) {
    (void)mod;
    if (button != MOUSE_BTN_1) return;
    mouseX = mfb_get_mouse_x(window);
    mouseY = mfb_get_mouse_y(window);
    if (isPressed) {
        for (PushButton* pushButton : buttons) {
            if (pushButton && pushButton->checkClick(mouseX, mouseY)) {
                pushButton->setPressed(true);
                pressedButton = pushButton;
                return;
            }
        }

        if (contentPanel) {
            for (Widget* child : contentPanel->getChildren()) {
                PushButton* pushButton = dynamic_cast<PushButton*>(child);
                if (pushButton && pushButton->checkClick(mouseX, mouseY)) {
                    pushButton->setPressed(true);
                    pressedButton = pushButton;
                    return;
                }
                TextBox* textBox = dynamic_cast<TextBox*>(child);
                if (textBox && textBox->checkClick(mouseX, mouseY)) {
                    if (focusedTextBox && focusedTextBox != textBox) {
                        focusedTextBox->setFocus(false);
                    }
                    textBox->setFocus(true);
                    textBox->handleMouseButton(mouseX, mouseY, true);
                    focusedTextBox = textBox;
                    selectingTextBox = textBox;
                    return;
                }
            }
        }
    } else {
        if (pressedButton) {
            if (pressedButton->checkClick(mouseX, mouseY)) {
                pressedButton->onClick();
            }
            pressedButton->setPressed(false);
            pressedButton = nullptr;
        }
        if (selectingTextBox) {
            selectingTextBox->handleMouseButton(mouseX, mouseY, false);
            selectingTextBox = nullptr;
        }
    }
}

void DialogueBox::handleMouseMove(int x, int y) {
    mouseX = x;
    mouseY = y;
    if (contentPanel) contentPanel->handleMouseMove(x, y);
    if (buttonPanel) buttonPanel->handleMouseMove(x, y);

    for (PushButton* button : buttons) {
        button->checkHover(x, y);
    }

    if (pressedButton) {
        pressedButton->setPressed(pressedButton->checkClick(x, y));
    }
    if (selectingTextBox) {
        selectingTextBox->handleMouseMove(x, y);
    }
}

void DialogueBox::handleChar(unsigned int charCode) {
    if (focusedTextBox && charCode >= 32 && charCode <= 126 && !keysPressed.empty()) {
        if (keysPressed.find(KB_KEY_LEFT_CONTROL) == keysPressed.end() &&
            keysPressed.find(KB_KEY_RIGHT_CONTROL) == keysPressed.end()) {
            focusedTextBox->handleChar(charCode);
        }
        keysPressed.clear();
    }
}

void DialogueBox::handleKey(mfb_key key, mfb_key_mod mod, bool isPressed) {
    (void)mod;
    if (isPressed) {
        if (focusedTextBox) {
            if (key == KB_KEY_BACKSPACE || key == KB_KEY_DELETE ||
                key == KB_KEY_LEFT || key == KB_KEY_RIGHT ||
                key == KB_KEY_HOME || key == KB_KEY_END) {
                focusedTextBox->handleKey(key, isPressed);
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
