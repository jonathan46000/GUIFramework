#include "GUIFramework.h"
#include <iostream>

int main() {
    GUIFramework gui("Splitter Widget Example", 1000, 700);

    if (!gui.loadSystemFont(12)) {
        std::cerr << "Failed to load system font" << std::endl;
        return 1;
    }

    Splitter* horizontalSplitter = new Splitter(10, 10, 980, 330, SplitterOrientation::HORIZONTAL);
    horizontalSplitter->setDividerPosition(490);
    horizontalSplitter->setMinPanelSize(100);

    Panel* leftPanel = horizontalSplitter->getLeftPanel();
    leftPanel->setBackgroundColor(0xFFE8F4F8);

    TextBox* textBox1 = new TextBox(10, 50, 440, 30);
    textBox1->setText("Type something here...");
    leftPanel->add(textBox1);

    TextBox* textBox2 = new TextBox(10, 90, 440, 30);
    textBox2->setText("Another text box");
    leftPanel->add(textBox2);

    PushButton* button1 = new PushButton("Clear Text", 10, 140, 150, 35);
    button1->setClickCallback([textBox1, textBox2]() {
        std::cout << "Clear Text button clicked!" << std::endl;
        textBox1->setText("");
        textBox2->setText("");
    });
    leftPanel->add(button1);

    CheckBox* checkbox1 = new CheckBox("Enable feature", 10, 190);
    checkbox1->setChangeCallback([](bool checked) {
        std::cout << "Checkbox 1 changed to: " << (checked ? "checked" : "unchecked") << std::endl;
    });
    leftPanel->add(checkbox1);

    Panel* rightPanel = horizontalSplitter->getRightPanel();
    rightPanel->setBackgroundColor(0xFFF8F4E8);

    ListBox* listBox = new ListBox(10, 50, 450, 250);
    listBox->addItem("Item 1 - Apple");
    listBox->addItem("Item 2 - Banana");
    listBox->addItem("Item 3 - Cherry");
    listBox->addItem("Item 4 - Date");
    listBox->addItem("Item 5 - Elderberry");
    listBox->addItem("Item 6 - Fig");
    listBox->addItem("Item 7 - Grape");
    listBox->addItem("Item 8 - Honeydew");
    listBox->setSelectionCallback([](int index, const std::string& item) {
        std::cout << "ListBox selection changed - Index: " << index << ", Item: " << item << std::endl;
    });
    rightPanel->add(listBox);

    gui.add(horizontalSplitter);

    Splitter* verticalSplitter = new Splitter(10, 350, 980, 340, SplitterOrientation::VERTICAL);
    verticalSplitter->setDividerPosition(170);
    verticalSplitter->setMinPanelSize(80);

    Panel* topPanel = verticalSplitter->getTopPanel();
    topPanel->setBackgroundColor(0xFFF0E8F8);

    PushButton* button2 = new PushButton("Button 1", 10, 50, 120, 35);
    button2->setClickCallback([]() {
        std::cout << "Button 1 clicked!" << std::endl;
    });
    topPanel->add(button2);

    PushButton* button3 = new PushButton("Button 2", 140, 50, 120, 35);
    button3->setClickCallback([]() {
        std::cout << "Button 2 clicked!" << std::endl;
    });
    topPanel->add(button3);

    PushButton* button4 = new PushButton("Button 3", 270, 50, 120, 35);
    button4->setClickCallback([]() {
        std::cout << "Button 3 clicked!" << std::endl;
    });
    topPanel->add(button4);

    CheckBox* checkbox2 = new CheckBox("Option A", 10, 100);
    checkbox2->setChangeCallback([](bool checked) {
        std::cout << "Checkbox 2 (Option A) changed to: " << (checked ? "checked" : "unchecked") << std::endl;
    });
    topPanel->add(checkbox2);

    CheckBox* checkbox3 = new CheckBox("Option B", 220, 100);
    checkbox3->setChangeCallback([](bool checked) {
        std::cout << "Checkbox 3 (Option B) changed to: " << (checked ? "checked" : "unchecked") << std::endl;
    });
    topPanel->add(checkbox3);

    Panel* bottomPanel = verticalSplitter->getBottomPanel();
    bottomPanel->setBackgroundColor(0xFFE8F8F0);

    gui.add(verticalSplitter);

    gui.run();

    return 0;
}
