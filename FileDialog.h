#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include "DialogueBox.h"
#include "ListBox.h"
#include "TextBox.h"
#include "TextLabel.h"
#include "PushButton.h"
#include <string>
#include <vector>
#include <functional>

enum class FileDialogMode {
    OPEN,
    SAVE
};

class FileDialog : public DialogueBox {
private:
    FileDialogMode mode;
    std::string currentPath;
    std::string selectedFile;
    ListBox* fileListBox;
    TextBox* fileNameTextBox;
    TextLabel* pathLabel;
    std::function<void(const std::string&)> fileSelectedCallback;

    void populateFileList();
    void navigateToPath(const std::string& path);
    std::vector<std::string> listDirectory(const std::string& path);
    std::string getParentPath(const std::string& path);
    bool isDirectory(const std::string& path);
    void handleFileSelection(const std::string& item);
    void handleOkButton();
    void handleCancelButton();
    void handleMouseButton(mfb_mouse_button button, mfb_key_mod mod, bool isPressed) override;
    void handleMouseMove(int x, int y) override;

public:
    FileDialog(FileDialogMode mode, const std::string& title, const std::string& initialPath = ".");
    FileDialog(FileDialogMode mode, const std::string& title, int width, int height, const std::string& initialPath = ".");
    ~FileDialog();

    void setFileSelectedCallback(std::function<void(const std::string&)> callback);
    std::string getSelectedFile() const { return selectedFile; }
    void setCurrentPath(const std::string& path);
    std::string getCurrentPath() const { return currentPath; }
};

#endif
