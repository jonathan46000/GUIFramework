#include "GUIFramework.h"
#include <iostream>

int main() {
    GUIFramework gui("GUI Framework - Complete Widget Test", 1024, 600);
    DialogManager dialogManager(gui);

    if (!gui.loadSystemFont(12)) {
        return 1;
    }

    MenuBar* topBar = new MenuBar(0, 0, 1024, 30, false);

    DropDownMenu* fileMenu = new DropDownMenu("File", 60, 26);
    topBar->add(fileMenu);

    MenuItem* newItem = new MenuItem("New");
    newItem->setCallback([]() {
        std::cout << "New file clicked!" << std::endl;
    });
    fileMenu->add(newItem);

    MenuItem* openItem = new MenuItem("Open");
    openItem->setCallback([&dialogManager]() {
        std::cout << "Open file clicked!" << std::endl;
        dialogManager.showOpenFileDialog();
    });
    fileMenu->add(openItem);

    CascadeMenuItem* saveItem = new CascadeMenuItem("Save");
    ContextMenu* saveSubmenu = new ContextMenu(150, 25);

    MenuItem* saveCurrentItem = new MenuItem("Save");
    saveCurrentItem->setCallback([]() {
        std::cout << "Save current file!" << std::endl;
    });
    saveSubmenu->add(saveCurrentItem);

    MenuItem* saveAsItem = new MenuItem("Save As...");
    saveAsItem->setCallback([&dialogManager]() {
        std::cout << "Save As clicked!" << std::endl;
        dialogManager.showSaveFileDialog();
    });
    saveSubmenu->add(saveAsItem);

    saveItem->setSubmenu(saveSubmenu);
    fileMenu->add(saveItem);

    MenuItem* exitItem = new MenuItem("Exit");
    exitItem->setCallback([]() {
        std::cout << "Exit clicked!" << std::endl;
        exit(0);
    });
    fileMenu->add(exitItem);

    DropDownMenu* editMenu = new DropDownMenu("Edit", 60, 26);
    topBar->add(editMenu);

    MenuItem* cutMenuItem = new MenuItem("Cut");
    cutMenuItem->setCallback([&gui]() {
        gui.cutFromTextBox();
        std::cout << "Cut from Edit menu" << std::endl;
    });
    editMenu->add(cutMenuItem);

    MenuItem* copyMenuItem = new MenuItem("Copy");
    copyMenuItem->setCallback([&gui]() {
        gui.copyFromTextBox();
        std::cout << "Copy from Edit menu" << std::endl;
    });
    editMenu->add(copyMenuItem);

    MenuItem* pasteMenuItem = new MenuItem("Paste");
    pasteMenuItem->setCallback([&gui]() {
        gui.pasteToTextBox();
        std::cout << "Paste from Edit menu" << std::endl;
    });
    editMenu->add(pasteMenuItem);

    MenuBar* bottomBar = new MenuBar(0, 575, 1024, 25, true);

    CascadeMenu* contextMenu = new CascadeMenu(180, 25);

    MenuItem* ctxCopy = new MenuItem("Copy");
    ctxCopy->setCallback([&gui]() {
        gui.copyFromTextBox();
        std::cout << "Context menu: Copy" << std::endl;
    });
    contextMenu->add(ctxCopy);

    MenuItem* ctxPaste = new MenuItem("Paste");
    ctxPaste->setCallback([&gui]() {
        gui.pasteToTextBox();
        std::cout << "Context menu: Paste" << std::endl;
    });
    contextMenu->add(ctxPaste);

    MenuItem* ctxCut = new MenuItem("Cut");
    ctxCut->setCallback([&gui]() {
        gui.cutFromTextBox();
        std::cout << "Context menu: Cut" << std::endl;
    });
    contextMenu->add(ctxCut);

    MenuItem* ctxSelectAll = new MenuItem("Select All");
    ctxSelectAll->setCallback([&gui]() {
        gui.selectAllInTextBox();
        std::cout << "Context menu: Select All" << std::endl;
    });
    contextMenu->add(ctxSelectAll);

    MenuItem* ctxSeparator = new MenuItem("----------");
    contextMenu->add(ctxSeparator);

    CascadeMenuItem* ctxMoreOptions = new CascadeMenuItem("More Options");
    ContextMenu* moreSubmenu = new ContextMenu(150, 25);

    MenuItem* option1 = new MenuItem("Option 1");
    option1->setCallback([]() {
        std::cout << "Context menu: Option 1" << std::endl;
    });
    moreSubmenu->add(option1);

    MenuItem* option2 = new MenuItem("Option 2");
    option2->setCallback([]() {
        std::cout << "Context menu: Option 2" << std::endl;
    });
    moreSubmenu->add(option2);

    ctxMoreOptions->setSubmenu(moreSubmenu);
    contextMenu->add(ctxMoreOptions);

    MenuItem* ctxProperties = new MenuItem("Properties");
    ctxProperties->setCallback([]() {
        std::cout << "Context menu: Properties" << std::endl;
    });
    contextMenu->add(ctxProperties);

    ImageWidget* imageWidget = new ImageWidget(50, 50, 200, 200, "../Resources/GUIFrameWork2.png");
    if (!imageWidget->hasImage()) {
        std::cerr << "Warning: Failed to load GUIFrameWork2.png" << std::endl;
    }

    Panel* textBoxPanel = new Panel(50, 270, 350, 150);
    textBoxPanel->setBackgroundColor(0xFFE0E0FF);

    CascadeMenu* textBoxContextMenu = new CascadeMenu(150, 25);

    MenuItem* ctxCutText = new MenuItem("Cut Text");
    ctxCutText->setCallback([&gui]() {
        gui.cutFromTextBox();
        std::cout << "TextBox context menu: Cut Text" << std::endl;
    });
    textBoxContextMenu->add(ctxCutText);

    MenuItem* ctxCopyText = new MenuItem("Copy Text");
    ctxCopyText->setCallback([&gui]() {
        gui.copyFromTextBox();
        std::cout << "TextBox context menu: Copy Text" << std::endl;
    });
    textBoxContextMenu->add(ctxCopyText);

    MenuItem* ctxPasteText = new MenuItem("Paste Text");
    ctxPasteText->setCallback([&gui]() {
        gui.pasteToTextBox();
        std::cout << "TextBox context menu: Paste Text" << std::endl;
    });
    textBoxContextMenu->add(ctxPasteText);

    MenuItem* ctxSelectAllText = new MenuItem("Select All");
    ctxSelectAllText->setCallback([&gui]() {
        gui.selectAllInTextBox();
        std::cout << "TextBox context menu: Select All" << std::endl;
    });
    textBoxContextMenu->add(ctxSelectAllText);

    textBoxPanel->setContextMenu(textBoxContextMenu);

    TextLabel* label1 = new TextLabel("Enter your name:", 10, 10);
    textBoxPanel->add(label1);

    TextBox* textBox1 = new TextBox(10, 30, 300, 30);
    textBox1->setChangeCallback([](const std::string& text) {
        std::cout << "Name TextBox changed: " << text << std::endl;
    });
    textBoxPanel->add(textBox1);

    TextLabel* label2 = new TextLabel("Enter your email:", 10, 70);
    textBoxPanel->add(label2);

    TextBox* textBox2 = new TextBox(10, 90, 300, 30);
    textBox2->setChangeCallback([](const std::string& text) {
        std::cout << "Email TextBox changed: " << text << std::endl;
    });
    textBoxPanel->add(textBox2);

    TextLabel* textBoxPanelLabel = new TextLabel("(Supports Ctrl+C/X/V/A and right-click)", 10, 125);
    textBoxPanel->add(textBoxPanelLabel);

    Panel* panel2 = new Panel(450, 50, 300, 250);
    panel2->setBackgroundColor(0xFFE0FFE0);

    TextLabel* checkLabel = new TextLabel("Select options:", 10, 10);
    panel2->add(checkLabel);

    CheckBox* checkbox1 = new CheckBox("Enable notifications", 10, 30);
    checkbox1->setChangeCallback([](bool checked) {
        std::cout << "Notifications: " << (checked ? "enabled" : "disabled") << std::endl;
    });
    panel2->add(checkbox1);

    CheckBox* checkbox2 = new CheckBox("Auto-save", 10, 55);
    checkbox2->setChangeCallback([](bool checked) {
        std::cout << "Auto-save: " << (checked ? "enabled" : "disabled") << std::endl;
    });
    panel2->add(checkbox2);

    CheckBox* checkbox3 = new CheckBox("Dark mode", 10, 80);
    checkbox3->setChangeCallback([](bool checked) {
        std::cout << "Dark mode: " << (checked ? "enabled" : "disabled") << std::endl;
    });
    panel2->add(checkbox3);

    TextLabel* radioLabel = new TextLabel("Choose size:", 10, 110);
    panel2->add(radioLabel);

    std::vector<RadioButton*> sizeGroup;

    RadioButton* radio1 = new RadioButton("Small", 10, 130);
    radio1->setChangeCallback([](bool checked) {
        if (checked) std::cout << "Size: Small" << std::endl;
    });
    sizeGroup.push_back(radio1);
    panel2->add(radio1);

    RadioButton* radio2 = new RadioButton("Medium", 10, 155);
    radio2->setChangeCallback([](bool checked) {
        if (checked) std::cout << "Size: Medium" << std::endl;
    });
    sizeGroup.push_back(radio2);
    panel2->add(radio2);

    RadioButton* radio3 = new RadioButton("Large", 10, 180);
    radio3->setChangeCallback([](bool checked) {
        if (checked) std::cout << "Size: Large" << std::endl;
    });
    sizeGroup.push_back(radio3);
    panel2->add(radio3);

    for (RadioButton* radio : sizeGroup) {
        radio->setGroup(&sizeGroup);
    }

    TextLabel* multiLineLabel = new TextLabel("Multi-line text input:", 770, 50);

    MultiLineTextBox* multiLineTextBox = new MultiLineTextBox(770, 70, 240, 400);
    multiLineTextBox->setChangeCallback([](const std::string& text) {
        std::cout << "MultiLineTextBox changed: " << text << std::endl;
    });

    PushButton* showDialogButton = new PushButton("Show Dialog", 50, 450, 120, 30);
    showDialogButton->setClickCallback([&dialogManager]() {
        std::cout << "Showing dialog!" << std::endl;
        dialogManager.showTestDialog();
    });

    DropDownMenu* standaloneMenu = new DropDownMenu("Standalone", 100, 30, 180, 450);
    MenuItem* testItem = new MenuItem("Test Item");
    testItem->setCallback([]() {
        std::cout << "Standalone menu item clicked!" << std::endl;
    });
    standaloneMenu->add(testItem);

    ScrollBar* horizontalScrollBar = new ScrollBar(50, 510, 350, ScrollBarOrientation::HORIZONTAL);
    horizontalScrollBar->setRange(0, 1000);
    horizontalScrollBar->setVisibleAmount(100);
    horizontalScrollBar->setValue(0);
    horizontalScrollBar->setChangeCallback([](double value) {
        std::cout << "Horizontal ScrollBar value: " << value << std::endl;
    });

    TextLabel* hScrollLabel = new TextLabel("Horizontal scroll demo:", 50, 490);

    PushButton* standaloneButton = new PushButton("Exit Application", 860, 530, 140, 40);
    standaloneButton->setClickCallback([]() {
        std::cout << "Exit button clicked!" << std::endl;
        exit(0);
    });

    ListBox* listBox = new ListBox(450, 320, 300, 150);
    listBox->setSelectionCallback([](int index, const std::string& item) {
        std::cout << "Selected item " << index << ": " << item << std::endl;
    });

    for (int i = 1; i <= 100; i++) {
        listBox->addItem("List Item " + std::to_string(i));
    }

    gui.add(topBar);
    gui.add(bottomBar);
    gui.add(imageWidget);
    gui.add(textBoxPanel);
    gui.add(panel2);
    gui.add(multiLineLabel);
    gui.add(multiLineTextBox);
    gui.add(showDialogButton);
    gui.add(standaloneMenu);
    gui.add(standaloneButton);
    gui.add(hScrollLabel);
    gui.add(horizontalScrollBar);
    gui.add(listBox);

    gui.addContextMenu(contextMenu);

    gui.run();

    return 0;
}
