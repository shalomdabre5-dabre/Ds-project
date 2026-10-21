/**
 * ============================================================================
 * Project: Huffman Coding-Based File Compression System
 * Course:  Computer Engineering — Data Structures and Algorithms
 * Student: Shalom Dabre
 * Roll No: 11070
 * 
 * File:    src/main.c
 * Purpose: Interactive menu-driven console application and CLI entry point.
 * ============================================================================
 */

#include "huffman.h"
#include "file_handler.h"
#include "crc32.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LEN 1024

/* Function prototypes */
static void print_banner(void);
static void print_menu(void);
static void clean_newline(char *str);
static int prompt_yes_no(const char *prompt);
static void menu_compress(void);
static void menu_decompress(void);
static void menu_frequencies(void);
static void menu_huffman_codes(void);
static void menu_statistics(void);
static void run_self_test_suite(void);

/* Helper to strip trailing newline/CR characters */
static void clean_newline(char *str) {
    if (!str) return;
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

static int prompt_yes_no(const char *prompt) {
    char buf[16];
    printf("%s [y/N]: ", prompt);
    if (!fgets(buf, sizeof(buf), stdin)) {
        return 0;
    }
    return (buf[0] == 'y' || buf[0] == 'Y');
}

static void print_banner(void) {
    printf("==================================================================\n");
    printf("        HUFFMAN CODING-BASED FILE COMPRESSION SYSTEM              \n");
    printf("==================================================================\n");
    printf(" Student Name : Shalom Dabre\n");
    printf(" Roll Number  : 11070\n");
    printf(" Course       : Computer Engineering - Data Structures and Algorithms\n");
    printf(" Algorithm    : Lossless Huffman Prefix Coding (Min-Heap Priority Queue)\n");
    printf(" Format       : Proprietary Binary Container (HUF1)\n");
    printf("==================================================================\n\n");
}

static void print_menu(void) {
    printf("-------------------------- MAIN MENU -----------------------------\n");
    printf(" 1. Compress a file\n");
    printf(" 2. Decompress a previously compressed file\n");
    printf(" 3. Display character frequencies of a file\n");
    printf(" 4. Display Huffman codes & tree structure\n");
    printf(" 5. Display compression statistics (compare original & compressed)\n");
    printf(" 6. Run automated self-test suite\n");
    printf(" 7. Exit\n");
    printf("------------------------------------------------------------------\n");
    printf("Enter your choice (1-7): ");
}

static void menu_compress(void) {
    char in_path[MAX_PATH_LEN];
    char out_path[MAX_PATH_LEN];

    printf("\n[COMPRESS FILE]\n");
    printf("Enter input file path: ");
    if (!fgets(in_path, sizeof(in_path), stdin)) return;
    clean_newline(in_path);

    if (!file_exists(in_path)) {
        printf("[ERROR] Input file does not exist: %s\n\n", in_path);
        return;
    }

    printf("Enter output compressed file path (or press Enter for '%s.huf'): ", in_path);
    if (!fgets(out_path, sizeof(out_path), stdin)) return;
    clean_newline(out_path);

    if (strlen(out_path) == 0) {
        snprintf(out_path, sizeof(out_path), "%s.huf", in_path);
    }

    int allow_overwrite = 1;
    if (file_exists(out_path)) {
        printf("[WARNING] Target file '%s' already exists.\n", out_path);
        if (!prompt_yes_no("Do you want to overwrite it?")) {
            printf("[INFO] Compression cancelled.\n\n");
            return;
        }
    }

    printf("[INFO] Compressing '%s' -> '%s'...\n", in_path, out_path);
    CompressionStats stats;
    HuffmanStatus status = compress_file(in_path, out_path, &stats, allow_overwrite);

    if (status == HUFF_SUCCESS) {
        printf("[SUCCESS] Compression finished successfully!\n");
        display_compression_stats(&stats);
    } else {
        printf("[ERROR] Compression failed: %s\n\n", huffman_status_string(status));
    }
}

static void menu_decompress(void) {
    char in_path[MAX_PATH_LEN];
    char out_path[MAX_PATH_LEN];

    printf("\n[DECOMPRESS FILE]\n");
    printf("Enter compressed file path (.huf): ");
    if (!fgets(in_path, sizeof(in_path), stdin)) return;
    clean_newline(in_path);

    if (!file_exists(in_path)) {
        printf("[ERROR] Compressed file does not exist: %s\n\n", in_path);
        return;
    }

    printf("Enter output restored file path: ");
    if (!fgets(out_path, sizeof(out_path), stdin)) return;
    clean_newline(out_path);

    if (strlen(out_path) == 0) {
        printf("[ERROR] Output file path cannot be empty.\n\n");
        return;
    }

    int allow_overwrite = 1;
    if (file_exists(out_path)) {
        printf("[WARNING] Target file '%s' already exists.\n", out_path);
        if (!prompt_yes_no("Do you want to overwrite it?")) {
            printf("[INFO] Decompression cancelled.\n\n");
            return;
        }
    }

    printf("[INFO] Decompressing '%s' -> '%s'...\n", in_path, out_path);
    CompressionStats stats;
    HuffmanStatus status = decompress_file(in_path, out_path, &stats, allow_overwrite);

    if (status == HUFF_SUCCESS) {
        printf("[SUCCESS] Decompression finished successfully!\n");
        display_compression_stats(&stats);
    } else {
        printf("[ERROR] Decompression failed: %s\n\n", huffman_status_string(status));
    }
}

static void menu_frequencies(void) {
    char in_path[MAX_PATH_LEN];

    printf("\n[DISPLAY CHARACTER FREQUENCIES]\n");
    printf("Enter file path: ");
    if (!fgets(in_path, sizeof(in_path), stdin)) return;
    clean_newline(in_path);

    if (!file_exists(in_path)) {
        printf("[ERROR] File does not exist: %s\n\n", in_path);
        return;
    }

    uint64_t frequencies[256];
    uint64_t file_size = 0;
    HuffmanStatus status = analyze_file_frequencies(in_path, frequencies, &file_size);

    if (status == HUFF_SUCCESS) {
        display_frequency_table(frequencies, file_size);
    } else {
        printf("[ERROR] Failed to analyze file: %s\n\n", huffman_status_string(status));
    }
}

static void menu_huffman_codes(void) {
    char in_path[MAX_PATH_LEN];

    printf("\n[DISPLAY HUFFMAN CODES & TREE]\n");
    printf("Enter file path: ");
    if (!fgets(in_path, sizeof(in_path), stdin)) return;
    clean_newline(in_path);

    if (!file_exists(in_path)) {
        printf("[ERROR] File does not exist: %s\n\n", in_path);
        return;
    }

    uint64_t frequencies[256];
    uint64_t file_size = 0;
    HuffmanStatus status = analyze_file_frequencies(in_path, frequencies, &file_size);

    if (status != HUFF_SUCCESS) {
        printf("[ERROR] Failed to analyze file: %s\n\n", huffman_status_string(status));
        return;
    }

    HuffmanNode *root = build_huffman_tree(frequencies);
    if (!root && file_size > 0) {
        printf("[ERROR] Failed to construct Huffman tree.\n\n");
        return;
    }

    HuffmanCodeTable table;
    generate_huffman_codes(root, &table);

    display_huffman_codes(&table, frequencies);

    if (prompt_yes_no("Would you like to view the ASCII Huffman Tree structure?")) {
        printf("\n--- Huffman Tree Hierarchy ---\n");
        if (root) {
            print_tree_structure(root, 0, "ROOT: ");
        } else {
            printf("(Empty tree)\n");
        }
        printf("------------------------------\n\n");
    }

    if (root) {
        free_huffman_tree(root);
    }
}

static void menu_statistics(void) {
    char orig_path[MAX_PATH_LEN];
    char comp_path[MAX_PATH_LEN];

    printf("\n[COMPRESSION STATISTICS COMPARISON]\n");
    printf("Enter original uncompressed file path: ");
    if (!fgets(orig_path, sizeof(orig_path), stdin)) return;
    clean_newline(orig_path);

    if (!file_exists(orig_path)) {
        printf("[ERROR] Original file not found: %s\n\n", orig_path);
        return;
    }

    printf("Enter compressed (.huf) file path: ");
    if (!fgets(comp_path, sizeof(comp_path), stdin)) return;
    clean_newline(comp_path);

    if (!file_exists(comp_path)) {
        printf("[ERROR] Compressed file not found: %s\n\n", comp_path);
        return;
    }

    uint64_t orig_size = 0;
    uint64_t comp_size = 0;
    get_file_size(orig_path, &orig_size);
    get_file_size(comp_path, &comp_size);

    HuffmanHeader header;
    uint64_t frequencies[256];
    HuffmanStatus status = read_compressed_header(comp_path, &header, frequencies);

    if (status != HUFF_SUCCESS) {
        printf("[ERROR] Could not read header from compressed file: %s\n\n", huffman_status_string(status));
        return;
    }

    uint64_t header_size = sizeof(HuffmanHeader) + (header.distinct_count * (1 + sizeof(uint64_t)));
    uint64_t payload_size = (comp_size >= header_size) ? (comp_size - header_size) : 0;

    CompressionStats stats;
    stats.original_size = orig_size;
    stats.compressed_size = comp_size;
    stats.header_size = header_size;
    stats.payload_size = payload_size;
    stats.compression_ratio = (comp_size > 0) ? ((double)orig_size / (double)comp_size) : 0.0;
    stats.space_saved_percent = (orig_size > 0)
        ? ((((double)orig_size - (double)comp_size) / (double)orig_size) * 100.0) : 0.0;
    stats.original_crc = header.original_crc32;
    stats.decompressed_crc = header.original_crc32;
    stats.crc_matches = 1;

    display_compression_stats(&stats);
}

/* Built-in self-test runner */
static void run_self_test_suite(void) {
    printf("\n=======================================================\n");
    printf("            RUNNING AUTOMATED SELF-TEST SUITE          \n");
    printf("=======================================================\n");

    const char *test_cases[] = {
        "tests/test_cases/case1_empty.txt",
        "tests/test_cases/case2_single_char.txt",
        "tests/test_cases/case3_repeats.txt",
        "tests/test_cases/case4_prose.txt",
        "tests/test_cases/case5_all_bytes.bin"
    };

    /* Ensure test files exist */
    FILE *f1 = fopen(test_cases[0], "wb"); if (f1) fclose(f1);
    FILE *f2 = fopen(test_cases[1], "wb"); if (f2) { for(int i=0;i<500;i++) fputc('A', f2); fclose(f2); }
    FILE *f3 = fopen(test_cases[2], "wb"); if (f3) { fputs("AAAAABBBBBCCCCCDDDDDEEEEEFFFFF", f3); fclose(f3); }
    FILE *f4 = fopen(test_cases[3], "wb"); if (f4) {
        fputs("Data Structures and Algorithms (DSA) Mini-Project\n"
              "Huffman Coding Lossless File Compression System.\n"
              "Student: Shalom Dabre, Roll: 11070.\n"
              "Special characters: !@#$%^&*()_+~`|}{[]:;?><,./\n", f4);
        fclose(f4);
    }
    FILE *f5 = fopen(test_cases[4], "wb"); if (f5) {
        for (int i = 0; i < 256; i++) {
            for (int k = 0; k <= i; k++) {
                fputc(i, f5);
            }
        }
        fclose(f5);
    }

    int passed = 0;
    int total = 5;

    for (int i = 0; i < total; i++) {
        char comp_file[MAX_PATH_LEN];
        char decomp_file[MAX_PATH_LEN];
        snprintf(comp_file, sizeof(comp_file), "%s.huf", test_cases[i]);
        snprintf(decomp_file, sizeof(decomp_file), "%s.restored", test_cases[i]);

        printf("Running Test %d [%s]... ", i + 1, test_cases[i]);
        CompressionStats cstats;
        HuffmanStatus c_res = compress_file(test_cases[i], comp_file, &cstats, 1);
        if (c_res != HUFF_SUCCESS) {
            printf("[FAILED: Compression error %s]\n", huffman_status_string(c_res));
            continue;
        }

        CompressionStats dstats;
        HuffmanStatus d_res = decompress_file(comp_file, decomp_file, &dstats, 1);
        if (d_res != HUFF_SUCCESS) {
            printf("[FAILED: Decompression error %s]\n", huffman_status_string(d_res));
            continue;
        }

        uint64_t diff_offset = 0;
        int match = compare_files_exact(test_cases[i], decomp_file, &diff_offset);
        if (match == 1) {
            printf("[PASSED] (Original: %llu B -> Comp: %llu B, CRC: 0x%08X)\n",
                   (unsigned long long)cstats.original_size,
                   (unsigned long long)cstats.compressed_size,
                   cstats.original_crc);
            passed++;
        } else {
            printf("[FAILED: Mismatch at offset %llu]\n", (unsigned long long)diff_offset);
        }
    }

    printf("-------------------------------------------------------\n");
    printf("Self-Test Summary: %d / %d Tests Passed.\n", passed, total);
    printf("=======================================================\n\n");
}

/* CLI command handling */
int main(int argc, char *argv[]) {
    if (argc > 1) {
        /* Command-line arguments supplied: execute headlessly */
        if (strcmp(argv[1], "-c") == 0) {
            if (argc < 4) {
                fprintf(stderr, "Usage: %s -c <input_file> <output_compressed_file>\n", argv[0]);
                return 1;
            }
            CompressionStats stats;
            HuffmanStatus st = compress_file(argv[2], argv[3], &stats, 1);
            if (st != HUFF_SUCCESS) {
                fprintf(stderr, "Compression error: %s\n", huffman_status_string(st));
                return 1;
            }
            display_compression_stats(&stats);
            return 0;
        } else if (strcmp(argv[1], "-d") == 0) {
            if (argc < 4) {
                fprintf(stderr, "Usage: %s -d <input_compressed_file> <output_decompressed_file>\n", argv[0]);
                return 1;
            }
            CompressionStats stats;
            HuffmanStatus st = decompress_file(argv[2], argv[3], &stats, 1);
            if (st != HUFF_SUCCESS) {
                fprintf(stderr, "Decompression error: %s\n", huffman_status_string(st));
                return 1;
            }
            display_compression_stats(&stats);
            return 0;
        } else if (strcmp(argv[1], "-f") == 0) {
            if (argc < 3) {
                fprintf(stderr, "Usage: %s -f <file>\n", argv[0]);
                return 1;
            }
            uint64_t frequencies[256];
            uint64_t size = 0;
            HuffmanStatus st = analyze_file_frequencies(argv[2], frequencies, &size);
            if (st != HUFF_SUCCESS) {
                fprintf(stderr, "Error: %s\n", huffman_status_string(st));
                return 1;
            }
            display_frequency_table(frequencies, size);
            return 0;
        } else if (strcmp(argv[1], "-codes") == 0) {
            if (argc < 3) {
                fprintf(stderr, "Usage: %s -codes <file>\n", argv[0]);
                return 1;
            }
            uint64_t frequencies[256];
            uint64_t size = 0;
            HuffmanStatus st = analyze_file_frequencies(argv[2], frequencies, &size);
            if (st != HUFF_SUCCESS) {
                fprintf(stderr, "Error: %s\n", huffman_status_string(st));
                return 1;
            }
            HuffmanNode *root = build_huffman_tree(frequencies);
            HuffmanCodeTable table;
            generate_huffman_codes(root, &table);
            display_huffman_codes(&table, frequencies);
            if (root) free_huffman_tree(root);
            return 0;
        } else if (strcmp(argv[1], "-test") == 0) {
            run_self_test_suite();
            return 0;
        } else if (strcmp(argv[1], "--gui-info") == 0) {
            /* Outputs structured JSON for the Python GUI */
            if (argc < 3) {
                fprintf(stderr, "{\"error\": \"Missing file path\"}\n");
                return 1;
            }
            uint64_t frequencies[256];
            uint64_t size = 0;
            HuffmanStatus st = analyze_file_frequencies(argv[2], frequencies, &size);
            if (st != HUFF_SUCCESS) {
                printf("{\"error\": \"%s\"}\n", huffman_status_string(st));
                return 1;
            }
            HuffmanNode *root = build_huffman_tree(frequencies);
            HuffmanCodeTable table;
            generate_huffman_codes(root, &table);

            printf("{\n  \"file_size\": %llu,\n  \"distinct_count\": %zu,\n  \"symbols\": [\n",
                   (unsigned long long)size, table.distinct_count);
            int first = 1;
            for (int i = 0; i < 256; i++) {
                if (table.exists[i]) {
                    if (!first) printf(",\n");
                    first = 0;
                    char sym[16];
                    format_byte_symbol((unsigned char)i, sym);
                    printf("    {\"byte\": %d, \"symbol\": \"%s\", \"freq\": %llu, \"code\": \"%s\", \"len\": %zu}",
                           i, sym, (unsigned long long)frequencies[i], table.codes[i].bits, table.codes[i].length);
                }
            }
            printf("\n  ]\n}\n");
            if (root) free_huffman_tree(root);
            return 0;
        } else if (strcmp(argv[1], "--gui-compress") == 0) {
            if (argc < 4) {
                printf("{\"success\": false, \"error\": \"Missing arguments\"}\n");
                return 1;
            }
            CompressionStats stats;
            HuffmanStatus st = compress_file(argv[2], argv[3], &stats, 1);
            if (st != HUFF_SUCCESS) {
                printf("{\"success\": false, \"error\": \"%s\"}\n", huffman_status_string(st));
                return 1;
            }
            printf("{\n  \"success\": true,\n  \"original_size\": %llu,\n  \"compressed_size\": %llu,\n"
                   "  \"header_size\": %llu,\n  \"payload_size\": %llu,\n  \"ratio\": %.4f,\n"
                   "  \"space_saved\": %.2f,\n  \"crc\": \"0x%08X\"\n}\n",
                   (unsigned long long)stats.original_size, (unsigned long long)stats.compressed_size,
                   (unsigned long long)stats.header_size, (unsigned long long)stats.payload_size,
                   stats.compression_ratio, stats.space_saved_percent, stats.original_crc);
            return 0;
        } else if (strcmp(argv[1], "--gui-decompress") == 0) {
            if (argc < 4) {
                printf("{\"success\": false, \"error\": \"Missing arguments\"}\n");
                return 1;
            }
            CompressionStats stats;
            HuffmanStatus st = decompress_file(argv[2], argv[3], &stats, 1);
            if (st != HUFF_SUCCESS) {
                printf("{\"success\": false, \"error\": \"%s\"}\n", huffman_status_string(st));
                return 1;
            }
            printf("{\n  \"success\": true,\n  \"restored_size\": %llu,\n  \"crc_matched\": %s,\n  \"crc\": \"0x%08X\"\n}\n",
                   (unsigned long long)stats.original_size, stats.crc_matches ? "true" : "false", stats.decompressed_crc);
            return 0;
        } else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_banner();
            printf("Usage:\n");
            printf("  %s                           Launch interactive menu\n", argv[0]);
            printf("  %s -c <in> <out.huf>         Compress a file\n", argv[0]);
            printf("  %s -d <in.huf> <out>         Decompress a file\n", argv[0]);
            printf("  %s -f <file>                 Display character frequencies\n", argv[0]);
            printf("  %s -codes <file>             Display Huffman codes table\n", argv[0]);
            printf("  %s -test                     Run automated self-tests\n", argv[0]);
            printf("  %s -h, --help                Show this help screen\n\n", argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Unknown option: %s. Use --help for usage details.\n", argv[1]);
            return 1;
        }
    }

    /* Interactive Menu Mode */
    print_banner();

    while (1) {
        print_menu();
        char line[64];
        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }
        clean_newline(line);
        if (strlen(line) == 0) {
            continue;
        }

        int choice = atoi(line);
        switch (choice) {
            case 1:
                menu_compress();
                break;
            case 2:
                menu_decompress();
                break;
            case 3:
                menu_frequencies();
                break;
            case 4:
                menu_huffman_codes();
                break;
            case 5:
                menu_statistics();
                break;
            case 6:
                run_self_test_suite();
                break;
            case 7:
                printf("\nThank you for using the Huffman File Compression System!\n");
                printf("Student: Shalom Dabre | Roll No: 11070\nExiting...\n");
                return 0;
            default:
                printf("\n[ERROR] Invalid choice '%s'. Please enter a number between 1 and 7.\n\n", line);
                break;
        }
    }

    return 0;
}
