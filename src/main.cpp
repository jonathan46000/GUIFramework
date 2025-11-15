#include "GUIFramework.h"
#include <iostream>

int main() {
    GUIFramework gui("GUI Framework - Complete Widget Test", 1920, 1200);
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

    MenuBar* bottomBar = new MenuBar(0, 1175, 1920, 25, true);

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

    ScrollBar* horizontalScrollBar = new ScrollBar(680, 600, 250, ScrollBarOrientation::HORIZONTAL);
    horizontalScrollBar->setRange(0, 1000);
    horizontalScrollBar->setVisibleAmount(100);
    horizontalScrollBar->setValue(0);
    horizontalScrollBar->setChangeCallback([](double value) {
        std::cout << "Horizontal ScrollBar value: " << value << std::endl;
    });

    TextLabel* hScrollLabel = new TextLabel("Horizontal scroll demo:", 50, 430);

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

    // TabbedPanel example - MUCH LARGER
    TabbedPanel* tabbedPanel = new TabbedPanel(50, 490, 1820, 670);

    // Tab 1: Info - LOTS OF WIDGETS
    Panel* infoTab = new Panel(0, 0, 0, 0);
    infoTab->setDrawBorder(false);
    TextLabel* infoLabel = new TextLabel("Tab 1: Information Panel", 10, 10, 300, 25);
    infoTab->add(infoLabel);

    TextLabel* descLabel = new TextLabel("This tab demonstrates various label and text displays:", 10, 50, 500, 25);
    infoTab->add(descLabel);

    for (int i = 0; i < 20; i++) {
        TextLabel* itemLabel = new TextLabel("Information item " + std::to_string(i+1) + ": Sample text content here",
                                            20 + (i % 3) * 500, 90 + (i / 3) * 30, 450, 25);
        infoTab->add(itemLabel);
    }

    PushButton* infoBtn1 = new PushButton("Action Button 1", 20, 350, 150, 35);
    infoBtn1->setClickCallback([]() {
        std::cout << "Info Tab: Action Button 1 clicked" << std::endl;
    });
    infoTab->add(infoBtn1);

    PushButton* infoBtn2 = new PushButton("Action Button 2", 180, 350, 150, 35);
    infoBtn2->setClickCallback([]() {
        std::cout << "Info Tab: Action Button 2 clicked" << std::endl;
    });
    infoTab->add(infoBtn2);

    PushButton* infoBtn3 = new PushButton("Action Button 3", 340, 350, 150, 35);
    infoBtn3->setClickCallback([]() {
        std::cout << "Info Tab: Action Button 3 clicked" << std::endl;
    });
    infoTab->add(infoBtn3);

    tabbedPanel->addTab("Info", infoTab);

    // Tab 2: Settings - LOTS OF WIDGETS
    Panel* settingsTab = new Panel(0, 0, 0, 0);
    settingsTab->setDrawBorder(false);
    TextLabel* settingsLabel = new TextLabel("Tab 2: Settings & Configuration", 10, 10, 400, 25);
    settingsTab->add(settingsLabel);

    // Column 1: Checkboxes
    TextLabel* checkHeader = new TextLabel("General Options:", 20, 50, 300, 25);
    settingsTab->add(checkHeader);

    for (int i = 0; i < 10; i++) {
        CheckBox* check = new CheckBox("Option " + std::to_string(i+1), 30, 80 + i * 30);
        check->setChangeCallback([i](bool checked) {
            std::cout << "Settings Tab: Option " << (i+1) << " = " << (checked ? "ON" : "OFF") << std::endl;
        });
        settingsTab->add(check);
    }

    // Column 2: Radio buttons
    TextLabel* radioHeader = new TextLabel("Display Mode:", 400, 50, 300, 25);
    settingsTab->add(radioHeader);

    static std::vector<RadioButton*> displayGroup;
    displayGroup.clear();
    std::vector<std::string> displayModes = {"Mode A", "Mode B", "Mode C", "Mode D", "Mode E"};
    for (size_t i = 0; i < displayModes.size(); i++) {
        RadioButton* radio = new RadioButton(displayModes[i], 410, 80 + i * 30);
        radio->setChangeCallback([i](bool checked) {
            if (checked) std::cout << "Settings Tab: Display mode changed to " << (i+1) << std::endl;
        });
        displayGroup.push_back(radio);
        settingsTab->add(radio);
    }
    for (RadioButton* r : displayGroup) {
        r->setGroup(&displayGroup);
    }

    // Text inputs
    TextLabel* inputHeader = new TextLabel("Configuration Values:", 750, 50, 300, 25);
    settingsTab->add(inputHeader);

    for (int i = 0; i < 8; i++) {
        TextLabel* lbl = new TextLabel("Parameter " + std::to_string(i+1) + ":", 760, 85 + i * 50, 150, 25);
        settingsTab->add(lbl);
        TextBox* txt = new TextBox(920, 80 + i * 50, 250, 30);
        txt->setChangeCallback([i](const std::string& text) {
            std::cout << "Settings Tab: Parameter " << (i+1) << " = " << text << std::endl;
        });
        settingsTab->add(txt);
    }

    PushButton* applyBtn = new PushButton("Apply Settings", 20, 550, 180, 40);
    applyBtn->setClickCallback([]() {
        std::cout << "Settings Tab: Apply button clicked" << std::endl;
    });
    settingsTab->add(applyBtn);

    PushButton* resetBtn = new PushButton("Reset to Defaults", 220, 550, 180, 40);
    resetBtn->setClickCallback([]() {
        std::cout << "Settings Tab: Reset button clicked" << std::endl;
    });
    settingsTab->add(resetBtn);

    tabbedPanel->addTab("Settings", settingsTab);

    // Tab 3: Data - TESTING LISTBOX
    Panel* dataTab = new Panel(0, 0, 0, 0);
    dataTab->setDrawBorder(false);
    TextLabel* dataLabel = new TextLabel("Tab 3: Data Entry", 10, 10, 400, 25);
    dataTab->add(dataLabel);

    // Just text inputs
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 2; col++) {
            int idx = row * 2 + col + 1;
            TextLabel* lbl = new TextLabel("Field " + std::to_string(idx) + ":",
                                          20 + col * 350, 55 + row * 60, 100, 25);
            dataTab->add(lbl);
            TextBox* txt = new TextBox(130 + col * 350, 50 + row * 60, 200, 30);
            txt->setChangeCallback([idx](const std::string& text) {
                std::cout << "Data Tab: Field " << idx << " = " << text << std::endl;
            });
            dataTab->add(txt);
        }
    }

    // Test: Add ListBox
    TextLabel* listLabel = new TextLabel("Data Items:", 20, 250, 200, 25);
    dataTab->add(listLabel);

    ListBox* dataList = new ListBox(20, 280, 400, 180);
    for (int i = 1; i <= 50; i++) {
        dataList->addItem("Data Item " + std::to_string(i));
    }
    dataList->setSelectionCallback([](int index, const std::string& item) {
        std::cout << "Data Tab: Selected " << item << " at index " << index << std::endl;
    });
    dataTab->add(dataList);

    // Add MultiLineTextBox to test
    TextLabel* multiLabel = new TextLabel("Notes:", 450, 250, 200, 25);
    dataTab->add(multiLabel);

    MultiLineTextBox* dataMultiText = new MultiLineTextBox(450, 280, 500, 180);
    dataMultiText->setChangeCallback([](const std::string&) {
        std::cout << "Data Tab: MultiLine changed" << std::endl;
    });
    dataTab->add(dataMultiText);

    // Action buttons
    PushButton* saveBtn = new PushButton("Save Data", 20, 480, 150, 40);
    saveBtn->setClickCallback([]() {
        std::cout << "Data Tab: Save button clicked" << std::endl;
    });
    dataTab->add(saveBtn);

    PushButton* loadBtn = new PushButton("Load Data", 190, 480, 150, 40);
    loadBtn->setClickCallback([]() {
        std::cout << "Data Tab: Load button clicked" << std::endl;
    });
    dataTab->add(loadBtn);

    PushButton* clearBtn = new PushButton("Clear All", 360, 480, 150, 40);
    clearBtn->setClickCallback([]() {
        std::cout << "Data Tab: Clear button clicked" << std::endl;
    });
    dataTab->add(clearBtn);

    // Status indicators
    TextLabel* statusLabel = new TextLabel("Status:", 550, 480, 80, 25);
    dataTab->add(statusLabel);

    CheckBox* autoSave = new CheckBox("Auto-save", 640, 480);
    autoSave->setChangeCallback([](bool checked) {
        std::cout << "Data Tab: Auto-save = " << (checked ? "ON" : "OFF") << std::endl;
    });
    dataTab->add(autoSave);

    CheckBox* validateInput = new CheckBox("Validate", 640, 510);
    validateInput->setChangeCallback([](bool checked) {
        std::cout << "Data Tab: Validate = " << (checked ? "ON" : "OFF") << std::endl;
    });
    dataTab->add(validateInput);

    tabbedPanel->addTab("Data", dataTab);

    gui.add(tabbedPanel);

    gui.addContextMenu(contextMenu);

    gui.run();

    return 0;
}
