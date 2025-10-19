#ifndef DIALOG_MANAGER_H
#define DIALOG_MANAGER_H

#include <memory>

class GUIFramework;
class DialogueBox;
class FileDialog;

class DialogManager {
public:
    DialogManager(GUIFramework& gui);
    ~DialogManager();
    void showTestDialog();
    void showOpenFileDialog();
    void showSaveFileDialog();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif
