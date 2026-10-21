"""
============================================================================
Project: Huffman Coding-Based File Compression System
Course:  Computer Engineering — Data Structures and Algorithms
Student: Shalom Dabre
Roll No: 11070

File:    gui/app.py
Purpose: Graphical User Interface using Python Tkinter.
         Integrates seamlessly with the compiled C binary (huffman.exe)
         via standard subprocess calls to ensure single algorithmic core.
============================================================================
"""

import os
import sys
import json
import subprocess
import tkinter as tk
from tkinter import ttk, filedialog, messagebox

# Resolve path to the compiled C executable
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, ".."))
EXECUTABLE_NAME = "huffman.exe" if os.name == "nt" else "huffman"
EXECUTABLE_PATH = os.path.join(PROJECT_ROOT, EXECUTABLE_NAME)


def format_size(bytes_val):
    """Formats bytes into human-friendly representation."""
    if bytes_val < 1024:
        return f"{bytes_val:,} Bytes"
    elif bytes_val < 1024 * 1024:
        return f"{bytes_val / 1024:.2f} KB ({bytes_val:,} B)"
    else:
        return f"{bytes_val / (1024 * 1024):.2f} MB ({bytes_val:,} B)"


class HuffmanGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Huffman Coding File Compression System — Shalom Dabre (11070)")
        self.root.geometry("1024x720")
        self.root.minsize(860, 600)

        # Apply Modern Styling
        self.setup_styles()

        # Check binary availability
        self.verify_executable()

        # Build UI Layout
        self.build_header()
        self.build_tabs()
        self.build_footer()

    def setup_styles(self):
        style = ttk.Style()
        try:
            style.theme_use("clam")
        except Exception:
            pass

        # Color palette
        self.bg_color = "#f8fafc"
        self.card_bg = "#ffffff"
        self.primary_color = "#1d4ed8"
        self.text_color = "#0f172a"

        self.root.configure(bg=self.bg_color)
        style.configure(".", background=self.bg_color, foreground=self.text_color, font=("Segoe UI", 10))
        style.configure("TNotebook", background=self.bg_color)
        style.configure("TNotebook.Tab", padding=[16, 8], font=("Segoe UI", 10, "bold"))
        style.configure("Accent.TButton", font=("Segoe UI", 10, "bold"), background=self.primary_color, foreground="#ffffff")
        style.map("Accent.TButton", background=[("active", "#1e40af")])
        style.configure("Card.TFrame", background=self.card_bg, relief="flat")
        style.configure("Treeview.Heading", font=("Segoe UI", 9, "bold"))

    def verify_executable(self):
        if not os.path.isfile(EXECUTABLE_PATH):
            messagebox.showwarning(
                "Executable Missing",
                f"Compiled binary not found at:\n{EXECUTABLE_PATH}\n\n"
                "Please compile the C core first using 'gcc' or 'make'."
            )

    def build_header(self):
        header_frame = tk.Frame(self.root, bg="#0f172a", padx=20, pady=12)
        header_frame.pack(fill=tk.X)

        title_lbl = tk.Label(
            header_frame,
            text="HUFFMAN CODING-BASED FILE COMPRESSION SYSTEM",
            font=("Segoe UI", 14, "bold"),
            fg="#ffffff",
            bg="#0f172a"
        )
        title_lbl.pack(anchor="w")

        info_lbl = tk.Label(
            header_frame,
            text="Student: Shalom Dabre  |  Roll No: 11070  |  Course: Computer Engineering — Data Structures and Algorithms",
            font=("Segoe UI", 9),
            fg="#94a3b8",
            bg="#0f172a"
        )
        info_lbl.pack(anchor="w", pady=(2, 0))

    def build_tabs(self):
        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill=tk.BOTH, expand=True, padx=16, pady=12)

        # Tab 1: Compress & Analyze
        self.tab_compress = ttk.Frame(self.notebook)
        self.notebook.add(self.tab_compress, text="  1. Compress & Analyze  ")
        self.setup_compress_tab()

        # Tab 2: Decompress
        self.tab_decompress = ttk.Frame(self.notebook)
        self.notebook.add(self.tab_decompress, text="  2. Decompress File  ")
        self.setup_decompress_tab()

        # Tab 3: Tree Visualizer
        self.tab_tree = ttk.Frame(self.notebook)
        self.notebook.add(self.tab_tree, text="  3. Tree Visualizer  ")
        self.setup_tree_tab()

        # Tab 4: Automated Self-Test
        self.tab_test = ttk.Frame(self.notebook)
        self.notebook.add(self.tab_test, text="  4. Automated Self-Test  ")
        self.setup_test_tab()

    def build_footer(self):
        footer_frame = tk.Frame(self.root, bg="#e2e8f0", padx=16, pady=6)
        footer_frame.pack(fill=tk.X, side=tk.BOTTOM)

        self.status_var = tk.StringVar(value="Ready. Select an input file to begin.")
        status_lbl = tk.Label(
            footer_frame,
            textvariable=self.status_var,
            font=("Segoe UI", 9),
            fg="#334155",
            bg="#e2e8f0"
        )
        status_lbl.pack(side=tk.LEFT)

    # -------------------------------------------------------------------------
    # TAB 1: COMPRESS & ANALYZE
    # -------------------------------------------------------------------------
    def setup_compress_tab(self):
        # File selector frame
        card = tk.LabelFrame(self.tab_compress, text=" File Selection ", font=("Segoe UI", 10, "bold"),
                             bg=self.card_bg, padx=16, pady=12, relief="groove")
        card.pack(fill=tk.X, padx=12, pady=8)

        # Input File
        tk.Label(card, text="Input File:", bg=self.card_bg, font=("Segoe UI", 9, "bold")).grid(row=0, column=0, sticky="w", pady=4)
        self.comp_in_var = tk.StringVar()
        in_entry = ttk.Entry(card, textvariable=self.comp_in_var, width=65)
        in_entry.grid(row=0, column=1, padx=8, pady=4, sticky="we")
        ttk.Button(card, text="Browse...", command=self.browse_compress_in).grid(row=0, column=2, padx=4, pady=4)

        # Output File
        tk.Label(card, text="Compressed Output (.huf):", bg=self.card_bg, font=("Segoe UI", 9, "bold")).grid(row=1, column=0, sticky="w", pady=4)
        self.comp_out_var = tk.StringVar()
        out_entry = ttk.Entry(card, textvariable=self.comp_out_var, width=65)
        out_entry.grid(row=1, column=1, padx=8, pady=4, sticky="we")
        ttk.Button(card, text="Browse...", command=self.browse_compress_out).grid(row=1, column=2, padx=4, pady=4)

        card.columnconfigure(1, weight=1)

        # Action Buttons
        btn_frame = tk.Frame(card, bg=self.card_bg)
        btn_frame.grid(row=2, column=0, columnspan=3, pady=(8, 0), sticky="e")

        ttk.Button(btn_frame, text="Analyze Frequencies & Codes", command=self.run_analyze).pack(side=tk.LEFT, padx=6)
        ttk.Button(btn_frame, text="Compress File", style="Accent.TButton", command=self.run_compress).pack(side=tk.LEFT, padx=6)

        # Compression Statistics Cards
        self.stats_frame = tk.LabelFrame(self.tab_compress, text=" Compression Statistics ",
                                         font=("Segoe UI", 10, "bold"), bg=self.card_bg, padx=12, pady=8, relief="groove")
        self.stats_frame.pack(fill=tk.X, padx=12, pady=6)

        self.stat_orig_sz = tk.StringVar(value="-")
        self.stat_comp_sz = tk.StringVar(value="-")
        self.stat_ratio = tk.StringVar(value="-")
        self.stat_saved = tk.StringVar(value="-")
        self.stat_crc = tk.StringVar(value="-")

        stats_grid = tk.Frame(self.stats_frame, bg=self.card_bg)
        stats_grid.pack(fill=tk.X)

        self.add_stat_box(stats_grid, 0, "Original Size", self.stat_orig_sz)
        self.add_stat_box(stats_grid, 1, "Compressed Size", self.stat_comp_sz)
        self.add_stat_box(stats_grid, 2, "Compression Ratio", self.stat_ratio)
        self.add_stat_box(stats_grid, 3, "Space Saved", self.stat_saved)
        self.add_stat_box(stats_grid, 4, "CRC-32 Checksum", self.stat_crc)

        # Frequency & Huffman Codes Table
        tbl_frame = tk.LabelFrame(self.tab_compress, text=" Character Frequency & Huffman Codes Table ",
                                  font=("Segoe UI", 10, "bold"), bg=self.card_bg, padx=8, pady=8, relief="groove")
        tbl_frame.pack(fill=tk.BOTH, expand=True, padx=12, pady=6)

        cols = ("no", "byte", "symbol", "freq", "pct", "code", "bits")
        self.code_tree = ttk.Treeview(tbl_frame, columns=cols, show="headings", height=8)
        self.code_tree.heading("no", text="#")
        self.code_tree.heading("byte", text="Hex Byte")
        self.code_tree.heading("symbol", text="Symbol / Representation")
        self.code_tree.heading("freq", text="Frequency")
        self.code_tree.heading("pct", text="Percentage")
        self.code_tree.heading("code", text="Huffman Variable Bit Code")
        self.code_tree.heading("bits", text="Bit Length")

        self.code_tree.column("no", width=40, anchor="center")
        self.code_tree.column("byte", width=80, anchor="center")
        self.code_tree.column("symbol", width=180, anchor="w")
        self.code_tree.column("freq", width=110, anchor="e")
        self.code_tree.column("pct", width=90, anchor="e")
        self.code_tree.column("code", width=220, anchor="w")
        self.code_tree.column("bits", width=80, anchor="center")

        scroll = ttk.Scrollbar(tbl_frame, orient=tk.VERTICAL, command=self.code_tree.yview)
        self.code_tree.configure(yscrollcommand=scroll.set)
        self.code_tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scroll.pack(side=tk.RIGHT, fill=tk.Y)

    def add_stat_box(self, parent, col, title, var):
        box = tk.Frame(parent, bg="#f1f5f9", padx=10, pady=8, relief="solid", bd=1)
        box.grid(row=0, column=col, padx=6, pady=4, sticky="nsew")
        parent.columnconfigure(col, weight=1)

        tk.Label(box, text=title, font=("Segoe UI", 8, "bold"), fg="#475569", bg="#f1f5f9").pack(anchor="w")
        tk.Label(box, textvariable=var, font=("Segoe UI", 11, "bold"), fg="#0f172a", bg="#f1f5f9").pack(anchor="w", pady=(2, 0))

    def browse_compress_in(self):
        f = filedialog.askopenfilename(title="Select File to Compress")
        if f:
            self.comp_in_var.set(f)
            self.comp_out_var.set(f + ".huf")
            self.status_var.set(f"Selected file: {os.path.basename(f)}")

    def browse_compress_out(self):
        f = filedialog.asksaveasfilename(title="Save Compressed File As", defaultextension=".huf",
                                         filetypes=[("Huffman Compressed Archive", "*.huf"), ("All Files", "*.*")])
        if f:
            self.comp_out_var.set(f)

    def run_analyze(self):
        in_path = self.comp_in_var.get().strip()
        if not in_path or not os.path.isfile(in_path):
            messagebox.showerror("Error", "Please select a valid input file.")
            return

        self.status_var.set(f"Analyzing '{os.path.basename(in_path)}'...")
        self.root.update_idletasks()

        try:
            res = subprocess.run([EXECUTABLE_PATH, "--gui-info", in_path],
                                 capture_output=True, text=True, check=True)
            data = json.loads(res.stdout)

            # Clear table
            for row in self.code_tree.get_children():
                self.code_tree.delete(row)

            total_size = data.get("file_size", 0)
            self.stat_orig_sz.set(format_size(total_size))

            idx = 1
            for sym in data.get("symbols", []):
                freq = sym.get("freq", 0)
                pct = (freq / total_size * 100.0) if total_size > 0 else 0.0
                byte_hex = f"0x{sym.get('byte', 0):02X}"
                self.code_tree.insert("", "end", values=(
                    idx,
                    byte_hex,
                    sym.get("symbol", ""),
                    f"{freq:,}",
                    f"{pct:.2f}%",
                    sym.get("code", ""),
                    sym.get("len", 0)
                ))
                idx += 1

            self.status_var.set(f"Analysis complete: {len(data.get('symbols', []))} distinct symbols found.")
            # Populate visual tree tab as well
            self.update_tree_visualization(data.get("symbols", []))
        except Exception as e:
            messagebox.showerror("Execution Error", f"Failed to analyze file:\n{str(e)}")

    def run_compress(self):
        in_path = self.comp_in_var.get().strip()
        out_path = self.comp_out_var.get().strip()

        if not in_path or not os.path.isfile(in_path):
            messagebox.showerror("Error", "Please select a valid input file.")
            return
        if not out_path:
            messagebox.showerror("Error", "Please specify an output file path.")
            return

        self.status_var.set(f"Compressing '{os.path.basename(in_path)}'...")
        self.root.update_idletasks()

        try:
            res = subprocess.run([EXECUTABLE_PATH, "--gui-compress", in_path, out_path],
                                 capture_output=True, text=True, check=True)
            data = json.loads(res.stdout)

            if data.get("success"):
                orig_sz = data.get("original_size", 0)
                comp_sz = data.get("compressed_size", 0)
                ratio = data.get("ratio", 0.0)
                saved = data.get("space_saved", 0.0)
                crc = data.get("crc", "")

                self.stat_orig_sz.set(format_size(orig_sz))
                self.stat_comp_sz.set(format_size(comp_sz))
                self.stat_ratio.set(f"{ratio:.3f}:1")
                self.stat_saved.set(f"{saved:.2f}%")
                self.stat_crc.set(crc)

                self.status_var.set(f"Compression succeeded: Saved to {os.path.basename(out_path)} ({saved:.2f}% space saved)")
                # Also load code table
                self.run_analyze()

                messagebox.showinfo(
                    "Compression Successful",
                    f"File successfully compressed!\n\n"
                    f"Original Size: {format_size(orig_sz)}\n"
                    f"Compressed Size: {format_size(comp_sz)}\n"
                    f"Compression Ratio: {ratio:.3f}:1\n"
                    f"Space Saved: {saved:.2f}%\n"
                    f"CRC-32: {crc}"
                )
            else:
                messagebox.showerror("Compression Failed", data.get("error", "Unknown error."))
        except Exception as e:
            messagebox.showerror("Execution Error", f"Failed to execute compressor:\n{str(e)}")

    # -------------------------------------------------------------------------
    # TAB 2: DECOMPRESS
    # -------------------------------------------------------------------------
    def setup_decompress_tab(self):
        card = tk.LabelFrame(self.tab_decompress, text=" Decompression Settings ", font=("Segoe UI", 10, "bold"),
                             bg=self.card_bg, padx=16, pady=16, relief="groove")
        card.pack(fill=tk.X, padx=16, pady=16)

        # Input File
        tk.Label(card, text="Compressed Input (.huf):", bg=self.card_bg, font=("Segoe UI", 9, "bold")).grid(row=0, column=0, sticky="w", pady=6)
        self.decomp_in_var = tk.StringVar()
        ttk.Entry(card, textvariable=self.decomp_in_var, width=65).grid(row=0, column=1, padx=8, pady=6, sticky="we")
        ttk.Button(card, text="Browse...", command=self.browse_decomp_in).grid(row=0, column=2, padx=4, pady=6)

        # Output File
        tk.Label(card, text="Restored Output Path:", bg=self.card_bg, font=("Segoe UI", 9, "bold")).grid(row=1, column=0, sticky="w", pady=6)
        self.decomp_out_var = tk.StringVar()
        ttk.Entry(card, textvariable=self.decomp_out_var, width=65).grid(row=1, column=1, padx=8, pady=6, sticky="we")
        ttk.Button(card, text="Browse...", command=self.browse_decomp_out).grid(row=1, column=2, padx=4, pady=6)

        card.columnconfigure(1, weight=1)

        btn_frame = tk.Frame(card, bg=self.card_bg)
        btn_frame.grid(row=2, column=0, columnspan=3, pady=(12, 0), sticky="e")
        ttk.Button(btn_frame, text="Decompress File", style="Accent.TButton", command=self.run_decompress).pack(side=tk.RIGHT)

        # Decompression Results Card
        self.decomp_results_card = tk.LabelFrame(self.tab_decompress, text=" Decompression Results & Verification ",
                                                 font=("Segoe UI", 10, "bold"), bg=self.card_bg, padx=16, pady=16, relief="groove")
        self.decomp_results_card.pack(fill=tk.BOTH, expand=True, padx=16, pady=12)

        self.decomp_res_size = tk.StringVar(value="-")
        self.decomp_res_crc = tk.StringVar(value="-")
        self.decomp_res_status = tk.StringVar(value="Waiting for decompression...")

        res_grid = tk.Frame(self.decomp_results_card, bg=self.card_bg)
        res_grid.pack(fill=tk.X, pady=8)

        self.add_stat_box(res_grid, 0, "Restored File Size", self.decomp_res_size)
        self.add_stat_box(res_grid, 1, "Data Integrity (CRC-32)", self.decomp_res_crc)

        status_box = tk.Frame(self.decomp_results_card, bg="#f8fafc", padx=16, pady=16, relief="solid", bd=1)
        status_box.pack(fill=tk.BOTH, expand=True, pady=12)

        tk.Label(status_box, text="Verification Status:", font=("Segoe UI", 10, "bold"), bg="#f8fafc").pack(anchor="w")
        self.status_detail_lbl = tk.Label(
            status_box,
            textvariable=self.decomp_res_status,
            font=("Segoe UI", 12),
            fg="#475569",
            bg="#f8fafc",
            wraplength=600,
            justify="left"
        )
        self.status_detail_lbl.pack(anchor="w", pady=(6, 0))

    def browse_decomp_in(self):
        f = filedialog.askopenfilename(title="Select Compressed File", filetypes=[("Huffman Compressed Archive", "*.huf"), ("All Files", "*.*")])
        if f:
            self.decomp_in_var.set(f)
            if f.endswith(".huf"):
                self.decomp_out_var.set(f[:-4] + ".restored")
            else:
                self.decomp_out_var.set(f + ".restored")
            self.status_var.set(f"Selected archive: {os.path.basename(f)}")

    def browse_decomp_out(self):
        f = filedialog.asksaveasfilename(title="Save Restored File As")
        if f:
            self.decomp_out_var.set(f)

    def run_decompress(self):
        in_path = self.decomp_in_var.get().strip()
        out_path = self.decomp_out_var.get().strip()

        if not in_path or not os.path.isfile(in_path):
            messagebox.showerror("Error", "Please select a valid compressed (.huf) file.")
            return
        if not out_path:
            messagebox.showerror("Error", "Please specify an output file destination.")
            return

        self.status_var.set(f"Decompressing '{os.path.basename(in_path)}'...")
        self.root.update_idletasks()

        try:
            res = subprocess.run([EXECUTABLE_PATH, "--gui-decompress", in_path, out_path],
                                 capture_output=True, text=True, check=True)
            data = json.loads(res.stdout)

            if data.get("success"):
                sz = data.get("restored_size", 0)
                crc = data.get("crc", "")
                matched = data.get("crc_matched", False)

                self.decomp_res_size.set(format_size(sz))
                self.decomp_res_crc.set(crc)

                if matched:
                    self.decomp_res_status.set("SUCCESS: 100% Lossless Recovery Verified!\nCRC-32 checksum matches original source data exactly.")
                    self.status_detail_lbl.configure(fg="#16a34a")
                else:
                    self.decomp_res_status.set("CORRUPTION DETECTED: Checksum mismatch between decompressed data and header.")
                    self.status_detail_lbl.configure(fg="#dc2626")

                self.status_var.set("Decompression finished successfully.")
                messagebox.showinfo("Success", f"File restored successfully to:\n{out_path}")
            else:
                messagebox.showerror("Decompression Failed", data.get("error", "Unknown error."))
        except Exception as e:
            messagebox.showerror("Execution Error", f"Decompression failed:\n{str(e)}")

    # -------------------------------------------------------------------------
    # TAB 3: TREE VISUALIZER
    # -------------------------------------------------------------------------
    def setup_tree_tab(self):
        ctrl_frame = tk.Frame(self.tab_tree, bg=self.bg_color, pady=6)
        ctrl_frame.pack(fill=tk.X, padx=12)

        tk.Label(ctrl_frame, text="Hierarchical Huffman Tree Structure & Prefix Encodings",
                 font=("Segoe UI", 11, "bold"), bg=self.bg_color).pack(side=tk.LEFT)

        canvas_frame = tk.Frame(self.tab_tree, bg=self.card_bg, relief="solid", bd=1)
        canvas_frame.pack(fill=tk.BOTH, expand=True, padx=12, pady=6)

        self.tree_canvas = tk.Canvas(canvas_frame, bg="#ffffff")
        v_scroll = ttk.Scrollbar(canvas_frame, orient=tk.VERTICAL, command=self.tree_canvas.yview)
        h_scroll = ttk.Scrollbar(canvas_frame, orient=tk.HORIZONTAL, command=self.tree_canvas.xview)
        self.tree_canvas.configure(xscrollcommand=h_scroll.set, yscrollcommand=v_scroll.set)

        self.tree_canvas.pack(side=tk.TOP, fill=tk.BOTH, expand=True)
        h_scroll.pack(side=tk.BOTTOM, fill=tk.X)
        v_scroll.pack(side=tk.RIGHT, fill=tk.Y)

        self.tree_canvas.create_text(
            400, 200,
            text="Run 'Analyze Frequencies & Codes' or 'Compress File' to visualize the Huffman Tree.",
            font=("Segoe UI", 11, "italic"),
            fill="#64748b"
        )

    def update_tree_visualization(self, symbols):
        self.tree_canvas.delete("all")
        if not symbols:
            self.tree_canvas.create_text(
                400, 200,
                text="No symbol data to render.",
                font=("Segoe UI", 11, "italic"),
                fill="#64748b"
            )
            return

        y_offset = 30
        self.tree_canvas.create_text(
            30, y_offset,
            text=f"Huffman Code Paths ({len(symbols)} Leaf Symbols)",
            font=("Segoe UI", 12, "bold"),
            anchor="w",
            fill="#0f172a"
        )
        y_offset += 35

        # Render each symbol as a tree path badge
        for s in symbols[:60]: # render top symbols
            sym_text = s.get("symbol", "")
            code = s.get("code", "")
            freq = s.get("freq", 0)
            bit_len = s.get("len", 0)

            # Draw root box
            self.tree_canvas.create_rectangle(30, y_offset, 90, y_offset + 24, fill="#e2e8f0", outline="#94a3b8")
            self.tree_canvas.create_text(60, y_offset + 12, text="ROOT", font=("Segoe UI", 8, "bold"), fill="#334155")

            # Draw connector line
            line_end_x = 120 + (bit_len * 24)
            self.tree_canvas.create_line(90, y_offset + 12, line_end_x, y_offset + 12, fill="#0ea5e9", width=2)

            # Draw bits along line
            for b_idx, bit_char in enumerate(code):
                bx = 110 + (b_idx * 24)
                color = "#2563eb" if bit_char == '0' else "#dc2626"
                self.tree_canvas.create_text(bx, y_offset + 2, text=bit_char, font=("Segoe UI", 8, "bold"), fill=color)

            # Draw leaf badge
            self.tree_canvas.create_rectangle(line_end_x, y_offset, line_end_x + 180, y_offset + 24,
                                              fill="#ecfdf5", outline="#10b981")
            self.tree_canvas.create_text(
                line_end_x + 10, y_offset + 12,
                text=f"{sym_text} (Freq: {freq:,})",
                font=("Segoe UI", 8, "bold"),
                anchor="w",
                fill="#065f46"
            )

            y_offset += 32

        if len(symbols) > 60:
            self.tree_canvas.create_text(
                30, y_offset + 10,
                text=f"... and {len(symbols) - 60} more symbols (see table in Tab 1 for full list).",
                font=("Segoe UI", 9, "italic"),
                anchor="w",
                fill="#64748b"
            )
            y_offset += 30

        self.tree_canvas.configure(scrollregion=(0, 0, 900, y_offset + 50))

    # -------------------------------------------------------------------------
    # TAB 4: AUTOMATED SELF-TEST
    # -------------------------------------------------------------------------
    def setup_test_tab(self):
        ctrl_frame = tk.Frame(self.tab_test, bg=self.bg_color, pady=8)
        ctrl_frame.pack(fill=tk.X, padx=12)

        tk.Label(ctrl_frame, text="Comprehensive Verification & Regression Suite",
                 font=("Segoe UI", 11, "bold"), bg=self.bg_color).pack(side=tk.LEFT)

        ttk.Button(ctrl_frame, text="Run Automated Test Suite", style="Accent.TButton",
                   command=self.run_automated_tests).pack(side=tk.RIGHT)

        log_frame = tk.Frame(self.tab_test, bg=self.card_bg, relief="solid", bd=1)
        log_frame.pack(fill=tk.BOTH, expand=True, padx=12, pady=6)

        self.test_log_txt = tk.Text(log_frame, bg="#0f172a", fg="#f8fafc", font=("Consolas", 10), padx=12, pady=12)
        scroll = ttk.Scrollbar(log_frame, orient=tk.VERTICAL, command=self.test_log_txt.yview)
        self.test_log_txt.configure(yscrollcommand=scroll.set)

        self.test_log_txt.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scroll.pack(side=tk.RIGHT, fill=tk.Y)

        self.test_log_txt.insert(tk.END, "Click 'Run Automated Test Suite' above to execute all 25 unit and end-to-end tests.\n")

    def run_automated_tests(self):
        test_runner_path = os.path.join(PROJECT_ROOT, "tests", "test_runner.exe" if os.name == "nt" else "test_runner")
        if not os.path.isfile(test_runner_path):
            # Fall back to huffman.exe -test
            cmd = [EXECUTABLE_PATH, "-test"]
        else:
            cmd = [test_runner_path]

        self.test_log_txt.delete("1.0", tk.END)
        self.test_log_txt.insert(tk.END, f"Executing: {' '.join(cmd)}\n\n")
        self.root.update_idletasks()

        try:
            res = subprocess.run(cmd, capture_output=True, text=True)
            self.test_log_txt.insert(tk.END, res.stdout)
            if res.stderr:
                self.test_log_txt.insert(tk.END, "\n[STDERR]\n" + res.stderr)

            if res.returncode == 0:
                self.status_var.set("All automated tests completed successfully! (100% Passed)")
                messagebox.showinfo("Tests Passed", "All automated unit and system tests executed successfully!")
            else:
                self.status_var.set("Some tests failed. Check log.")
                messagebox.showwarning("Tests Failed", "One or more tests reported a failure. See log for details.")
        except Exception as e:
            self.test_log_txt.insert(tk.END, f"\nExecution failed: {str(e)}\n")
            messagebox.showerror("Error", f"Failed to run test suite:\n{str(e)}")


def main():
    root = tk.Tk()
    app = HuffmanGUI(root)
    root.mainloop()


if __name__ == "__main__":
    main()
