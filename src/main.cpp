#include "GUIFramework.h"
#include <iostream>

int main() {
    GUIFramework gui("GUI Framework - Complete Widget Test", 1400, 900);
    DialogManager dialogManager(gui);

    if (!gui.loadSystemFont(12)) {
        return 1;
    }

    // ============================================================================
    // SECTION 1: OBJECT INSTANTIATION
    // ============================================================================

    // Menu bars
    MenuBar* topBar = new MenuBar(0, 0, 1024, 30, false);
    MenuBar* bottomBar = new MenuBar(0, 1175, 1920, 25, true);

    // File menu items
    DropDownMenu* fileMenu = new DropDownMenu("File", 60, 26);
    MenuItem* newItem = new MenuItem("New");
    MenuItem* openItem = new MenuItem("Open");
    CascadeMenuItem* saveItem = new CascadeMenuItem("Save");
    ContextMenu* saveSubmenu = new ContextMenu(150, 25);
    MenuItem* saveCurrentItem = new MenuItem("Save");
    MenuItem* saveAsItem = new MenuItem("Save As...");
    MenuItem* exitItem = new MenuItem("Exit");

    // Edit menu items
    DropDownMenu* editMenu = new DropDownMenu("Edit", 60, 26);
    MenuItem* cutMenuItem = new MenuItem("Cut");
    MenuItem* copyMenuItem = new MenuItem("Copy");
    MenuItem* pasteMenuItem = new MenuItem("Paste");

    // Context menu
    CascadeMenu* contextMenu = new CascadeMenu(180, 25);
    MenuItem* ctxCopy = new MenuItem("Copy");
    MenuItem* ctxPaste = new MenuItem("Paste");
    MenuItem* ctxCut = new MenuItem("Cut");
    MenuItem* ctxSelectAll = new MenuItem("Select All");
    MenuItem* ctxSeparator = new MenuItem("----------");
    CascadeMenuItem* ctxMoreOptions = new CascadeMenuItem("More Options");
    ContextMenu* moreSubmenu = new ContextMenu(150, 25);
    MenuItem* option1 = new MenuItem("Option 1");
    MenuItem* option2 = new MenuItem("Option 2");
    MenuItem* ctxProperties = new MenuItem("Properties");

    // Top-level widgets
    ImageWidget* imageWidget = new ImageWidget(50, 50, 200, 200, "../Resources/GUIFrameWork2.png");
    Panel* textBoxPanel = new Panel(50, 270, 350, 150);
    Panel* panel2 = new Panel(450, 50, 300, 250);
    TextLabel* multiLineLabel = new TextLabel("Multi-line text input:", 770, 50);
    MultiLineTextBox* multiLineTextBox = new MultiLineTextBox(770, 70, 240, 400);
    PushButton* showDialogButton = new PushButton("Show Dialog", 50, 450, 120, 30);
    DropDownMenu* standaloneMenu = new DropDownMenu("Standalone", 100, 30, 180, 450);
    MenuItem* testItem = new MenuItem("Test Item");
    ScrollBar* horizontalScrollBar = new ScrollBar(680, 600, 250, ScrollBarOrientation::HORIZONTAL);
    PushButton* standaloneButton = new PushButton("Exit Application", 860, 530, 140, 40);
    ListBox* listBox = new ListBox(450, 320, 300, 150);

    // TextBox panel widgets
    CascadeMenu* textBoxContextMenu = new CascadeMenu(150, 25);
    MenuItem* ctxCutText = new MenuItem("Cut Text");
    MenuItem* ctxCopyText = new MenuItem("Copy Text");
    MenuItem* ctxPasteText = new MenuItem("Paste Text");
    MenuItem* ctxSelectAllText = new MenuItem("Select All");
    TextLabel* label1 = new TextLabel("Enter your name:", 10, 10);
    TextBox* textBox1 = new TextBox(10, 30, 300, 30);
    TextLabel* label2 = new TextLabel("Enter your email:", 10, 70);
    TextBox* textBox2 = new TextBox(10, 90, 300, 30);
    TextLabel* textBoxPanelLabel = new TextLabel("(Supports Ctrl+C/X/V/A and right-click)", 10, 125);

    // Panel 2 widgets
    TextLabel* checkLabel = new TextLabel("Select options:", 10, 10);
    CheckBox* checkbox1 = new CheckBox("Enable notifications", 10, 30);
    CheckBox* checkbox2 = new CheckBox("Auto-save", 10, 55);
    CheckBox* checkbox3 = new CheckBox("Dark mode", 10, 80);
    TextLabel* radioLabel = new TextLabel("Choose size:", 10, 110);
    RadioButton* radio1 = new RadioButton("Small", 10, 130);
    RadioButton* radio2 = new RadioButton("Medium", 10, 155);
    RadioButton* radio3 = new RadioButton("Large", 10, 180);
    std::vector<RadioButton*> sizeGroup = {radio1, radio2, radio3};

    // TabbedPanel
    TabbedPanel* tabbedPanel = new TabbedPanel(50, 490, 1300, 380);

    // Info tab widgets
    Panel* infoTab = new Panel(0, 0, 0, 0);
    TextLabel* infoLabel = new TextLabel("Tab 1: Information Panel", 10, 10, 300, 25);
    TextLabel* descLabel = new TextLabel("This tab demonstrates labels and buttons:", 10, 50, 500, 25);
    TextLabel* infoItem1 = new TextLabel("Information item 1: Sample text content", 20, 90, 450, 25);
    TextLabel* infoItem2 = new TextLabel("Information item 2: Sample text content", 20, 120, 450, 25);
    TextLabel* infoItem3 = new TextLabel("Information item 3: Sample text content", 20, 150, 450, 25);
    TextLabel* infoItem4 = new TextLabel("Information item 4: Sample text content", 20, 180, 450, 25);
    TextLabel* infoItem5 = new TextLabel("Information item 5: Sample text content", 20, 210, 450, 25);
    PushButton* infoBtn1 = new PushButton("Action Button", 20, 250, 150, 35);

    // Settings tab widgets
    Panel* settingsTab = new Panel(0, 0, 0, 0);
    TextLabel* settingsLabel = new TextLabel("Tab 2: Settings & Configuration", 10, 10, 400, 25);
    TextLabel* checkHeader = new TextLabel("General Options:", 20, 50, 300, 25);
    CheckBox* settingsCheck1 = new CheckBox("Option 1", 30, 80);
    CheckBox* settingsCheck2 = new CheckBox("Option 2", 30, 110);
    CheckBox* settingsCheck3 = new CheckBox("Option 3", 30, 140);
    TextLabel* radioHeader = new TextLabel("Display Mode:", 250, 50, 300, 25);
    RadioButton* settingsRadio1 = new RadioButton("Mode A", 260, 80);
    RadioButton* settingsRadio2 = new RadioButton("Mode B", 260, 110);
    RadioButton* settingsRadio3 = new RadioButton("Mode C", 260, 140);
    static std::vector<RadioButton*> displayGroup = {settingsRadio1, settingsRadio2, settingsRadio3};
    TextLabel* inputHeader = new TextLabel("Configuration:", 500, 50, 300, 25);
    TextLabel* paramLabel1 = new TextLabel("Param 1:", 510, 85, 100, 25);
    TextBox* paramText1 = new TextBox(620, 80, 200, 30);
    TextLabel* paramLabel2 = new TextLabel("Param 2:", 510, 125, 100, 25);
    TextBox* paramText2 = new TextBox(620, 120, 200, 30);
    TextLabel* paramLabel3 = new TextLabel("Param 3:", 510, 165, 100, 25);
    TextBox* paramText3 = new TextBox(620, 160, 200, 30);
    TextLabel* comboLabel = new TextLabel("Theme:", 510, 215, 100, 25);
    ComboBox* themeCombo = new ComboBox(620, 210, 200, 30);
    PushButton* applyBtn = new PushButton("Apply Settings", 20, 300, 180, 40);

    // Data tab widgets
    Panel* dataTab = new Panel(0, 0, 0, 0);
    TextLabel* dataLabel = new TextLabel("Tab 3: Data Entry", 10, 10, 400, 25);
    TextLabel* dataFieldLabel1 = new TextLabel("Field 1:", 20, 55, 100, 25);
    TextBox* dataField1 = new TextBox(130, 50, 200, 30);
    TextLabel* dataFieldLabel2 = new TextLabel("Field 2:", 20, 95, 100, 25);
    TextBox* dataField2 = new TextBox(130, 90, 200, 30);
    TextLabel* listLabel = new TextLabel("Items:", 20, 150, 200, 25);
    ListBox* dataList = new ListBox(20, 180, 300, 120);
    TextLabel* multiLabel = new TextLabel("Notes:", 350, 150, 200, 25);
    MultiLineTextBox* dataMultiText = new MultiLineTextBox(350, 180, 400, 120);
    PushButton* saveBtn = new PushButton("Save", 20, 320, 100, 30);
    CheckBox* autoSave = new CheckBox("Auto-save", 150, 320);

    // ============================================================================
    // SECTION 2: WIDGET SETUP & CONFIGURATION
    // ============================================================================

    // Image widget check
    if (!imageWidget->hasImage()) {
        std::cerr << "Failed to load image!" << std::endl;
    }

    // Radio button groups
    for (RadioButton* r : sizeGroup) {
        r->setGroup(&sizeGroup);
    }
    for (RadioButton* r : displayGroup) {
        r->setGroup(&displayGroup);
    }

    // Horizontal scrollbar config
    horizontalScrollBar->setRange(0, 1000);
    horizontalScrollBar->setVisibleAmount(100);
    horizontalScrollBar->setValue(0);

    // ListBox items
    for (int i = 1; i <= 100; i++) {
        listBox->addItem("List Item " + std::to_string(i));
    }

    // Data tab ListBox items
    for (int i = 1; i <= 20; i++) {
        dataList->addItem("Data Item " + std::to_string(i));
    }

    // ComboBox items
    themeCombo->addItem("Default");
    themeCombo->addItem("Dark");
    themeCombo->addItem("Light");
    themeCombo->addItem("High Contrast");
    themeCombo->addItem("Solarized");
    themeCombo->setSelectedIndex(0);

    // Panel settings
    textBoxPanel->setContextMenu(contextMenu);
    infoTab->setDrawBorder(false);
    settingsTab->setDrawBorder(false);
    dataTab->setDrawBorder(false);

    // ============================================================================
    // SECTION 3: CALLBACK SETUP
    // ============================================================================

    // File menu callbacks
    newItem->setCallback([]() {
        std::cout << "New file clicked!" << std::endl;
    });
    openItem->setCallback([&dialogManager]() {
        std::cout << "Open file clicked!" << std::endl;
        dialogManager.showOpenFileDialog();
    });
    saveCurrentItem->setCallback([]() {
        std::cout << "Save current file!" << std::endl;
    });
    saveAsItem->setCallback([&dialogManager]() {
        std::cout << "Save As clicked!" << std::endl;
        dialogManager.showSaveFileDialog();
    });
    exitItem->setCallback([]() {
        std::cout << "Exit clicked!" << std::endl;
        exit(0);
    });

    // Edit menu callbacks
    cutMenuItem->setCallback([&gui]() {
        gui.cutFromTextBox();
        std::cout << "Cut from Edit menu" << std::endl;
    });
    copyMenuItem->setCallback([&gui]() {
        gui.copyFromTextBox();
        std::cout << "Copy from Edit menu" << std::endl;
    });
    pasteMenuItem->setCallback([&gui]() {
        gui.pasteToTextBox();
        std::cout << "Paste from Edit menu" << std::endl;
    });

    // Context menu callbacks
    ctxCopy->setCallback([&gui]() {
        gui.copyFromTextBox();
        std::cout << "Context menu: Copy" << std::endl;
    });
    ctxPaste->setCallback([&gui]() {
        gui.pasteToTextBox();
        std::cout << "Context menu: Paste" << std::endl;
    });
    ctxCut->setCallback([&gui]() {
        gui.cutFromTextBox();
        std::cout << "Context menu: Cut" << std::endl;
    });

    // TextBox callbacks
    textBox1->setChangeCallback([](const std::string& text) {
        std::cout << "TextBox 1: " << text << std::endl;
    });
    textBox2->setChangeCallback([](const std::string& text) {
        std::cout << "TextBox 2: " << text << std::endl;
    });

    // Panel 2 checkbox callbacks
    checkbox1->setChangeCallback([](bool checked) {
        std::cout << "Notifications: " << (checked ? "ON" : "OFF") << std::endl;
    });
    checkbox2->setChangeCallback([](bool checked) {
        std::cout << "Auto-save: " << (checked ? "ON" : "OFF") << std::endl;
    });
    checkbox3->setChangeCallback([](bool checked) {
        std::cout << "Dark mode: " << (checked ? "ON" : "OFF") << std::endl;
    });

    // Radio button callbacks
    for (RadioButton* r : sizeGroup) {
        r->setChangeCallback([](bool checked) {
            if (checked) std::cout << "Size option changed" << std::endl;
        });
    }

    // MultiLine textbox callback
    multiLineTextBox->setChangeCallback([](const std::string& text) {
        std::cout << "MultiLine changed (length=" << text.length() << ")" << std::endl;
    });

    // Button callbacks
    showDialogButton->setClickCallback([&dialogManager]() {
        std::cout << "Showing dialog!" << std::endl;
        dialogManager.showTestDialog();
    });
    standaloneButton->setClickCallback([]() {
        std::cout << "Exit button clicked!" << std::endl;
        exit(0);
    });

    // Standalone menu callback
    testItem->setCallback([]() {
        std::cout << "Standalone menu item clicked!" << std::endl;
    });

    // Scrollbar callback
    horizontalScrollBar->setChangeCallback([](double value) {
        std::cout << "Horizontal ScrollBar value: " << value << std::endl;
    });

    // ListBox callback
    listBox->setSelectionCallback([](int index, const std::string& item) {
        std::cout << "Selected item " << index << ": " << item << std::endl;
    });

    // Info tab button callback
    infoBtn1->setClickCallback([]() {
        std::cout << "Info Tab: Action Button clicked" << std::endl;
    });

    // Settings tab checkbox callbacks
    settingsCheck1->setChangeCallback([](bool checked) {
        std::cout << "Settings Tab: Option 1 = " << (checked ? "ON" : "OFF") << std::endl;
    });
    settingsCheck2->setChangeCallback([](bool checked) {
        std::cout << "Settings Tab: Option 2 = " << (checked ? "ON" : "OFF") << std::endl;
    });
    settingsCheck3->setChangeCallback([](bool checked) {
        std::cout << "Settings Tab: Option 3 = " << (checked ? "ON" : "OFF") << std::endl;
    });

    // Settings tab radio callbacks
    settingsRadio1->setChangeCallback([](bool checked) {
        if (checked) std::cout << "Settings Tab: Display mode A" << std::endl;
    });
    settingsRadio2->setChangeCallback([](bool checked) {
        if (checked) std::cout << "Settings Tab: Display mode B" << std::endl;
    });
    settingsRadio3->setChangeCallback([](bool checked) {
        if (checked) std::cout << "Settings Tab: Display mode C" << std::endl;
    });

    // Settings tab textbox callbacks
    paramText1->setChangeCallback([](const std::string& text) {
        std::cout << "Settings Tab: Parameter 1 = " << text << std::endl;
    });
    paramText2->setChangeCallback([](const std::string& text) {
        std::cout << "Settings Tab: Parameter 2 = " << text << std::endl;
    });
    paramText3->setChangeCallback([](const std::string& text) {
        std::cout << "Settings Tab: Parameter 3 = " << text << std::endl;
    });

    // ComboBox callbacks
    themeCombo->setSelectionCallback([](int index, const std::string& item) {
        std::cout << "Settings Tab: Theme selected: " << item << " (index " << index << ")" << std::endl;
    });
    themeCombo->setChangeCallback([](const std::string& text) {
        std::cout << "Settings Tab: Theme text changed: " << text << std::endl;
    });

    // Apply button callback
    applyBtn->setClickCallback([]() {
        std::cout << "Settings Tab: Apply button clicked" << std::endl;
    });

    // Data tab textbox callbacks
    dataField1->setChangeCallback([](const std::string& text) {
        std::cout << "Data Tab: Field 1 = " << text << std::endl;
    });
    dataField2->setChangeCallback([](const std::string& text) {
        std::cout << "Data Tab: Field 2 = " << text << std::endl;
    });

    // Data tab ListBox callback
    dataList->setSelectionCallback([](int index, const std::string& item) {
        std::cout << "Data Tab: Selected " << item << " at index " << index << std::endl;
    });

    // Data tab MultiLineTextBox callback
    dataMultiText->setChangeCallback([](const std::string&) {
        std::cout << "Data Tab: MultiLine changed" << std::endl;
    });

    // Data tab button callback
    saveBtn->setClickCallback([]() {
        std::cout << "Data Tab: Save clicked" << std::endl;
    });

    // Data tab checkbox callback
    autoSave->setChangeCallback([](bool checked) {
        std::cout << "Data Tab: Auto-save = " << (checked ? "ON" : "OFF") << std::endl;
    });

    // TextBox context menu callbacks
    ctxCutText->setCallback([&gui]() {
        gui.cutFromTextBox();
        std::cout << "TextBox context menu: Cut Text" << std::endl;
    });
    ctxCopyText->setCallback([&gui]() {
        gui.copyFromTextBox();
        std::cout << "TextBox context menu: Copy Text" << std::endl;
    });
    ctxPasteText->setCallback([&gui]() {
        gui.pasteToTextBox();
        std::cout << "TextBox context menu: Paste Text" << std::endl;
    });
    ctxSelectAllText->setCallback([&gui]() {
        gui.selectAllInTextBox();
        std::cout << "TextBox context menu: Select All" << std::endl;
    });

    // ============================================================================
    // SECTION 4: CONTAINER ASSEMBLY
    // ============================================================================

    // Build File menu
    fileMenu->add(newItem);
    fileMenu->add(openItem);
    saveSubmenu->add(saveCurrentItem);
    saveSubmenu->add(saveAsItem);
    saveItem->setSubmenu(saveSubmenu);
    fileMenu->add(saveItem);
    fileMenu->add(exitItem);

    // Build Edit menu
    editMenu->add(cutMenuItem);
    editMenu->add(copyMenuItem);
    editMenu->add(pasteMenuItem);

    // Build menu bar
    topBar->add(fileMenu);
    topBar->add(editMenu);

    // Build context menu
    contextMenu->add(ctxCopy);
    contextMenu->add(ctxPaste);
    contextMenu->add(ctxCut);
    contextMenu->add(ctxSelectAll);
    contextMenu->add(ctxSeparator);
    moreSubmenu->add(option1);
    moreSubmenu->add(option2);
    ctxMoreOptions->setSubmenu(moreSubmenu);
    contextMenu->add(ctxMoreOptions);
    contextMenu->add(ctxProperties);

    // Build textBox context menu
    textBoxContextMenu->add(ctxCutText);
    textBoxContextMenu->add(ctxCopyText);
    textBoxContextMenu->add(ctxPasteText);
    textBoxContextMenu->add(ctxSelectAllText);

    // Build standalone menu
    standaloneMenu->add(testItem);

    // Build TextBox panel
    textBoxPanel->add(label1);
    textBoxPanel->add(textBox1);
    textBoxPanel->add(label2);
    textBoxPanel->add(textBox2);
    textBoxPanel->add(textBoxPanelLabel);
    textBoxPanel->setContextMenu(textBoxContextMenu);

    // Build Panel 2
    panel2->add(checkLabel);
    panel2->add(checkbox1);
    panel2->add(checkbox2);
    panel2->add(checkbox3);
    panel2->add(radioLabel);
    panel2->add(radio1);
    panel2->add(radio2);
    panel2->add(radio3);

    // Build Info tab
    infoTab->add(infoLabel);
    infoTab->add(descLabel);
    infoTab->add(infoItem1);
    infoTab->add(infoItem2);
    infoTab->add(infoItem3);
    infoTab->add(infoItem4);
    infoTab->add(infoItem5);
    infoTab->add(infoBtn1);

    // Build Settings tab
    settingsTab->add(settingsLabel);
    settingsTab->add(checkHeader);
    settingsTab->add(settingsCheck1);
    settingsTab->add(settingsCheck2);
    settingsTab->add(settingsCheck3);
    settingsTab->add(radioHeader);
    settingsTab->add(settingsRadio1);
    settingsTab->add(settingsRadio2);
    settingsTab->add(settingsRadio3);
    settingsTab->add(inputHeader);
    settingsTab->add(paramLabel1);
    settingsTab->add(paramText1);
    settingsTab->add(paramLabel2);
    settingsTab->add(paramText2);
    settingsTab->add(paramLabel3);
    settingsTab->add(paramText3);
    settingsTab->add(comboLabel);
    settingsTab->add(themeCombo);
    settingsTab->add(applyBtn);

    // Build Data tab
    dataTab->add(dataLabel);
    dataTab->add(dataFieldLabel1);
    dataTab->add(dataField1);
    dataTab->add(dataFieldLabel2);
    dataTab->add(dataField2);
    dataTab->add(listLabel);
    dataTab->add(dataList);
    dataTab->add(multiLabel);
    dataTab->add(dataMultiText);
    dataTab->add(saveBtn);
    dataTab->add(autoSave);

    // Build TabbedPanel
    tabbedPanel->addTab("Info", infoTab);
    tabbedPanel->addTab("Settings", settingsTab);
    tabbedPanel->addTab("Data", dataTab);

    // ============================================================================
    // SECTION 5: GUI ASSEMBLY
    // ============================================================================

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
    gui.add(horizontalScrollBar);
    gui.add(listBox);
    gui.add(tabbedPanel);

    gui.addContextMenu(contextMenu);

    gui.run();

    return 0;
}
