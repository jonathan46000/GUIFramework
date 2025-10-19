CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I/usr/include/freetype2
LIBS = -lminifb -lX11 -lGL -lfreetype -lfontconfig -lpthread -lz

# Directory structure
BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/gui_app

# Object files with build directory prefix
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/GUIFramework.o $(BUILD_DIR)/Widget.o \
       $(BUILD_DIR)/MenuBar.o $(BUILD_DIR)/DropDownMenu.o $(BUILD_DIR)/MenuItem.o \
       $(BUILD_DIR)/FontRenderer.o $(BUILD_DIR)/TextBox.o $(BUILD_DIR)/PushButton.o \
       $(BUILD_DIR)/Panel.o $(BUILD_DIR)/TextLabel.o $(BUILD_DIR)/BooleanWidget.o \
       $(BUILD_DIR)/CheckBox.o $(BUILD_DIR)/RadioButton.o $(BUILD_DIR)/ScrollBar.o \
       $(BUILD_DIR)/ListBox.o $(BUILD_DIR)/ContextMenu.o $(BUILD_DIR)/CascadeMenu.o \
       $(BUILD_DIR)/DialogueBox.o $(BUILD_DIR)/FileDialog.o $(BUILD_DIR)/DialogManager.o \
       $(BUILD_DIR)/ImageLoader.o $(BUILD_DIR)/ImageWidget.o $(BUILD_DIR)/MultiLineTextBox.o

# Create directories and build target
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

# Pattern rule for building object files
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Create bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: clean
