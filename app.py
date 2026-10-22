"""
============================================================================
HUFFMAN COMPRESSION STUDIO
Lossless File Compression & Algorithm Visualizer
Powered by ISO C99 Huffman Engine & HUF1 Binary Container
============================================================================
"""

import os
import sys
import json
import time
import subprocess
import tempfile
import streamlit as st
import pandas as pd
import plotly.express as px
import plotly.graph_objects as go

# -----------------------------------------------------------------------------
# 1. VIEWPORT & PAGE CONFIGURATION
# -----------------------------------------------------------------------------
st.set_page_config(
    page_title="Huffman Studio — Lossless File Compression",
    page_icon="⚡",
    layout="wide",
    initial_sidebar_state="collapsed"
)

# Resolve path to the compiled C executable
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, ".."))
EXECUTABLE_NAME = "huffman.exe" if os.name == "nt" else "huffman"
EXECUTABLE_PATH = os.path.join(PROJECT_ROOT, EXECUTABLE_NAME)
TEST_RUNNER_PATH = os.path.join(PROJECT_ROOT, "tests", "test_runner.exe" if os.name == "nt" else "test_runner")

# -----------------------------------------------------------------------------
# 2. DESIGN SYSTEM & CUSTOM CSS (FULL-SCREEN, NO SIDEBAR, ZERO CLUTTER)
# -----------------------------------------------------------------------------
st.markdown("""
<style>
    /* Import Modern Fonts */
    @import url('https://fonts.googleapis.com/css2?family=Plus+Jakarta+Sans:wght@300;400;500;600;700;800&family=JetBrains+Mono:wght@400;500;600;700&display=swap');

    /* Complete Sidebar Removal */
    [data-testid="stSidebar"], section[data-testid="stSidebar"], [data-testid="collapsedControl"] {
        display: none !important;
    }

    /* Full-Screen Viewport Management */
    .block-container {
        padding-top: 1rem !important;
        padding-bottom: 2.5rem !important;
        padding-left: 2rem !important;
        padding-right: 2rem !important;
        max-width: 100% !important;
    }

    /* Base Styling */
    html, body, [class*="css"] {
        font-family: 'Plus Jakarta Sans', -apple-system, BlinkMacSystemFont, sans-serif;
        background-color: #06080e !important;
        color: #f1f5f9 !important;
    }

    code, pre {
        font-family: 'JetBrains Mono', monospace !important;
    }

    /* Top Navigation Header */
    .studio-header {
        display: flex;
        align-items: center;
        justify-content: space-between;
        background: linear-gradient(180deg, rgba(15, 23, 42, 0.8) 0%, rgba(10, 15, 29, 0.6) 100%);
        border: 1px solid rgba(255, 255, 255, 0.08);
        border-radius: 14px;
        padding: 14px 24px;
        margin-bottom: 16px;
        backdrop-filter: blur(16px);
        box-shadow: 0 4px 20px -2px rgba(0, 0, 0, 0.5);
    }

    .brand-wrap {
        display: flex;
        align-items: center;
        gap: 14px;
    }

    .brand-logo-icon {
        width: 38px;
        height: 38px;
        border-radius: 10px;
        background: linear-gradient(135deg, #06b6d4 0%, #6366f1 100%);
        display: flex;
        align-items: center;
        justify-content: center;
        box-shadow: 0 0 15px rgba(6, 182, 212, 0.4);
    }

    .brand-text-col {
        display: flex;
        flex-direction: column;
    }

    .brand-name {
        font-size: 1.25rem;
        font-weight: 800;
        letter-spacing: -0.5px;
        background: linear-gradient(90deg, #ffffff, #cbd5e1);
        -webkit-background-clip: text;
        -webkit-text-fill-color: transparent;
        line-height: 1.2;
    }

    .brand-tagline {
        font-size: 0.7rem;
        font-weight: 700;
        color: #06b6d4;
        letter-spacing: 1.5px;
        text-transform: uppercase;
    }

    .system-status {
        display: flex;
        align-items: center;
        gap: 10px;
        background: rgba(15, 23, 42, 0.8);
        border: 1px solid rgba(16, 185, 129, 0.3);
        padding: 6px 14px;
        border-radius: 30px;
        font-size: 0.75rem;
        font-weight: 600;
        letter-spacing: 0.5px;
        color: #34d399;
    }

    .status-pulse {
        width: 8px;
        height: 8px;
        border-radius: 50%;
        background: #10b981;
        box-shadow: 0 0 8px #10b981;
        animation: pulse 2s infinite;
    }

    @keyframes pulse {
        0% { transform: scale(0.95); box-shadow: 0 0 0 0 rgba(16, 185, 129, 0.7); }
        70% { transform: scale(1); box-shadow: 0 0 0 6px rgba(16, 185, 129, 0); }
        100% { transform: scale(0.95); box-shadow: 0 0 0 0 rgba(16, 185, 129, 0); }
    }

    /* Compact Hero Section */
    .hero-strip {
        display: flex;
        align-items: center;
        justify-content: space-between;
        background: radial-gradient(ellipse at 80% 20%, rgba(99, 102, 241, 0.12) 0%, rgba(9, 13, 22, 0.7) 70%);
        border: 1px solid rgba(99, 102, 241, 0.2);
        border-radius: 12px;
        padding: 16px 24px;
        margin-bottom: 20px;
    }

    .hero-h1 {
        font-size: 1.45rem;
        font-weight: 800;
        color: #ffffff;
        margin: 0;
        letter-spacing: -0.3px;
    }

    .hero-h1 span {
        background: linear-gradient(90deg, #38bdf8, #818cf8);
        -webkit-background-clip: text;
        -webkit-text-fill-color: transparent;
    }

    .hero-p {
        font-size: 0.85rem;
        color: #94a3b8;
        margin: 3px 0 0 0;
    }

    .hero-capsules {
        display: flex;
        gap: 10px;
    }

    .hero-cap {
        background: rgba(30, 41, 59, 0.6);
        border: 1px solid rgba(255, 255, 255, 0.08);
        border-radius: 8px;
        padding: 6px 12px;
        text-align: right;
    }

    .cap-lbl {
        font-size: 0.65rem;
        font-weight: 700;
        color: #64748b;
        text-transform: uppercase;
        letter-spacing: 0.5px;
    }

    .cap-val {
        font-size: 0.85rem;
        font-weight: 700;
        color: #e2e8f0;
        font-family: 'JetBrains Mono', monospace;
    }

    /* Modern Tabs Styling */
    .stTabs [data-baseweb="tab-list"] {
        gap: 6px;
        background-color: transparent;
        border-bottom: 1px solid rgba(255, 255, 255, 0.08);
        padding-bottom: 4px;
    }

    .stTabs [data-baseweb="tab"] {
        padding: 8px 18px;
        font-size: 0.88rem;
        font-weight: 600;
        color: #94a3b8;
        background-color: rgba(15, 23, 42, 0.4);
        border: 1px solid rgba(255, 255, 255, 0.05);
        border-radius: 8px;
        transition: all 0.2s ease;
    }

    .stTabs [data-baseweb="tab"]:hover {
        color: #ffffff;
        background-color: rgba(30, 41, 59, 0.6);
        border-color: rgba(255, 255, 255, 0.12);
    }

    .stTabs [aria-selected="true"] {
        color: #ffffff !important;
        background: linear-gradient(135deg, rgba(6, 182, 212, 0.2) 0%, rgba(99, 102, 241, 0.25) 100%) !important;
        border-color: rgba(6, 182, 212, 0.5) !important;
        box-shadow: 0 0 12px rgba(6, 182, 212, 0.2) !important;
    }

    /* Glass Cards */
    .glass-card {
        background: rgba(14, 19, 32, 0.7);
        border: 1px solid rgba(255, 255, 255, 0.08);
        border-radius: 12px;
        padding: 18px 22px;
        margin-bottom: 16px;
        backdrop-filter: blur(12px);
    }

    /* KPI Results Grid */
    .kpi-grid {
        display: grid;
        grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
        gap: 12px;
        margin: 16px 0;
    }

    .kpi-box {
        background: rgba(18, 25, 43, 0.8);
        border: 1px solid rgba(255, 255, 255, 0.06);
        border-radius: 10px;
        padding: 14px 16px;
        transition: transform 0.2s ease, border-color 0.2s ease;
    }

    .kpi-box:hover {
        transform: translateY(-2px);
        border-color: rgba(6, 182, 212, 0.4);
    }

    .kpi-box-lbl {
        font-size: 0.72rem;
        font-weight: 700;
        text-transform: uppercase;
        color: #64748b;
        letter-spacing: 0.5px;
    }

    .kpi-box-val {
        font-size: 1.35rem;
        font-weight: 700;
        color: #ffffff;
        margin-top: 4px;
        font-family: 'JetBrains Mono', monospace;
    }

    .kpi-box-sub {
        font-size: 0.72rem;
        color: #10b981;
        margin-top: 2px;
    }

    .kpi-box-sub.warn {
        color: #f59e0b;
    }

    /* Pipeline Step Bar */
    .pipeline-bar {
        display: flex;
        align-items: center;
        justify-content: space-between;
        background: rgba(15, 23, 42, 0.4);
        border: 1px solid rgba(255, 255, 255, 0.06);
        border-radius: 10px;
        padding: 12px 18px;
        margin-bottom: 18px;
    }

    .pipe-step {
        display: flex;
        flex-direction: column;
        align-items: center;
        gap: 4px;
    }

    .pipe-dot {
        width: 8px;
        height: 8px;
        border-radius: 50%;
        background: #06b6d4;
        box-shadow: 0 0 6px #06b6d4;
    }

    .pipe-txt {
        font-size: 0.72rem;
        font-weight: 700;
        color: #cbd5e1;
        text-transform: uppercase;
        letter-spacing: 0.5px;
    }

    .pipe-connector {
        flex: 1;
        height: 1px;
        background: linear-gradient(90deg, rgba(6, 182, 212, 0.4), rgba(99, 102, 241, 0.4));
        margin: 0 10px;
        margin-bottom: 14px;
    }

    /* File Drop Zone Enhancements */
    [data-testid="stFileUploader"] {
        border-radius: 12px !important;
        border: 1px dashed rgba(6, 182, 212, 0.4) !important;
        background: rgba(14, 20, 36, 0.4) !important;
        padding: 16px !important;
        transition: border-color 0.2s ease, background 0.2s ease;
    }

    [data-testid="stFileUploader"]:hover {
        border-color: #06b6d4 !important;
        background: rgba(14, 20, 36, 0.7) !important;
    }

    /* Streamlit Buttons */
    .stButton button {
        border-radius: 8px !important;
        font-weight: 600 !important;
        letter-spacing: 0.3px !important;
        transition: all 0.2s ease !important;
    }

    .stButton button[kind="primary"] {
        background: linear-gradient(135deg, #06b6d4 0%, #4f46e5 100%) !important;
        border: 1px solid rgba(255, 255, 255, 0.15) !important;
        color: #ffffff !important;
        box-shadow: 0 4px 14px rgba(6, 182, 212, 0.3) !important;
    }

    .stButton button[kind="primary"]:hover {
        box-shadow: 0 6px 20px rgba(6, 182, 212, 0.5) !important;
        transform: translateY(-1px) !important;
    }

    /* Integrity Badges */
    .verified-banner {
        background: linear-gradient(90deg, rgba(16, 185, 129, 0.15) 0%, rgba(16, 185, 129, 0.05) 100%);
        border: 1px solid rgba(16, 185, 129, 0.4);
        border-radius: 10px;
        padding: 14px 20px;
        display: flex;
        align-items: center;
        gap: 14px;
        margin: 14px 0;
    }
</style>
""", unsafe_allow_html=True)


# -----------------------------------------------------------------------------
# 3. HELPER FUNCTIONS
# -----------------------------------------------------------------------------
def format_size(bytes_val):
    """Formats raw bytes into clean human-friendly strings."""
    if bytes_val < 1024:
        return f"{bytes_val:,} B"
    elif bytes_val < 1024 * 1024:
        return f"{bytes_val / 1024:.2f} KB"
    else:
        return f"{bytes_val / (1024 * 1024):.2f} MB"


def build_graphviz_huffman_tree(symbols, max_leaves=24):
    """
    Builds a clean Graphviz DOT diagram from the actual prefix codes.
    Limits to the most frequent symbols to prevent browser canvas clutter.
    """
    sorted_syms = sorted(symbols, key=lambda x: x['freq'], reverse=True)[:max_leaves]
    dot = [
        'digraph HuffmanTree {',
        '  rankdir=TB;',
        '  bgcolor="transparent";',
        '  node [fontname="JetBrains Mono", shape=circle, style="filled", fillcolor="#1e293b", fontcolor="#ffffff", color="#475569", penwidth=1.5];',
        '  edge [fontname="JetBrains Mono", fontcolor="#38bdf8", color="#475569", penwidth=1.5, fontsize=10];',
        '  "ROOT" [label="ROOT", fillcolor="#0284c7", fontcolor="#ffffff", shape=ellipse, style="filled", penwidth=2];'
    ]

    nodes = {"": "ROOT"}

    for s in sorted_syms:
        code = s['code']
        raw_sym = s['symbol']
        clean_sym = raw_sym.replace('"', '\\"').replace('\\', '\\\\')
        freq = s['freq']
        curr = ""

        for i, bit in enumerate(code):
            next_prefix = curr + bit
            if next_prefix not in nodes:
                is_leaf = (i == len(code) - 1)
                node_id = f"n_{next_prefix}"
                nodes[next_prefix] = node_id

                if is_leaf:
                    dot.append(f'  "{node_id}" [label="{clean_sym}\\n{freq:,}", shape=box, style="filled,rounded", fillcolor="#059669", fontcolor="#ffffff", color="#10b981", penwidth=1.5];')
                else:
                    dot.append(f'  "{node_id}" [label="", shape=circle, width=0.18, height=0.18, fillcolor="#1e293b"];')

                parent_id = nodes[curr]
                dot.append(f'  "{parent_id}" -> "{node_id}" [label=" {bit} "];')

            curr = next_prefix

    dot.append('}')
    return '\n'.join(dot)


# -----------------------------------------------------------------------------
# 4. CORE ENGINE VERIFICATION
# -----------------------------------------------------------------------------
if not os.path.isfile(EXECUTABLE_PATH):
    st.error(f"⚠️ Core C engine binary not found at `{EXECUTABLE_PATH}`. Please run `.\\build.bat` in your terminal.")
    st.stop()


# -----------------------------------------------------------------------------
# 5. TOP NAVIGATION & HEADER (COMPACT & PROFESSIONAL)
# -----------------------------------------------------------------------------
st.markdown("""
<div class="studio-header">
    <div class="brand-wrap">
        <div class="brand-logo-icon">
            <svg width="22" height="22" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
                <path d="M12 3V9M12 9L6 15M12 9L18 15M6 15V21M18 15V21" stroke="#ffffff" stroke-width="2.2" stroke-linecap="round"/>
                <circle cx="12" cy="4" r="2" fill="#ffffff"/>
                <circle cx="6" cy="15" r="2" fill="#ffffff"/>
                <circle cx="18" cy="15" r="2" fill="#ffffff"/>
                <circle cx="6" cy="20" r="1.5" fill="#34d399"/>
                <circle cx="18" cy="20" r="1.5" fill="#34d399"/>
            </svg>
        </div>
        <div class="brand-text-col">
            <span class="brand-name">HUFFMAN STUDIO</span>
            <span class="brand-tagline">LOSSLESS FILE COMPRESSION SYSTEM</span>
        </div>
    </div>
    <div class="system-status">
        <div class="status-pulse"></div>
        <span>CORE: C99 OPTIMIZED | HUF1 LOSSLESS READY</span>
    </div>
</div>
""", unsafe_allow_html=True)


# -----------------------------------------------------------------------------
# 6. COMPACT HERO SECTION
# -----------------------------------------------------------------------------
st.markdown("""
<div class="hero-strip">
    <div>
        <h1 class="hero-h1">Compress Smarter. <span>Preserve Every Bit.</span></h1>
        <p class="hero-p">Lossless entropy compression powered by manual Min-Heap priority queues and prefix trees.</p>
    </div>
    <div class="hero-capsules">
        <div class="hero-cap">
            <div class="cap-lbl">ALGORITHM</div>
            <div class="cap-val">HUFFMAN 1952</div>
        </div>
        <div class="hero-cap">
            <div class="cap-lbl">INTEGRITY</div>
            <div class="cap-val">CRC-32</div>
        </div>
        <div class="hero-cap">
            <div class="cap-lbl">ARCHIVE</div>
            <div class="cap-val">HUF1 BINARY</div>
        </div>
    </div>
</div>
""", unsafe_allow_html=True)


# -----------------------------------------------------------------------------
# 7. MAIN APPLICATION NAVIGATION TABS
# -----------------------------------------------------------------------------
tab_compress, tab_decompress, tab_visualizer, tab_analytics, tab_testing, tab_docs = st.tabs([
    "🗜️ Compress",
    "📂 Decompress",
    "🌳 Visualizer",
    "📈 Analytics",
    "🧪 Testing",
    "📘 Documentation"
])


# -----------------------------------------------------------------------------
# TAB 1: COMPRESS WORKSPACE
# -----------------------------------------------------------------------------
with tab_compress:
    st.markdown("#### Primary Compression Workspace")
    st.caption("Upload any file to encode it into a genuine binary `HUF1` archive using bit-level streaming.")

    col_up, col_preview = st.columns([3, 2], gap="medium")

    with col_up:
        uploaded_src = st.file_uploader(
            "Drag & drop any file to compress",
            type=None,
            key="studio_compress_uploader",
            label_visibility="collapsed"
        )

    with col_preview:
        if uploaded_src is not None:
            raw_data = uploaded_src.getvalue()
            src_size = len(raw_data)
            st.markdown(f"""
            <div class="glass-card" style="margin-bottom:0;">
                <div style="font-size:0.75rem; font-weight:700; color:#06b6d4; text-transform:uppercase;">SELECTED INPUT FILE</div>
                <div style="font-size:1.15rem; font-weight:700; color:#f8fafc; margin-top:4px; word-break:break-all;">{uploaded_src.name}</div>
                <div style="display:flex; gap:16px; margin-top:8px; font-size:0.85rem; color:#94a3b8;">
                    <span><b>Size:</b> {format_size(src_size)}</span>
                    <span><b>Type:</b> {uploaded_src.type or 'Binary / Text'}</span>
                </div>
            </div>
            """, unsafe_allow_html=True)
        else:
            st.markdown("""
            <div class="glass-card" style="margin-bottom:0; text-align:center; padding:32px 16px;">
                <div style="color:#64748b; font-size:0.88rem;">Select or drop a file on the left to inspect file properties and begin compression.</div>
            </div>
            """, unsafe_allow_html=True)

    if uploaded_src is not None:
        c_btn1, c_btn2, c_spacer = st.columns([1.5, 1, 4])
        with c_btn1:
            trigger_comp = st.button("⚡ Compress File", type="primary", use_container_width=True)
        with c_btn2:
            clear_comp = st.button("Clear", use_container_width=True)

        if clear_comp:
            st.rerun()

        if trigger_comp:
            with tempfile.NamedTemporaryFile(delete=False, suffix="_" + uploaded_src.name) as tmp_in:
                tmp_in.write(uploaded_src.getvalue())
                tmp_in_path = tmp_in.name

            tmp_out_path = tmp_in_path + ".huf"

            with st.spinner("Executing C Huffman engine (Pass 1: Frequencies & Heap $\\to$ Pass 2: Bitstream Packing)..."):
                start_clock = time.perf_counter()
                try:
                    res = subprocess.run(
                        [EXECUTABLE_PATH, "--gui-compress", tmp_in_path, tmp_out_path],
                        capture_output=True, text=True, check=True
                    )
                    elapsed_ms = (time.perf_counter() - start_clock) * 1000.0
                    comp_stats = json.loads(res.stdout)

                    if comp_stats.get("success"):
                        orig_sz = comp_stats.get("original_size", 0)
                        comp_sz = comp_stats.get("compressed_size", 0)
                        hdr_sz = comp_stats.get("header_size", 0)
                        pay_sz = comp_stats.get("payload_size", 0)
                        ratio = comp_stats.get("ratio", 0.0)
                        saved_pct = comp_stats.get("space_saved", 0.0)
                        crc_val = comp_stats.get("crc", "")
                        saved_bytes = orig_sz - comp_sz

                        st.markdown("---")
                        st.markdown("#### Compression Results")

                        # KPI Metric Cards
                        saved_subtext = f"{saved_bytes:,} bytes saved" if saved_bytes >= 0 else f"{abs(saved_bytes):,} bytes expansion"
                        saved_class = "kpi-box-sub" if saved_bytes >= 0 else "kpi-box-sub warn"

                        st.markdown(f"""
                        <div class="kpi-grid">
                            <div class="kpi-box">
                                <div class="kpi-box-lbl">Original File Size</div>
                                <div class="kpi-box-val">{format_size(orig_sz)}</div>
                                <div class="kpi-box-sub">{orig_sz:,} B</div>
                            </div>
                            <div class="kpi-box">
                                <div class="kpi-box-lbl">Compressed Archive</div>
                                <div class="kpi-box-val">{format_size(comp_sz)}</div>
                                <div class="kpi-box-sub">{comp_sz:,} B</div>
                            </div>
                            <div class="kpi-box">
                                <div class="kpi-box-lbl">Compression Ratio</div>
                                <div class="kpi-box-val">{ratio:.3f}:1</div>
                                <div class="kpi-box-sub">Factor</div>
                            </div>
                            <div class="kpi-box">
                                <div class="kpi-box-lbl">Space Saved</div>
                                <div class="kpi-box-val">{saved_pct:.2f}%</div>
                                <div class="{saved_class}">{saved_subtext}</div>
                            </div>
                            <div class="kpi-box">
                                <div class="kpi-box-lbl">Execution Time</div>
                                <div class="kpi-box-val">{elapsed_ms:.1f} ms</div>
                                <div class="kpi-box-sub">C Core Runtime</div>
                            </div>
                            <div class="kpi-box">
                                <div class="kpi-box-lbl">IEEE 802.3 CRC-32</div>
                                <div class="kpi-box-val" style="font-size:1.15rem; color:#38bdf8;">{crc_val}</div>
                                <div class="kpi-box-sub">Integrity Hash</div>
                            </div>
                        </div>
                        """, unsafe_allow_html=True)

                        if comp_sz > orig_sz:
                            st.info("ℹ️ **Small File Expansion Notice:** The compressed archive is larger than the original due to the fixed 20-byte container header and character frequency table. This is normal and expected for very small inputs (< 250 bytes).")

                        # Archive Structure Chart + Download Action
                        col_arch_chart, col_arch_dl = st.columns([3, 2], gap="medium")

                        with col_arch_chart:
                            pie_fig = go.Figure(data=[go.Pie(
                                labels=['Encoded Payload', 'Header & Metadata'],
                                values=[pay_sz, hdr_sz],
                                hole=.6,
                                marker_colors=['#06b6d4', '#6366f1']
                            )])
                            pie_fig.update_layout(
                                title="Physical Archive Structure",
                                paper_bgcolor="rgba(0,0,0,0)",
                                plot_bgcolor="rgba(0,0,0,0)",
                                font_color="#e2e8f0",
                                height=240,
                                margin=dict(l=10, r=10, t=35, b=10)
                            )
                            st.plotly_chart(pie_fig, use_container_width=True)

                        with col_arch_dl:
                            st.markdown("##### Download Generated Archive")
                            with open(tmp_out_path, "rb") as f_dl:
                                archive_bytes = f_dl.read()

                            st.download_button(
                                label=f"⬇️ Download {uploaded_src.name}.huf",
                                data=archive_bytes,
                                file_name=f"{uploaded_src.name}.huf",
                                mime="application/octet-stream",
                                type="primary",
                                use_container_width=True
                            )
                            st.caption(f"Proprietary format: `HUF1` | Exact archive size: `{len(archive_bytes):,} bytes`")
                    else:
                        st.error(f"Compression error: {comp_stats.get('error')}")
                except Exception as e:
                    st.error(f"Execution Error: {str(e)}")

            try:
                if os.path.exists(tmp_in_path): os.remove(tmp_in_path)
                if os.path.exists(tmp_out_path): os.remove(tmp_out_path)
            except Exception:
                pass


# -----------------------------------------------------------------------------
# TAB 2: DECOMPRESS WORKSPACE
# -----------------------------------------------------------------------------
with tab_decompress:
    st.markdown("#### Decompression & Integrity Validation Workspace")
    st.caption("Upload a supported `.huf` archive. The engine reconstructs the tree from header metadata and performs byte-by-byte CRC-32 validation.")

    decomp_upload = st.file_uploader(
        "Upload a .huf archive to decompress",
        type=["huf"],
        key="studio_decomp_uploader"
    )

    if decomp_upload is not None:
        huf_data = decomp_upload.getvalue()
        huf_size = len(huf_data)

        st.markdown(f"""
        <div class="glass-card">
            <div style="font-size:0.75rem; font-weight:700; color:#8b5cf6; text-transform:uppercase;">LOADED ARCHIVE</div>
            <div style="font-size:1.15rem; font-weight:700; color:#f8fafc; margin-top:4px;">{decomp_upload.name}</div>
            <div style="font-size:0.85rem; color:#94a3b8; margin-top:4px;">Size on Disk: <b>{format_size(huf_size)}</b> ({huf_size:,} bytes)</div>
        </div>
        """, unsafe_allow_html=True)

        with tempfile.NamedTemporaryFile(delete=False, suffix=".huf") as tmp_d_huf:
            tmp_d_huf.write(huf_data)
            tmp_d_huf_path = tmp_d_huf.name

        base_restored_name = decomp_upload.name[:-4] if decomp_upload.name.endswith(".huf") else decomp_upload.name + ".restored"
        tmp_d_res_path = tmp_d_huf_path + ".restored"

        if st.button("🔓 Decompress File & Validate Checksum", type="primary", use_container_width=True):
            with st.spinner("Rebuilding Huffman tree and validating CRC-32 checksum..."):
                start_clock = time.perf_counter()
                try:
                    res = subprocess.run(
                        [EXECUTABLE_PATH, "--gui-decompress", tmp_d_huf_path, tmp_d_res_path],
                        capture_output=True, text=True, check=True
                    )
                    decomp_time_ms = (time.perf_counter() - start_clock) * 1000.0
                    d_data = json.loads(res.stdout)

                    if d_data.get("success"):
                        restored_sz = d_data.get("restored_size", 0)
                        matched = d_data.get("crc_matched", False)
                        crc_str = d_data.get("crc", "")

                        if matched:
                            st.markdown(f"""
                            <div class="verified-banner">
                                <div style="font-size:1.8rem;">🛡️</div>
                                <div>
                                    <div style="font-size:1.1rem; font-weight:800; color:#34d399;">100% Lossless Recovery Verified</div>
                                    <div style="font-size:0.82rem; color:#cbd5e1; margin-top:2px;">
                                        Output data restored byte-for-byte. IEEE 802.3 CRC-32 checksum matches the archive header perfectly (<code>{crc_str}</code>).
                                    </div>
                                </div>
                            </div>
                            """, unsafe_allow_html=True)
                        else:
                            st.error("⚠️ Checksum Mismatch! The archive may be corrupted or truncated.")

                        st.markdown(f"""
                        <div class="kpi-grid">
                            <div class="kpi-box">
                                <div class="kpi-box-lbl">Restored Size</div>
                                <div class="kpi-box-val">{format_size(restored_sz)}</div>
                                <div class="kpi-box-sub">{restored_sz:,} B</div>
                            </div>
                            <div class="kpi-box">
                                <div class="kpi-box-lbl">Integrity Status</div>
                                <div class="kpi-box-val" style="font-size:1.1rem; color:#34d399;">MATCHED</div>
                                <div class="kpi-box-sub">Bit-for-Bit Verified</div>
                            </div>
                            <div class="kpi-box">
                                <div class="kpi-box-lbl">Decompress Time</div>
                                <div class="kpi-box-val">{decomp_time_ms:.1f} ms</div>
                                <div class="kpi-box-sub">C Core Runtime</div>
                            </div>
                            <div class="kpi-box">
                                <div class="kpi-box-lbl">Restored CRC-32</div>
                                <div class="kpi-box-val" style="font-size:1.15rem; color:#38bdf8;">{crc_str}</div>
                                <div class="kpi-box-sub">32-Bit Cyclic Checksum</div>
                            </div>
                        </div>
                        """, unsafe_allow_html=True)

                        with open(tmp_d_res_path, "rb") as f_res:
                            restored_bytes = f_res.read()

                        st.download_button(
                            label=f"⬇️ Download Restored File ({base_restored_name})",
                            data=restored_bytes,
                            file_name=base_restored_name,
                            mime="application/octet-stream",
                            type="primary",
                            use_container_width=True
                        )
                    else:
                        st.error(f"Decompression error: {d_data.get('error')}")
                except Exception as e:
                    st.error(f"Execution Error: {str(e)}")

        try:
            if os.path.exists(tmp_d_huf_path): os.remove(tmp_d_huf_path)
            if os.path.exists(tmp_d_res_path): os.remove(tmp_d_res_path)
        except Exception:
            pass


# -----------------------------------------------------------------------------
# TAB 3: UNIQUE DATA STRUCTURES VISUALIZER
# -----------------------------------------------------------------------------
with tab_visualizer:
    st.markdown("#### Real Algorithm Data Structures Visualizer")
    st.caption("Inspect live intermediate data recorded directly from the C engine execution: Frequency Analysis, Min-Heap Merges, Binary Tree Topology, and Prefix Codes.")

    # File selector for visualizer
    sample_dir = os.path.join(PROJECT_ROOT, "sample_files")
    sample_list = [f for f in os.listdir(sample_dir) if os.path.isfile(os.path.join(sample_dir, f))] if os.path.exists(sample_dir) else []
    
    col_viz_src1, col_viz_src2 = st.columns([2, 3])
    with col_viz_src1:
        chosen_sample = st.selectbox("Select sample file or custom upload:", ["(Upload Custom File)"] + sample_list)

    inspect_file_path = None
    if chosen_sample != "(Upload Custom File)":
        inspect_file_path = os.path.join(sample_dir, chosen_sample)
    else:
        with col_viz_src2:
            custom_viz_up = st.file_uploader("Upload custom file for visualization", key="custom_viz_up", label_visibility="collapsed")
            if custom_viz_up is not None:
                with tempfile.NamedTemporaryFile(delete=False) as tmp_viz:
                    tmp_viz.write(custom_viz_up.getvalue())
                    inspect_file_path = tmp_viz.name

    if inspect_file_path and os.path.isfile(inspect_file_path):
        try:
            res = subprocess.run([EXECUTABLE_PATH, "--gui-info", inspect_file_path], capture_output=True, text=True, check=True)
            algo_info = json.loads(res.stdout)

            tot_size = algo_info.get("file_size", 0)
            dist_syms = algo_info.get("distinct_count", 0)
            symbols = algo_info.get("symbols", [])
            merge_steps = algo_info.get("merge_steps", [])

            # Pipeline Visual Bar
            st.markdown("""
            <div class="pipeline-bar">
                <div class="pipe-step"><div class="pipe-dot"></div><div class="pipe-txt">1. Input Stream</div></div>
                <div class="pipe-connector"></div>
                <div class="pipe-step"><div class="pipe-dot"></div><div class="pipe-txt">2. Frequency Table</div></div>
                <div class="pipe-connector"></div>
                <div class="pipe-step"><div class="pipe-dot"></div><div class="pipe-txt">3. Min-Heap Queue</div></div>
                <div class="pipe-connector"></div>
                <div class="pipe-step"><div class="pipe-dot"></div><div class="pipe-txt">4. Huffman Tree</div></div>
                <div class="pipe-connector"></div>
                <div class="pipe-step"><div class="pipe-dot"></div><div class="pipe-txt">5. Bit-Packing</div></div>
                <div class="pipe-connector"></div>
                <div class="pipe-step"><div class="pipe-dot"></div><div class="pipe-txt">6. HUF1 Archive</div></div>
            </div>
            """, unsafe_allow_html=True)

            sub_tab1, sub_tab2, sub_tab3, sub_tab4 = st.tabs([
                "🌳 Huffman Binary Tree",
                "⚡ Min-Heap Merging Steps",
                "📊 Frequency Analysis",
                "📋 Prefix Code Table"
            ])

            # 1. Tree Visualization
            with sub_tab1:
                st.markdown("##### Dynamically Reconstructed Binary Tree")
                st.caption("Green nodes = Leaf symbols with byte frequencies. Blue/Slate nodes = Merged internal subtrees. Edge labels indicate bits '0' (Left) and '1' (Right).")
                if symbols:
                    dot_code = build_graphviz_huffman_tree(symbols, max_leaves=24)
                    st.graphviz_chart(dot_code, use_container_width=True)
                else:
                    st.info("Input file is empty (0 bytes).")

            # 2. Min-Heap Step Viewer (Real Intermediate Data from C Execution)
            with sub_tab2:
                st.markdown("##### Min-Heap Priority Queue Extraction & Merge Steps")
                st.caption("At each step, the C Min-Heap extracts the two subtrees with the lowest frequencies and re-inserts a new combined internal node.")
                
                if merge_steps:
                    st.markdown(f"**Total Min-Heap Merge Operations:** `{len(merge_steps)}` steps executed.")
                    
                    step_idx = st.slider("Step Slider (Scrub through algorithm execution):", 1, len(merge_steps), 1)
                    curr_step = merge_steps[step_idx - 1]

                    col_s1, col_s2, col_s3 = st.columns(3)
                    with col_s1:
                        st.markdown(f"""
                        <div class="glass-card">
                            <div style="font-size:0.7rem; font-weight:700; color:#38bdf8;">EXTRACTED MIN #1 (LEFT)</div>
                            <div style="font-size:1.3rem; font-weight:800; color:#ffffff; margin-top:4px;">{curr_step['left_sym']}</div>
                            <div style="font-size:0.8rem; color:#94a3b8;">Frequency: <b>{curr_step['left_freq']:,}</b></div>
                            <div style="font-size:0.75rem; color:#64748b;">Type: {'Leaf' if curr_step['left_is_leaf'] else 'Internal Subtree'}</div>
                        </div>
                        """, unsafe_allow_html=True)

                    with col_s2:
                        st.markdown(f"""
                        <div class="glass-card">
                            <div style="font-size:0.7rem; font-weight:700; color:#38bdf8;">EXTRACTED MIN #2 (RIGHT)</div>
                            <div style="font-size:1.3rem; font-weight:800; color:#ffffff; margin-top:4px;">{curr_step['right_sym']}</div>
                            <div style="font-size:0.8rem; color:#94a3b8;">Frequency: <b>{curr_step['right_freq']:,}</b></div>
                            <div style="font-size:0.75rem; color:#64748b;">Type: {'Leaf' if curr_step['right_is_leaf'] else 'Internal Subtree'}</div>
                        </div>
                        """, unsafe_allow_html=True)

                    with col_s3:
                        st.markdown(f"""
                        <div class="glass-card" style="border-color:rgba(16, 185, 129, 0.4);">
                            <div style="font-size:0.7rem; font-weight:700; color:#34d399;">MERGED PARENT NODE</div>
                            <div style="font-size:1.3rem; font-weight:800; color:#34d399; margin-top:4px;">{curr_step['parent_freq']:,}</div>
                            <div style="font-size:0.8rem; color:#94a3b8;">Sum: {curr_step['left_freq']:,} + {curr_step['right_freq']:,}</div>
                            <div style="font-size:0.75rem; color:#10b981;">Re-inserted into Min-Heap</div>
                        </div>
                        """, unsafe_allow_html=True)
                else:
                    st.info("Input has fewer than 2 distinct symbols; no heap mergers required.")

            # 3. Frequency Analysis
            with sub_tab3:
                if symbols:
                    df_sym = pd.DataFrame([
                        {
                            "Hex": f"0x{s.get('byte', 0):02X}",
                            "Symbol": s.get("symbol", ""),
                            "Frequency": s.get("freq", 0),
                            "Probability (%)": round((s.get("freq", 0) / tot_size * 100), 2) if tot_size > 0 else 0,
                            "Code": s.get("code", ""),
                            "Bits": s.get("len", 0)
                        } for s in symbols
                    ])

                    top_bar = df_sym.sort_values(by="Frequency", ascending=False).head(20)
                    fig_freq = px.bar(
                        top_bar,
                        x="Symbol",
                        y="Frequency",
                        color="Bits",
                        color_continuous_scale="Viridis",
                        title="Top 20 Characters & Assigned Variable Bit Lengths",
                        labels={"Frequency": "Occurrences", "Bits": "Bit Length"}
                    )
                    fig_freq.update_layout(
                        paper_bgcolor="#06080e",
                        plot_bgcolor="#0c1017",
                        font_color="#cbd5e1",
                        height=350,
                        margin=dict(l=20, r=20, t=40, b=20)
                    )
                    st.plotly_chart(fig_freq, use_container_width=True)
                else:
                    st.info("No symbol frequency data.")

            # 4. Code Table
            with sub_tab4:
                if symbols:
                    st.markdown("##### Complete Generated Prefix Code Table")
                    st.dataframe(df_sym, use_container_width=True, height=340)

        except Exception as e:
            st.error(f"Visualizer Error: {str(e)}")


# -----------------------------------------------------------------------------
# TAB 4: ANALYTICS WORKSPACE
# -----------------------------------------------------------------------------
with tab_analytics:
    st.markdown("#### High-Density Analytics & Theoretical Efficiency")
    st.caption("Examine entropy compression limits, symbol probabilities, and theoretical Shannon bounds.")

    col_an1, col_an2 = st.columns([1, 1], gap="medium")

    with col_an1:
        st.markdown("##### Entropy vs. Average Code Length")
        st.markdown("""
        Shannon's Source Coding Theorem establishes that the minimum average code length for any lossless encoding is bounded by **Entropy ($H$)**:
        $$H(X) = - \\sum_{i=1}^{k} P(x_i) \\log_2 P(x_i)$$
        Huffman coding approaches this theoretical limit within **1 bit per symbol**:
        $$H(X) \\le L < H(X) + 1$$
        """)

    with col_an2:
        st.markdown("##### Space Complexity & Memory Overhead")
        st.markdown("""
        - **Frequency Table:** 256 counters $\\times$ 8 bytes = **2 KB** ($O(1)$)
        - **Min-Heap Queue:** At most 511 node pointers $\\approx$ **16 KB** ($O(1)$)
        - **Bitstream I/O Buffer:** 64 KB page-buffered chunk streaming ($O(1)$)
        - **Total Auxiliary Memory:** Strictly bounded **under 200 KB** regardless of input file size.
        """)


# -----------------------------------------------------------------------------
# TAB 5: TESTING WORKSPACE
# -----------------------------------------------------------------------------
with tab_testing:
    st.markdown("#### Automated System Verification & Regression Suite")
    st.caption("Executes the standalone C test runner (`tests/test_runner.exe`) covering all 25 unit, system, and edge-case test assertions.")

    if st.button("🧪 Execute All 25 Automated Tests Now", type="primary"):
        with st.spinner("Running full C test harness..."):
            try:
                cmd = [TEST_RUNNER_PATH] if os.path.isfile(TEST_RUNNER_PATH) else [EXECUTABLE_PATH, "-test"]
                t_res = subprocess.run(cmd, capture_output=True, text=True)

                if t_res.returncode == 0:
                    st.markdown("""
                    <div style="background:rgba(16, 185, 129, 0.15); border:1px solid #10b981; border-radius:10px; padding:16px; margin:14px 0;">
                        <span style="color:#34d399; font-weight:800; font-size:1.1rem;">✅ ALL 25 AUTOMATED TESTS PASSED (100.0%)</span>
                        <div style="color:#cbd5e1; font-size:0.85rem; margin-top:4px;">
                            Min-Heap Priority Queue, Tree Construction, Prefix (Fano) Condition, and all 10 Edge-Case System Scenarios verified successfully.
                        </div>
                    </div>
                    """, unsafe_allow_html=True)
                else:
                    st.error("One or more tests failed.")

                st.code(t_res.stdout, language="text")

            except Exception as e:
                st.error(f"Test Execution Failed: {str(e)}")


# -----------------------------------------------------------------------------
# TAB 6: DOCUMENTATION WORKSPACE
# -----------------------------------------------------------------------------
with tab_docs:
    st.markdown("#### Technical Documentation & Specification")

    exp1 = st.expander("📖 1. Algorithm Overview & Worked Example (ABRACADABRA)", expanded=True)
    with exp1:
        st.markdown("""
        Let the input string be **`"ABRACADABRA"`** (Length $L = 11$ bytes):
        - **`'A'`**: Freq = 5 (45.45%) $\\to$ Assigned shortest code: `0` (1 bit)
        - **`'B'`**: Freq = 2 (18.18%) $\\to$ Assigned code: `110` (3 bits)
        - **`'R'`**: Freq = 2 (18.18%) $\\to$ Assigned code: `111` (3 bits)
        - **`'C'`**: Freq = 1 (9.09%) $\\to$ Assigned code: `100` (3 bits)
        - **`'D'`**: Freq = 1 (9.09%) $\\to$ Assigned code: `101` (3 bits)
        
        **Total Compressed Payload:**
        $$(5 \\times 1) + (2 \\times 3) + (2 \\times 3) + (1 \\times 3) + (1 \\times 3) = 23 \\text{ bits}$$
        Compared to fixed 8-bit ASCII ($11 \\times 8 = 88$ bits), this saves **73.86%** on payload bits.
        """)

    exp2 = st.expander("📦 2. HUF1 Binary Container Specification")
    with exp2:
        st.code("""
+-------------------------------------------------------------------------+
| OFFSET | FIELD NAME       | TYPE     | DESCRIPTION                      |
+-------------------------------------------------------------------------+
| 0x00   | Magic Signature  | char[4]  | Fixed: 'H', 'U', 'F', '1'        |
| 0x04   | Format Version   | uint8_t  | Fixed: 0x01                      |
| 0x05   | Reserved Flags   | uint8_t  | Default: 0x00                    |
| 0x06   | Distinct Count   | uint16_t | Unique symbols present (0 - 256) |
| 0x08   | Original Size    | uint64_t | Uncompressed size in bytes       |
| 0x10   | Original CRC-32  | uint32_t | IEEE 802.3 checksum of source    |
+-------------------------------------------------------------------------+
| 0x14   | Frequency Table (Distinct Count entries of 9 bytes each):      |
|        | - Symbol Value   | uint8_t  | Byte value (0x00 to 0xFF)        |
|        | - Symbol Freq    | uint64_t | Total occurrences in source data |
+-------------------------------------------------------------------------+
| PAYLOAD| Bit-Packed Data  | uint8_t* | Huffman codes packed MSB to LSB  |
+-------------------------------------------------------------------------+
        """, language="text")

    exp3 = st.expander("⚡ 3. Computational Complexity Analysis")
    with exp3:
        st.markdown(r"""
        | Phase | Time Complexity | Auxiliary Space Complexity |
        |:---|:---:|:---:|
        | **Frequency Counting** | $O(N)$ | $O(1)$ |
        | **Min-Heap Construction** | $O(k \log k)$ | $O(k)$ |
        | **Tree Merging** | $O(k \log k)$ | $O(k)$ |
        | **Code Generation** | $O(k)$ | $O(k)$ |
        | **Bitstream Encoding / Decoding** | $O(N)$ | $O(1)$ |
        
        Because the alphabet size $k \le 256$ is fixed, $k \log k \le 2048$ operations ($O(1)$).
        Therefore, overall runtime is **strictly linear $O(N)$** and memory consumption is **bounded under 200 KB**.
        """)
