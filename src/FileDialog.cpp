#include "FileDialog.h"
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <cstring>

FileDialog::FileDialog(FileDialogMode mode, const std::string& title, const std::string& initialPath)
    : FileDialog(mode, title, 600, 500, initialPath) {}

FileDialog::FileDialog(FileDialogMode mode, const std::string& title, int width, int height, const std::string& initialPath)
    : DialogueBox(width, height, title),
      mode(mode),
      currentPath(initialPath),
      selectedFile(""),
      fileListBox(nullptr),
      fileNameTextBox(nullptr),
      pathLabel(nullptr),
      fileSelectedCallback(nullptr) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        if (currentPath == "." || currentPath.empty()) {
            currentPath = std::string(cwd);
        }
    }

    pathLabel = new TextLabel("Path: " + currentPath, 10, 10);
    getContentPanel()->add(pathLabel);

    PushButton* parentButton = new PushButton("..", 10, 35, 60, 25);
    parentButton->setClickCallback([this]() {
        std::string parentPath = getParentPath(currentPath);
        std::cout << "FileDialog: Navigating from " << currentPath << " to parent " << parentPath << std::endl;
        navigateToPath(parentPath);
    });
    getContentPanel()->add(parentButton);

    int listBoxHeight = height - titleBarHeight - buttonPanelHeight - 2 * borderWidth - 150;
    fileListBox = new ListBox(10, 65, width - 2 * borderWidth - 20, listBoxHeight);
    fileListBox->setSelectionCallback([this](int index, const std::string& item) {
        (void)index;
        handleFileSelection(item);
    });
    getContentPanel()->add(fileListBox);

    int fileNameLabelY = 65 + listBoxHeight + 10;
    TextLabel* fileNameLabel = new TextLabel("File name:", 10, fileNameLabelY);
    getContentPanel()->add(fileNameLabel);

    int fileNameBoxY = fileNameLabelY + 20;
    fileNameTextBox = new TextBox(10, fileNameBoxY, width - 2 * borderWidth - 20, 30);
    getContentPanel()->add(fileNameTextBox);

    clearButtons();

    addButton("OK", [this]() {
        handleOkButton();
    });

    addButton("Cancel", [this]() {
        handleCancelButton();
    });

    populateFileList();
}

FileDialog::~FileDialog() {
    delete fileListBox;
    delete fileNameTextBox;
    delete pathLabel;
    fileListBox = nullptr;
    fileNameTextBox = nullptr;
    pathLabel = nullptr;
}

void FileDialog::populateFileList() {
    fileListBox->clearItems();

    std::vector<std::string> entries = listDirectory(currentPath);

    std::cout << "FileDialog: Listing directory: " << currentPath << std::endl;
    std::cout << "FileDialog: Found " << entries.size() << " entries" << std::endl;

    std::vector<std::string> directories;
    std::vector<std::string> files;

    for (const std::string& entry : entries) {
        if (entry == "." || entry == "..") continue;

        std::string fullPath = currentPath + "/" + entry;
        if (isDirectory(fullPath)) {
            directories.push_back("[DIR] " + entry);
        } else {
            files.push_back(entry);
        }
    }

    std::sort(directories.begin(), directories.end());
    std::sort(files.begin(), files.end());

    std::cout << "FileDialog: Adding " << directories.size() << " directories and " << files.size() << " files" << std::endl;

    for (const std::string& dir : directories) {
        fileListBox->addItem(dir);
    }

    for (const std::string& file : files) {
        fileListBox->addItem(file);
    }

    if (pathLabel) {
        pathLabel->setText("Path: " + currentPath);
    }
}

void FileDialog::navigateToPath(const std::string& path) {
    std::cout << "FileDialog: navigateToPath called with: " << path << std::endl;
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
        char resolved[PATH_MAX];
        if (realpath(path.c_str(), resolved) != nullptr) {
            currentPath = std::string(resolved);
            std::cout << "FileDialog: Successfully navigated to: " << currentPath << std::endl;
            populateFileList();
        } else {
            std::cerr << "FileDialog: realpath failed for: " << path << std::endl;
        }
    } else {
        std::cerr << "FileDialog: Not a valid directory: " << path << std::endl;
    }
}

std::vector<std::string> FileDialog::listDirectory(const std::string& path) {
    std::vector<std::string> entries;
    DIR* dir = opendir(path.c_str());

    if (dir == nullptr) {
        return entries;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        entries.push_back(entry->d_name);
    }

    closedir(dir);
    return entries;
}

std::string FileDialog::getParentPath(const std::string& path) {
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash != std::string::npos && lastSlash > 0) {
        return path.substr(0, lastSlash);
    }
    return "/";
}

bool FileDialog::isDirectory(const std::string& path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) == 0) {
        return S_ISDIR(statbuf.st_mode);
    }
    return false;
}

void FileDialog::handleFileSelection(const std::string& item) {
    if (item.substr(0, 6) == "[DIR] ") {
        std::string dirName = item.substr(6);
        std::string newPath = currentPath + "/" + dirName;
        navigateToPath(newPath);
    } else {
        if (fileNameTextBox) {
            fileNameTextBox->setText(item);
        }
        selectedFile = currentPath + "/" + item;
        std::cout << "FileDialog: Selected file: " << selectedFile << std::endl;
    }
}

void FileDialog::handleOkButton() {
    std::string filename = fileNameTextBox ? fileNameTextBox->getText() : "";

    if (filename.empty()) {
        std::cerr << "FileDialog: No file name specified" << std::endl;
        return;
    }

    if (filename[0] == '/') {
        selectedFile = filename;
    } else {
        selectedFile = currentPath + "/" + filename;
    }

    if (mode == FileDialogMode::OPEN) {
        struct stat statbuf;
        if (stat(selectedFile.c_str(), &statbuf) != 0 || S_ISDIR(statbuf.st_mode)) {
            std::cerr << "FileDialog: File does not exist or is a directory" << std::endl;
            return;
        }
    }

    std::cout << "FileDialog: Selected file: " << selectedFile << std::endl;

    if (fileSelectedCallback) {
        fileSelectedCallback(selectedFile);
    }

    close();
}

void FileDialog::handleCancelButton() {
    selectedFile = "";
    std::cout << "FileDialog: Cancelled" << std::endl;
    close();
}

void FileDialog::setFileSelectedCallback(std::function<void(const std::string&)> callback) {
    fileSelectedCallback = callback;
}

void FileDialog::setCurrentPath(const std::string& path) {
    navigateToPath(path);
}

void FileDialog::handleMouseButton(mfb_mouse_button button, mfb_key_mod mod, bool isPressed) {
    DialogueBox::handleMouseButton(button, mod, isPressed);
    if (button != MOUSE_BTN_1) return;

    if (isPressed) {
        if (getContentPanel() && getContentPanel()->containsPoint(mouseX, mouseY)) {
            for (Widget* child : getContentPanel()->getChildren()) {
                PushButton* pushButton = dynamic_cast<PushButton*>(child);
                if (pushButton && pushButton->checkClick(mouseX, mouseY)) {
                    pushButton->setPressed(true);
                    pressedButton = pushButton;
                    return;
                }
                if (fileListBox && fileListBox->containsPoint(mouseX, mouseY)) {
                    fileListBox->handleMouseButton(mouseX, mouseY, true);
                    return;
                }
            }
        }
    } else {
        if (getContentPanel()) {
            for (Widget* child : getContentPanel()->getChildren()) {
                PushButton* pushButton = dynamic_cast<PushButton*>(child);
                if (pushButton && pushButton == pressedButton) {
                    if (pushButton->checkClick(mouseX, mouseY)) {
                        pushButton->onClick();
                    }
                    pushButton->setPressed(false);
                    pressedButton = nullptr;
                }
            }
            if (fileListBox) {
                fileListBox->handleMouseButton(mouseX, mouseY, false);
            }
        }
    }
}

void FileDialog::handleMouseMove(int x, int y) {
    DialogueBox::handleMouseMove(x, y);
    if (getContentPanel()) {
        for (Widget* child : getContentPanel()->getChildren()) {
            PushButton* pushButton = dynamic_cast<PushButton*>(child);
            if (pushButton) {
                pushButton->checkHover(x, y);
            }
        }
    }
    if (fileListBox) {
        fileListBox->handleMouseMove(x, y);
        fileListBox->checkHover(x, y);
    }
}
