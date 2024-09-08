# Makefile for CMake-based build system

# Directories
BUILD_DIR := build
BIN_DIR := bin

# Project name
PROJECT_NAME := OsProject

# Detect the OS
ifeq ($(OS),Windows_NT)
    RM := del /Q
    MKDIR := if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
    EXE := .exe
else
    RM := rm -rf
    MKDIR := mkdir -p $(BUILD_DIR)
    EXE :=
endif

# Default target: Build and run the project
.PHONY: all
all: cmake-build
	@$(MAKE) -C $(BUILD_DIR) # let make run in the build directory

# Run the executable after the build
	@echo "Running the executable..."
	@./$(BIN_DIR)/$(PROJECT_NAME)$(EXE)

# CMake configuration and build
.PHONY: cmake-build
cmake-build:
	@echo "Starting cmake..."
	@$(MKDIR)
	@cd $(BUILD_DIR) && cmake ..
	@$(MAKE) -C $(BUILD_DIR)

# Clean build and binary directories
.PHONY: clean
clean:
	@echo "Cleaning up build and bin directories..."
	@$(RM) $(BUILD_DIR) $(BIN_DIR)
	@echo "Done."

# Rebuild everything from scratch
.PHONY: rebuild
rebuild: clean all
