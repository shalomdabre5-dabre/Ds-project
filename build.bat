@echo off
rem ==============================================================================
rem Build script for Huffman Coding-Based File Compression System
rem Student Name : Shalom Dabre
rem Roll Number  : 11070
rem Course       : Computer Engineering — Data Structures and Algorithms
rem ==============================================================================

echo [BUILD] Compiling Huffman File Compression System...
gcc -Wall -Wextra -Wpedantic -std=c99 -O2 -Isrc -o huffman.exe src\main.c src\huffman.c src\min_heap.c src\file_handler.c src\bit_io.c src\crc32.c
if %errorlevel% neq 0 (
    echo [ERROR] Compilation failed!
    exit /b %errorlevel%
)
echo [SUCCESS] Built huffman.exe successfully.

echo [BUILD] Compiling Automated Test Runner...
gcc -Wall -Wextra -Wpedantic -std=c99 -O2 -Isrc -o tests\test_runner.exe tests\test_huffman.c src\huffman.c src\min_heap.c src\file_handler.c src\bit_io.c src\crc32.c
if %errorlevel% neq 0 (
    echo [ERROR] Test runner compilation failed!
    exit /b %errorlevel%
)
echo [SUCCESS] Built tests\test_runner.exe successfully.
echo.
echo Build complete! You can run:
echo   .\huffman.exe               (Interactive CLI menu)
echo   .\tests\test_runner.exe     (Run 25 automated tests)
echo   python gui\app.py           (Launch modern desktop GUI)
