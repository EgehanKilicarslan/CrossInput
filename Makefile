# CrossInput Makefile
# Simple makefile for building CrossInput library and tests

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./include -I./src

# Platform detection
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    PLATFORM = linux
    LDFLAGS = -lX11 -lXtst
    # Check for libei and gio for Wayland support via RemoteDesktop portal
    LIBEI_EXISTS := $(shell pkg-config --exists libei-1.0 2>/dev/null && echo yes)
    GIO_EXISTS := $(shell pkg-config --exists gio-unix-2.0 2>/dev/null && echo yes)
    ifeq ($(LIBEI_EXISTS),yes)
        ifeq ($(GIO_EXISTS),yes)
            CXXFLAGS += -DCROSSINPUT_HAS_LIBEI $(shell pkg-config --cflags libei-1.0 gio-unix-2.0)
            LDFLAGS += $(shell pkg-config --libs libei-1.0 gio-unix-2.0)
            $(info Wayland support enabled (libei + gio))
        else
            $(info libei found but gio-unix-2.0 missing - Wayland support disabled)
        endif
    else
        $(info libei not found - Wayland support disabled)
    endif
endif
ifeq ($(UNAME_S),Darwin)
    PLATFORM = macos
    LDFLAGS =
endif
ifeq ($(OS),Windows_NT)
    PLATFORM = windows
    LDFLAGS =
endif

# Directories
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build
INCLUDE_DIR = include

# Source files
LIB_SOURCES = $(SRC_DIR)/CrossInput.cpp
TEST_SOURCES = $(TEST_DIR)/test_crossinput.cpp

# Object files
LIB_OBJECTS = $(BUILD_DIR)/CrossInput.o
TEST_OBJECTS = $(BUILD_DIR)/test_crossinput.o

# Targets
LIB_TARGET = $(BUILD_DIR)/libCrossInput.a
TEST_TARGET = $(BUILD_DIR)/test_crossinput
INTERACTIVE_TARGET = $(BUILD_DIR)/test_interactive

.PHONY: all clean test lib interactive

all: lib test interactive

lib: $(LIB_TARGET)

test: $(TEST_TARGET)

interactive: $(INTERACTIVE_TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LIB_TARGET): $(LIB_OBJECTS) | $(BUILD_DIR)
	ar rcs $@ $^

$(BUILD_DIR)/CrossInput.o: $(SRC_DIR)/CrossInput.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_OBJECTS) $(LIB_TARGET) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -L$(BUILD_DIR) -lCrossInput $(LDFLAGS) -o $@

$(BUILD_DIR)/test_crossinput.o: $(TEST_DIR)/test_crossinput.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(INTERACTIVE_TARGET): $(BUILD_DIR)/test_interactive.o $(LIB_TARGET) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -L$(BUILD_DIR) -lCrossInput $(LDFLAGS) -o $@

$(BUILD_DIR)/test_interactive.o: $(TEST_DIR)/test_interactive.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

run_tests: test
	./$(TEST_TARGET)

run_interactive: interactive
	./$(INTERACTIVE_TARGET)

# Run interactive test in X11 mode (for Wayland systems)
run_interactive_x11: interactive
	@echo "Running in X11/XWayland mode..."
	env -u WAYLAND_DISPLAY XDG_SESSION_TYPE=x11 ./$(INTERACTIVE_TARGET)


# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: all

# Release build
release: CXXFLAGS += -O2 -DNDEBUG
release: all

# Installation directories
PREFIX ?= /usr/local
INCLUDEDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib

# Install library and headers
install: lib
	@echo "Installing CrossInput to $(PREFIX)..."
	install -d $(DESTDIR)$(INCLUDEDIR)/CrossInput
	install -d $(DESTDIR)$(LIBDIR)
	install -m 644 include/CrossInput.h $(DESTDIR)$(INCLUDEDIR)/CrossInput/
	install -m 644 $(LIBRARY) $(DESTDIR)$(LIBDIR)/
	@echo "Done! Use with: -I$(INCLUDEDIR) -L$(LIBDIR) -lCrossInput"

# Uninstall
uninstall:
	rm -f $(DESTDIR)$(INCLUDEDIR)/CrossInput/CrossInput.h
	rmdir $(DESTDIR)$(INCLUDEDIR)/CrossInput 2>/dev/null || true
	rm -f $(DESTDIR)$(LIBDIR)/libCrossInput.a
	@echo "CrossInput uninstalled"

help:
	@echo "CrossInput Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all                  - Build library, tests, and interactive tests (default)"
	@echo "  lib                  - Build the CrossInput library only"
	@echo "  test                 - Build the unit test executable"
	@echo "  interactive          - Build the interactive test executable"
	@echo "  run_tests            - Build and run unit tests"
	@echo "  run_interactive      - Build and run interactive tests"
	@echo "  run_interactive_x11  - Build and run interactive tests in X11 mode (for Wayland)"
	@echo "  install              - Install library to PREFIX (default: /usr/local)"
	@echo "  uninstall            - Remove installed files"
	@echo "  clean                - Remove build artifacts"
	@echo "  debug                - Build with debug symbols"
	@echo "  release              - Build with optimizations"
	@echo "  help                 - Show this help message"
	@echo ""
	@echo "Platform: $(PLATFORM)"

