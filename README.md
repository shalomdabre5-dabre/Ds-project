# Huffman Coding-Based File Compression System

A complete, production-grade, lossless file compression and decompression application implemented in Standard C (C99), paired with an interactive terminal interface, a modern Python Tkinter desktop GUI, and an automated test suite.

---

## 👨‍💻 Project Details

- **Student Name:** Shalom Dabre
- **Roll Number:** 11070
- **Course:** Computer Engineering — Data Structures and Algorithms (DSA)
- **Programming Language:** Standard C (C99) for core engine, Python (Tkinter) for GUI
- **Format:** Proprietary `HUF1` Binary Archive Container

---

## 📖 Introduction

Data compression is a fundamental area of Computer Science and Engineering. This mini-project implements a completely functional, lossless compression system based on **Huffman Coding**, an optimal prefix-free entropy encoding algorithm developed by David A. Huffman in 1952.

Unlike trivial academic demos that write ASCII `'0'` and `'1'` characters to text files (which actually increases file size by 800%), this system implements real bit-level packing: variable-length prefix codes are packed directly into 8-bit bytes (MSB to LSB). To ensure complete data integrity, files are protected by an IEEE 802.3 32-bit Cyclic Redundancy Check (CRC-32) that verifies 100% byte-for-byte exact recovery upon decompression.

---

## 🎯 Objectives & Problem Statement

Standard character encodings (such as ASCII) allocate a uniform 8 bits to every character, squandering storage capacity when character frequencies are skewed. This project addresses this inefficiency by:
1. Dynamically computing the frequency of every byte value in an input file.
2. Manually building a binary min-heap priority queue and a Huffman binary tree.
3. Generating optimal variable-length binary prefix codes.
4. Serializing bit-packed streams into self-contained binary archives (`HUF1`).
5. Providing full error handling, input validation, and byte-for-byte lossless recovery.

---

## ✨ Features

- **Zero External Compression Libraries:** All data structures (frequency tables, min-heaps, binary trees, code tables, bitstreams) are implemented from scratch in pure C.
- **Genuine Binary Bitstream Processing:** Custom `BitWriter` and `BitReader` stream individual bits packed MSB-to-LSB into physical bytes.
- **Self-Contained File Format (`HUF1`):** Stores magic signature, version, frequency metadata, original uncompressed size, and 32-bit CRC checksum.
- **Guaranteed Lossless Recovery:** Decompressed data is validated against the original CRC-32 checksum to immediately catch bit flips or file corruption.
- **Robust Edge-Case Handling:**
  - Empty files (0 bytes).
  - Single repeated characters (e.g., 5000 `'A'`s).
  - High-entropy and binary data (all 256 byte values $0x00$ to $0xFF$).
  - Malformed headers, corrupted magic signatures, and truncated archives.
- **Interactive Console Menu:** 7 menu options for compression, decompression, frequency analysis, code inspection, stats comparison, self-testing, and exit.
- **Command-Line Interface (CLI):** Headless execution switches (`-c`, `-d`, `-f`, `-codes`, `-test`, `--gui-info`, `--gui-compress`, `--gui-decompress`).
- **Modern Python Tkinter GUI:** Tabbed desktop interface featuring file pickers, metric cards, frequency tables, and an interactive Huffman tree visualizer.
- **Automated Test Suite:** 25 automated unit and regression tests verifying all 10 required system test scenarios.

---

## 💻 Requirements

### Software Requirements
- **Operating System:** Windows 10/11, Linux, or macOS.
- **C Compiler:** GCC (MinGW for Windows, or standard GCC/Clang on Linux/macOS) supporting C99 (`-std=c99`).
- **Python (Optional for GUI):** Python 3.8+ with standard `tkinter` installed.
- **Build Tools:** `make` (or `mingw32-make`), or the provided `build.bat` script.

### Hardware Requirements
- **Processor:** Any modern x86 / x64 / ARM processor.
- **Memory (RAM):** Under 50 MB (application runtime requires < 200 KB).
- **Disk Space:** 10 MB free space.

---

## 📂 Project Directory Structure

```text
c:\Ds mini project\
├── src\
│   ├── main.c              # Interactive CLI menu & command-line entry point
│   ├── huffman.c           # Huffman tree construction, DFS code generation
│   ├── huffman.h           # Tree node definitions & function prototypes
│   ├── min_heap.c          # Manual 0-indexed Min-Heap priority queue implementation
│   ├── min_heap.h          # Min-Heap structure & prototypes
│   ├── bit_io.c            # Bit-level streaming (BitWriter & BitReader)
│   ├── bit_io.h            # Bit I/O definitions
│   ├── file_handler.c      # HUF1 container serialization, CRC & stats engine
│   ├── file_handler.h      # File format structures & status codes
│   ├── crc32.c             # IEEE 802.3 CRC-32 checksum accumulator
│   └── crc32.h             # CRC-32 prototypes
├── tests\
│   ├── test_huffman.c      # Automated test harness (all 10 test scenarios)
│   ├── test_runner.exe     # Compiled automated test runner binary
│   └── test_cases\         # Sample test files generated for regression tests
├── gui\
│   └── app.py              # Modern Python Tkinter desktop graphical interface
├── sample_files\
│   ├── sample.txt          # Introductory DSA text sample (1,113 B)
│   ├── poem.txt            # Shakespeare Hamlet soliloquy (609 B)
│   ├── single_char.txt     # Single character edge case (1,000 'A's)
│   ├── all_bytes.bin       # Binary file with all 256 byte values (256 B)
│   └── empty.txt           # Empty file edge case (0 B)
├── output\                 # Directory for compressed and restored test files
├── docs\
│   ├── project_report.md   # Comprehensive academic engineering project report
│   ├── algorithm_explanation.md # Step-by-step worked example & dry run
│   └── viva_questions.md   # 30 detailed questions and answers for oral viva
├── Makefile                # Multi-platform build file
├── build.bat               # Windows batch build script
├── README.md               # Complete project documentation
└── .gitignore              # Git ignore rules for binaries and archives
```

---

## 🪟 Windows Compilation & Execution Guide (PowerShell & CMD)

This project has been verified on Windows using MinGW GCC and Python 3. Follow these exact commands in your Windows terminal (PowerShell or Command Prompt) from the `c:\Ds mini project` directory:

### Step 1: Verify Installed Tools
```powershell
gcc --version
python --version
```

### Step 2: Compile the Project
Choose any of the following three equivalent methods:

#### Method A: Using the One-Click Batch Script
```cmd
.\build.bat
```

#### Method B: Using GCC Directly
```powershell
# Compile the main compression engine (zero warnings with strict flags)
gcc -Wall -Wextra -Wpedantic -std=c99 -O2 -Isrc -o huffman.exe src/main.c src/huffman.c src/min_heap.c src/file_handler.c src/bit_io.c src/crc32.c

# Compile the automated test suite runner
gcc -Wall -Wextra -Wpedantic -std=c99 -O2 -Isrc -o tests/test_runner.exe tests/test_huffman.c src/huffman.c src/min_heap.c src/file_handler.c src/bit_io.c src/crc32.c
```

#### Method C: Using Make
```cmd
mingw32-make
```

---

### Step 3: Run the Application

#### Option A: Launch the Interactive Console Menu
```powershell
.\huffman.exe
```

#### Option B: Run Automated Self-Tests
```powershell
.\tests\test_runner.exe
```

#### Option C: Launch the Modern Tkinter Desktop GUI
```powershell
python gui\app.py
```

#### Option D: Launch the Modern Streamlit Web Application
```powershell
streamlit run gui\streamlit_app.py
```

---

### Step 4: Step-by-Step Compression, Decompression & Verification

To compress a file, decompress it, and verify that the restored file is 100% identical byte-for-byte to the original, run:

```powershell
# 1. Compress sample.txt to a binary HUF1 archive:
.\huffman.exe -c sample_files/sample.txt output/sample.huf

# 2. Decompress the archive back to a restored file:
.\huffman.exe -d output/sample.huf output/sample.restored

# 3. Perform a binary byte-by-byte comparison using Windows FC (File Compare):
fc.exe /b sample_files\sample.txt output\sample.restored
```

If the compression was completely lossless, `fc.exe` will output:
```text
FC: no differences encountered
```

---

## 🛠️ General Build Targets (Cross-Platform)

# Or on Linux:
make
```

### 3. Build Directly Using `gcc`
```bash
# Compile main application
gcc -Wall -Wextra -Wpedantic -std=c99 -O2 -Isrc -o huffman.exe src/main.c src/huffman.c src/min_heap.c src/file_handler.c src/bit_io.c src/crc32.c

# Compile test runner
gcc -Wall -Wextra -Wpedantic -std=c99 -O2 -Isrc -o tests/test_runner.exe tests/test_huffman.c src/huffman.c src/min_heap.c src/file_handler.c src/bit_io.c src/crc32.c
```

---

## 🚀 How to Run

### Option A: Interactive Console Menu
Run without arguments to launch the text-based user menu:
```cmd
.\huffman.exe
```

```text
==================================================================
        HUFFMAN CODING-BASED FILE COMPRESSION SYSTEM              
==================================================================
 Student Name : Shalom Dabre
 Roll Number  : 11070
 Course       : Computer Engineering - Data Structures and Algorithms
 Algorithm    : Lossless Huffman Prefix Coding (Min-Heap Priority Queue)
 Format       : Proprietary Binary Container (HUF1)
==================================================================

-------------------------- MAIN MENU -----------------------------
 1. Compress a file
 2. Decompress a previously compressed file
 3. Display character frequencies of a file
 4. Display Huffman codes & tree structure
 5. Display compression statistics (compare original & compressed)
 6. Run automated self-test suite
 7. Exit
------------------------------------------------------------------
Enter your choice (1-7):
```

---

### Option B: Command-Line Flags (Headless Mode)

```cmd
# 1. Compress a file
.\huffman.exe -c sample_files/sample.txt output/sample.huf

# 2. Decompress a file
.\huffman.exe -d output/sample.huf output/sample.restored

# 3. View character frequency table
.\huffman.exe -f sample_files/sample.txt

# 4. View generated Huffman prefix codes
.\huffman.exe -codes sample_files/sample.txt

# 5. Run built-in self-tests
.\huffman.exe -test
```

---

### Option C: Graphical User Interface (Python Tkinter & Streamlit)

You can launch either the native desktop interface or the modern web browser interface:

#### 1. Desktop Interface (Tkinter)
```powershell
python gui\app.py
```

#### 2. Interactive Web Application (Streamlit)
```powershell
streamlit run gui\streamlit_app.py
```

**Features of the Graphical Interfaces:**
1. **Compress & Analyze Tab:** Browse input and output files, view live metrics cards (Original Size, Compressed Size, Ratio, Space Saved, CRC), and inspect the character frequency table and charts.
2. **Decompress Tab:** Select `.huf` archives, restore original files, and view automatic CRC-32 integrity validation.
3. **Tree Visualizer Tab:** Interactive visualizer mapping Huffman codes from root to leaves.
4. **Automated Self-Test Tab:** Run all 25 unit and regression tests with live log output.

---

## 📊 Sample Execution & Output

### Compressing a Text File:
```text
C:\Ds mini project> .\huffman.exe -c sample_files/sample.txt output/sample.huf

=======================================================
               COMPRESSION STATISTICS                  
=======================================================
  Original File Size    : 1113 bytes
  Compressed File Size  : 736 bytes
  - Metadata/Header     : 389 bytes
  - Encoded Payload     : 347 bytes
-------------------------------------------------------
  Compression Ratio     : 1.512:1
  Space Saved           : 33.87%
  Original CRC-32       : 0x4B3A8B1A
  Restored CRC-32       : 0x4B3A8B1A
  Integrity Check       : MATCHED (100% Lossless Recovery)
=======================================================
```

### Binary Exact Verification (`fc /b`):
```text
C:\Ds mini project> .\huffman.exe -d output/sample.huf output/sample.restored
C:\Ds mini project> fc.exe /b sample_files\sample.txt output\sample.restored
Comparing files sample_files\sample.txt and output\sample.restored
FC: no differences encountered
```

---

## 🧪 Testing Requirements & Results

The system includes an automated test harness (`tests/test_runner.exe`) that executes **25 assertions** across all 10 required test scenarios:

| Test Scenario | Description | Status |
|:---|:---|:---:|
| **Scenario 1** | Empty input (0 bytes) round-trip | **PASSED** |
| **Scenario 2** | Single repeated character (2000 `'Z'`s) | **PASSED** |
| **Scenario 3** | Multiple repeated characters blocks | **PASSED** |
| **Scenario 4** | Prose containing spaces, punctuation, newlines, tabs | **PASSED** |
| **Scenario 5** | Binary file with all 256 byte values ($0x00$ to $0xFF$) | **PASSED** |
| **Scenario 6** | Larger text file (~50 KB) | **PASSED** |
| **Scenario 7** | Corrupted compressed file (invalid magic signature rejected) | **PASSED** |
| **Scenario 8** | Truncated compressed file (abruptly cut mid-stream rejected) | **PASSED** |
| **Scenario 9** | Non-existent or inaccessible input path | **PASSED** |
| **Scenario 10**| Full round-trip compression and decompression byte-by-byte match | **PASSED** |

To execute the test suite:
```cmd
.\tests\test_runner.exe
```

---

## ⏱️ Complexity Analysis

| Phase | Time Complexity | Auxiliary Space Complexity | Description |
|:---|:---:|:---:|:---|
| **Frequency Counting** | $O(N)$ | $O(1)$ | Single linear scan of $N$ bytes into a fixed 256-element array. |
| **Min-Heap Construction** | $O(k \log k)$ | $O(k)$ | Inserting $k$ unique characters into the heap ($k \le 256$). |
| **Huffman Tree Build** | $O(k \log k)$ | $O(k)$ | Merging $(k - 1)$ nodes in the priority queue. |
| **Code Generation** | $O(k)$ | $O(k)$ | DFS traversal of $(2k - 1)$ tree nodes. |
| **Bitstream Encoding** | $O(N)$ | $O(1)$ | Pass 2 over $N$ bytes with $O(1)$ code lookups. |
| **Decompression** | $O(N)$ | $O(k)$ | Stream traversal decoding 1 bit per tree branch. |

Because $k \le 256$ is bounded by the fixed size of a single byte, $k \log k \le 2048$ operations ($O(1)$).
Therefore, overall runtime is **strictly linear $O(N)$** and memory consumption is **bounded under 200 KB** regardless of file size.

---

## ⚠️ Limitations & Future Scope

### Limitations
1. **Integer Bit Discretization:** Huffman coding cannot allocate fractional bits. Symbols with probabilities $> 50\%$ still consume at least 1 full bit.
2. **Small File Expansion:** Files smaller than ~200 bytes may expand due to the 20-byte fixed header and frequency table metadata.
3. **Static Two-Pass Requirement:** The input file must be scanned once to build the frequency table before encoding can begin.

### Future Scope
1. **Adaptive / Dynamic Huffman Coding:** Implement the FGK or Vitter algorithm to encode in a single pass without storing frequency tables in headers.
2. **Canonical Huffman Encoding:** Store only code lengths in the header instead of explicit frequencies, reducing header size.
3. **Hybrid LZ77 + Huffman (DEFLATE):** Combine sliding-window dictionary matching with Huffman entropy coding (as used in GZIP/ZIP) for enhanced compression ratios.
