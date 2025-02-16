# Compiler and compiler flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Directories
SRC_DIR = ./src
HEADERS_DIR = ./headers
FONT_DIR = ./font
BUILD_DIR = ./build
GTEST_DIR = ./googletest
GTEST_BUILD_DIR = $(GTEST_DIR)/build
GTEST_LIB_DIR = $(GTEST_BUILD_DIR)/lib

# Source and object files
SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/print.cpp $(SRC_DIR)/orderbook.cpp
TEST_SRCS = $(SRC_DIR)/test.cpp $(SRC_DIR)/orderbook.cpp
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
TEST_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(TEST_SRCS))

# Output executables
TARGET = main
TEST_TARGET = test

# Default target
all: $(TARGET) test

# Rule to build the main executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to build Google Test
$(GTEST_LIB_DIR)/libgtest.a:
	@mkdir -p $(GTEST_BUILD_DIR)
	cd $(GTEST_BUILD_DIR) && cmake .. && make

# Rule to build the test executable
test: $(TEST_OBJS) $(GTEST_LIB_DIR)/libgtest.a $(GTEST_LIB_DIR)/libgtest_main.a
	$(CXX) $(CXXFLAGS) -I$(GTEST_DIR)/include -pthread -o $(BUILD_DIR)/$(TEST_TARGET) $(TEST_OBJS) $(GTEST_LIB_DIR)/libgtest.a $(GTEST_LIB_DIR)/libgtest_main.a

# Rule to compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(BUILD_DIR)/$(TEST_TARGET) $(GTEST_BUILD_DIR)

# Run the main program
run: $(TARGET)
	./$(TARGET)

# Run the tests
run_tests: test
	./$(BUILD_DIR)/$(TEST_TARGET)

# Phony targets
.PHONY: all clean run run_tests
