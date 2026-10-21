# ==============================================================================
# Makefile for Huffman Coding-Based File Compression System
# Student Name : Shalom Dabre
# Roll Number  : 11070
# Course       : Computer Engineering — Data Structures and Algorithms
# ==============================================================================

CC ?= gcc
CFLAGS ?= -Wall -Wextra -Wpedantic -std=c99 -O2 -Isrc
LDFLAGS ?=

ifeq ($(OS),Windows_NT)
    TARGET = huffman.exe
    TEST_TARGET = tests/test_runner.exe
    RM = del /Q /F
    MKDIR = if not exist output mkdir output
    FIXPATH = $(subst /,\,$1)
else
    TARGET = huffman
    TEST_TARGET = tests/test_runner
    RM = rm -rf
    MKDIR = mkdir -p output
    FIXPATH = $1
endif

# Source files
SRCS = src/main.c src/huffman.c src/min_heap.c src/file_handler.c src/bit_io.c src/crc32.c
TEST_SRCS = tests/test_huffman.c src/huffman.c src/min_heap.c src/file_handler.c src/bit_io.c src/crc32.c

.PHONY: all clean test run gui help

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(SRCS)
	@echo Compiling main application: $(TARGET)...
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

$(TEST_TARGET): $(TEST_SRCS)
	@echo Compiling automated test harness: $(TEST_TARGET)...
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRCS) $(LDFLAGS)

test: $(TEST_TARGET)
	@echo Running automated test suite...
	./$(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

gui: $(TARGET)
	python gui/app.py

clean:
ifeq ($(OS),Windows_NT)
	-@if exist huffman.exe $(RM) huffman.exe
	-@if exist tests\test_runner.exe $(RM) tests\test_runner.exe
	-@if exist output\* $(RM) output\*
else
	@$(RM) $(TARGET) $(TEST_TARGET) output/*
endif
	@echo Clean complete.

help:
	@echo Available Make targets:
	@echo   make          - Build main application and test runner
	@echo   make test     - Compile and run all automated test suites
	@echo   make run      - Run interactive console application
	@echo   make gui      - Launch modern desktop GUI
	@echo   make clean    - Remove compiled executables and temporary output files
