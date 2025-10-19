#include "DialogManager.h"
#include "GUIFramework.h"
#include "DialogueBox.h"
#include "FileDialog.h"
#include <memory>
#include <vector>
#include <iostream>

struct DialogManager::Impl {
    GUIFramework& gui;
    std::vector<std::unique_ptr<DialogueBox>> dialogs;

    Impl(GUIFramework& g) : gui(g) {}

    std::unique_ptr<DialogueBox> extractDialog(DialogueBox* dialog) {
        for (auto it = dialogs.begin(); it != dialogs.end(); ++it) {
            if (it->get() == dialog) {
                auto extracted = std::move(*it);
                dialogs.erase(it);
                return extracted;
            }
        }
        return nullptr;
    }
};

DialogManager::DialogManager(GUIFramework& gui) : pImpl(std::make_unique<Impl>(gui)) {}

DialogManager::~DialogManager() = default;

void DialogManager::showTestDialog() {
    auto dialog = std::unique_ptr<DialogueBox>(pImpl->gui.createDialogueBox(300, 200, "Test Dialog"));
    auto* rawDlg = dialog.get();
    rawDlg->getContentPanel()->add(new TextLabel("This is a test dialogue box!", 10, 10));
    rawDlg->getContentPanel()->add(new TextLabel("It has multiple lines of content", 10, 35));
    auto* textBox = new TextBox(10, 60, 280, 30);
    textBox->setText("Enter text here...");
    rawDlg->getContentPanel()->add(textBox);
    rawDlg->addButton("OK", [rawDlg]() {
        std::cout << "OK clicked!" << std::endl;
        rawDlg->close();
    });
    rawDlg->addButton("Cancel", [rawDlg]() {
        std::cout << "Cancel clicked!" << std::endl;
        rawDlg->close();
    });
    rawDlg->setCloseCallback([this, rawDlg]() {
        std::cout << "DialogueBox: Deleting dialog via unique_ptr" << std::endl;
        auto extracted = pImpl->extractDialog(rawDlg);
    });
    rawDlg->show();
    pImpl->dialogs.push_back(std::move(dialog));
}

void DialogManager::showOpenFileDialog() {
    auto dialog = std::unique_ptr<FileDialog>(pImpl->gui.createFileDialog(FileDialogMode::OPEN, "Open File", 700, 550));
    auto* rawDlg = dialog.get();
    rawDlg->setFileSelectedCallback([](const std::string& filepath) {
        std::cout << "File opened: " << filepath << std::endl;
    });
    rawDlg->setCloseCallback([this, rawDlg]() {
        std::cout << "FileDialog: Deleting dialog via unique_ptr" << std::endl;
        auto extracted = pImpl->extractDialog(rawDlg);
    });
    rawDlg->show();
    pImpl->dialogs.push_back(std::move(dialog));
}

void DialogManager::showSaveFileDialog() {
    auto dialog = std::unique_ptr<FileDialog>(pImpl->gui.createFileDialog(FileDialogMode::SAVE, "Save File", 700, 550));
    auto* rawDlg = dialog.get();
    rawDlg->setFileSelectedCallback([](const std::string& filepath) {
        std::cout << "File saved to: " << filepath << std::endl;
    });
    rawDlg->setCloseCallback([this, rawDlg]() {
        std::cout << "FileDialog: Deleting dialog via unique_ptr" << std::endl;
        auto extracted = pImpl->extractDialog(rawDlg);
    });
    rawDlg->show();
    pImpl->dialogs.push_back(std::move(dialog));
}
