CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I/usr/include/freetype2 -Isources
LIBS = -lminifb -lX11 -lGL -lfreetype -lfontconfig -lpthread -lz

# Directory structure
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/gui_app

# Source files
SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/GUIFramework.cpp $(SRC_DIR)/Widget.cpp \
       $(SRC_DIR)/MenuBar.cpp $(SRC_DIR)/DropDownMenu.cpp $(SRC_DIR)/MenuItem.cpp \
       $(SRC_DIR)/FontRenderer.cpp $(SRC_DIR)/TextBox.cpp $(SRC_DIR)/PushButton.cpp \
       $(SRC_DIR)/Panel.cpp $(SRC_DIR)/TextLabel.cpp $(SRC_DIR)/BooleanWidget.cpp \
       $(SRC_DIR)/CheckBox.cpp $(SRC_DIR)/RadioButton.cpp $(SRC_DIR)/ScrollBar.cpp \
       $(SRC_DIR)/ListBox.cpp $(SRC_DIR)/ContextMenu.cpp $(SRC_DIR)/CascadeMenu.cpp \
       $(SRC_DIR)/DialogueBox.cpp $(SRC_DIR)/FileDialog.cpp $(SRC_DIR)/DialogManager.cpp \
       $(SRC_DIR)/ImageLoader.cpp $(SRC_DIR)/ImageWidget.cpp $(SRC_DIR)/MultiLineTextBox.cpp \
       $(SRC_DIR)/TabbedPanel.cpp $(SRC_DIR)/ComboBox.cpp $(SRC_DIR)/StatusBar.cpp \
       $(SRC_DIR)/ProgressBar.cpp

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Create directories and build target
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

# Pattern rule for building object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
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
