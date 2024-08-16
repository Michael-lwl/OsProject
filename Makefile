# Makefile for project

# Directories
SRC_DIR := src
LIB_DIR := lib
BUILD_DIR := build
BIN_DIR := bin


# Filename
EXEC_NAME := os.sh


# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra

# Source files
SRC_CPP := $(wildcard $(SRC_DIR)/*.cpp)
SRC_MAIN := $(SRC_DIR)/main.cpp
SRC_LIB_CPP := $(shell find $(LIB_DIR) -name '*.cpp')
OBJ := $(SRC_CPP:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) $(SRC_LIB_CPP:$(LIB_DIR)/%.cpp=$(BUILD_DIR)/%.o)
OBJ_MAIN := $(SRC_MAIN:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Executable
EXEC := $(BIN_DIR)/$(EXEC_NAME)

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJ) $(OBJ_MAIN) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)/*
	make all
	./$(BIN_DIR)/$(EXEC_NAME)
