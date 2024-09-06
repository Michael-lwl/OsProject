# Makefile for CMake-based build system

# Directories
BUILD_DIR := build
BIN_DIR := bin

# Project name
PROJECT_NAME := OsProject

# Default target: Build and run the project
.PHONY: all
all: cmake-build
	@$(MAKE) -C $(BUILD_DIR) # let make run in the build directory

# Run the executable after the build
	@echo "Running the executable..."
	@./$(BIN_DIR)/$(PROJECT_NAME)

# CMake configuration and build
.PHONY: cmake-build
cmake-build:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake ..
	@$(MAKE) -C $(BUILD_DIR)

# Clean build and binary directories
.PHONY: clean
clean:
	@echo "Cleaning up build and bin directories..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

# Rebuild everything from scratch
.PHONY: rebuild
rebuild: clean all
