# Algorithm Explanation: Lossless Huffman Coding

**Course:** Computer Engineering — Data Structures and Algorithms  
**Project Title:** Huffman Coding-Based File Compression System  
**Student Name:** Shalom Dabre  
**Roll Number:** 11070  

---

## 1. Introduction to Huffman Coding

Huffman Coding is a renowned entropy encoding algorithm invented by **David A. Huffman** in 1952 while completing his Ph.D. at MIT. It solves the fundamental problem of finding an **optimal prefix-free binary code** for a given set of symbols with known frequencies of occurrence.

### 1.1 The Core Idea
Standard text representations (such as ASCII) assign a fixed width of 8 bits (1 byte) to every character. In ASCII, the letter `'e'`, which appears in almost every English sentence, consumes the same 8 bits as the rarely used symbol `'~'` or `'{'`.

Huffman coding utilizes a **variable-length prefix code**:
1. Characters that occur **frequently** are assigned **shorter** binary sequences (e.g., 2 to 4 bits).
2. Characters that occur **infrequently** are assigned **longer** binary sequences (e.g., 8 to 12 bits).
3. The overall weighted average code length is minimized, resulting in substantial file size reduction.

### 1.2 The Prefix Property (Prefix-Free Code)
A code is **prefix-free** if no valid code word is a prefix of any other valid code word.
- Example of an invalid prefix code:
  - `'A' = 0`
  - `'B' = 01`  *(Invalid: code for 'A' is a prefix of 'B'! If the bitstream begins with `01`, the decoder cannot tell if it is `'A'` followed by something else, or `'B'`)*.
- Example of a valid Huffman code:
  - `'A' = 0`
  - `'B' = 10`
  - `'C' = 110`
  - `'D' = 111`
  *(No code begins with another code. The bitstream `0101100` unambiguously decodes to `A-B-C-A` without needing delimiters or spaces!)*

---

## 2. Theoretical Foundations: Greedy Choice & Optimal Substructure

Huffman coding belongs to the class of **Greedy Algorithms**. It builds the optimal solution bottom-up by making a series of locally optimal choices:

1. **Greedy Choice Property:** At each step, merge the two subtrees with the lowest total frequencies. This locally optimal decision is guaranteed to lead to a globally optimal prefix tree.
2. **Optimal Substructure:** An optimal tree for an alphabet of $N$ symbols contains within it the optimal tree for the $(N - 1)$ symbols formed by merging the two lowest-frequency symbols into a single composite symbol.

---

## 3. Step-by-Step Worked Example & Dry Run

To illustrate the exact mechanics of the algorithm as implemented in our C codebase, let us perform a complete dry run for the input string:

$$\text{"ABRACADABRA"}$$

### Step 1: Frequency Analysis
Let us count the occurrences of each unique character in `"ABRACADABRA"` (total length $L = 11$ characters):

| Character | ASCII Value | Frequency | Percentage of Input |
|:---------:|:-----------:|:---------:|:-------------------:|
| `'A'`     | 65          | 5         | 45.45%              |
| `'B'`     | 66          | 2         | 18.18%              |
| `'R'`     | 82          | 2         | 18.18%              |
| `'C'`     | 67          | 1         | 9.09%               |
| `'D'`     | 68          | 1         | 9.09%               |

*Total distinct symbols:* $k = 5$.

---

### Step 2: Min-Heap Initialization
For each distinct symbol, allocate a leaf node and insert it into our manually implemented Min-Heap. The priority key is the node's **frequency**:

```text
Initial Min-Heap state:
  [ 'C' (1), 'D' (1), 'B' (2), 'R' (2), 'A' (5) ]
```

---

### Step 3: Iterative Tree Construction (Greedy Merging)

#### Iteration 1:
- Extract minimum #1: Node `'C'` (freq = 1)
- Extract minimum #2: Node `'D'` (freq = 1)
- Create new internal node $INT_1$:
  - Left child: `'C'`
  - Right child: `'D'`
  - Combined frequency: $1 + 1 = 2$
- Insert $INT_1$ (freq = 2) into the Min-Heap.
- Heap state: `[ 'B' (2), 'R' (2), INT_1 (2), 'A' (5) ]`

#### Iteration 2:
- Extract minimum #1: Node `'B'` (freq = 2)
- Extract minimum #2: Node `'R'` (freq = 2)
- Create new internal node $INT_2$:
  - Left child: `'B'`
  - Right child: `'R'`
  - Combined frequency: $2 + 2 = 4$
- Insert $INT_2$ (freq = 4) into the Min-Heap.
- Heap state: `[ INT_1 (2), INT_2 (4), 'A' (5) ]`

#### Iteration 3:
- Extract minimum #1: Node $INT_1$ (freq = 2)
- Extract minimum #2: Node $INT_2$ (freq = 4)
- Create new internal node $INT_3$:
  - Left child: $INT_1$ (freq = 2)
  - Right child: $INT_2$ (freq = 4)
  - Combined frequency: $2 + 4 = 6$
- Insert $INT_3$ (freq = 6) into the Min-Heap.
- Heap state: `[ 'A' (5), INT_3 (6) ]`

#### Iteration 4:
- Extract minimum #1: Node `'A'` (freq = 5)
- Extract minimum #2: Node $INT_3$ (freq = 6)
- Create new internal node $ROOT$:
  - Left child: `'A'` (freq = 5)
  - Right child: $INT_3$ (freq = 6)
  - Combined frequency: $5 + 6 = 11$
- Insert $ROOT$ into the Min-Heap.
- Heap size is now 1. The construction loop terminates!

---

### Step 4: Final Huffman Tree Topology

```text
                  ROOT (11)
                 /         \
            0   /           \   1
               /             \
            'A' (5)         INT_3 (6)
                           /         \
                      0   /           \   1
                         /             \
                   INT_1 (2)         INT_2 (4)
                   /       \         /       \
              0   /     1   \   0   /     1   \
                 /           \     /           \
              'C' (1)     'D' (1) 'B' (2)     'R' (2)
```

---

### Step 5: Generating the Huffman Prefix Codes
Traverse from the root to each leaf node. Moving left appends a `'0'`, moving right appends a `'1'`:

| Character | Tree Traversal Path | Huffman Code | Bit Length | Fixed ASCII Bits | Bits Saved per Char |
|:---------:|:-------------------:|:------------:|:----------:|:----------------:|:-------------------:|
| `'A'`     | Left                | `0`          | 1 bit      | 8 bits           | 7 bits (87.5%)      |
| `'C'`     | Right -> Left -> Left  | `100`        | 3 bits     | 8 bits           | 5 bits (62.5%)      |
| `'D'`     | Right -> Left -> Right | `101`        | 3 bits     | 8 bits           | 5 bits (62.5%)      |
| `'B'`     | Right -> Right -> Left | `110`        | 3 bits     | 8 bits           | 5 bits (62.5%)      |
| `'R'`     | Right -> Right -> Right| `111`        | 3 bits     | 8 bits           | 5 bits (62.5%)      |

---

### Step 6: Encoding the Input String `"ABRACADABRA"`

Substitute each character with its generated bit code:

```text
Input text :  A   B     R     A   C     A   D     A   B     R     A
Codes      :  0   110   111   0   100   0   101   0   110   111   0
```

Concatenated bitstream:
```text
01101110100010101101110
```

Total bits in compressed payload:
$$(5 \times 1) + (2 \times 3) + (2 \times 3) + (1 \times 3) + (1 \times 3) = 5 + 6 + 6 + 3 + 3 = 23 \text{ bits}$$

Comparison with Uncompressed ASCII:
- Uncompressed: $11 \text{ characters} \times 8 \text{ bits/char} = 88 \text{ bits}$ (11 bytes).
- Compressed payload: $23 \text{ bits} = 2 \text{ bytes and } 7 \text{ bits}$ (padded to 3 bytes).
- Payload compression ratio: $\frac{88}{24} \approx 3.66 : 1$.
- Payload space saved: $\frac{88 - 24}{88} \times 100\% \approx 72.7\%$.

---

### Step 7: Bit Packing into 8-Bit Bytes
Computers write to disk in multiples of bytes (8 bits). Our `BitWriter` packs bits from Most Significant Bit (MSB) to Least Significant Bit (LSB):

```text
Bit stream: 01101110 10001010 1101110[0]  (1 bit of zero-padding at the end)

Byte 0: 0 1 1 0 1 1 1 0  -> 0x6E (ASCII 'n')
Byte 1: 1 0 0 0 1 0 1 0  -> 0x8A
Byte 2: 1 1 0 1 1 1 0 0  -> 0xDC (Padded with 1 zero bit)
```

---

### Step 8: Decompression & Decoding Dry Run
During decompression, the decoder reads the header, recovers the exact original size ($11$) and character frequencies, and rebuilds the exact same Huffman tree.

The decoder then reads the bits one by one from the bitstream, starting at the root:

```text
Read '0' : Branch LEFT  -> Reached leaf 'A'! Output 'A'. Reset to root. (Count = 1)
Read '1' : Branch RIGHT -> Node INT_3
Read '1' : Branch RIGHT -> Node INT_2
Read '0' : Branch LEFT  -> Reached leaf 'B'! Output 'B'. Reset to root. (Count = 2)
Read '1' : Branch RIGHT -> Node INT_3
Read '1' : Branch RIGHT -> Node INT_2
Read '1' : Branch RIGHT -> Reached leaf 'R'! Output 'R'. Reset to root. (Count = 3)
Read '0' : Branch LEFT  -> Reached leaf 'A'! Output 'A'. Reset to root. (Count = 4)
Read '1' : Branch RIGHT -> Node INT_3
Read '0' : Branch LEFT  -> Node INT_1
Read '0' : Branch LEFT  -> Reached leaf 'C'! Output 'C'. Reset to root. (Count = 5)
Read '0' : Branch LEFT  -> Reached leaf 'A'! Output 'A'. Reset to root. (Count = 6)
Read '1' : Branch RIGHT -> Node INT_3
Read '0' : Branch LEFT  -> Node INT_1
Read '1' : Branch RIGHT -> Reached leaf 'D'! Output 'D'. Reset to root. (Count = 7)
Read '0' : Branch LEFT  -> Reached leaf 'A'! Output 'A'. Reset to root. (Count = 8)
Read '1' : Branch RIGHT -> Node INT_3
Read '1' : Branch RIGHT -> Node INT_2
Read '0' : Branch LEFT  -> Reached leaf 'B'! Output 'B'. Reset to root. (Count = 9)
Read '1' : Branch RIGHT -> Node INT_3
Read '1' : Branch RIGHT -> Node INT_2
Read '1' : Branch RIGHT -> Reached leaf 'R'! Output 'R'. Reset to root. (Count = 10)
Read '0' : Branch LEFT  -> Reached leaf 'A'! Output 'A'. Reset to root. (Count = 11)
```

Since the decoded byte count has reached $11$ (the `original_size` stored in the file header), **decoding stops immediately**! The trailing padding bit (`0`) in Byte 2 is safely ignored without any ambiguity.

Restored output: `"ABRACADABRA"`. Byte-for-byte exact recovery!

---

## 4. Complexity Analysis

| Phase | Time Complexity | Auxiliary Space Complexity | Explanation |
|:---|:---:|:---:|:---|
| **Frequency Counting** | $O(N)$ | $O(1)$ | Single linear scan of $N$ input bytes; constant 256 array counters. |
| **Min-Heap Construction** | $O(k \log k)$ | $O(k)$ | Inserting $k$ distinct symbols into the heap ($k \le 256$). |
| **Huffman Tree Build** | $O(k \log k)$ | $O(k)$ | Exactly $(k - 1)$ extract-min operations and insertions in the heap. |
| **Code Generation** | $O(k)$ | $O(k)$ | DFS traversal of binary tree with $(2k - 1)$ total nodes. |
| **Bitstream Encoding** | $O(N)$ | $O(1)$ | Streaming pass over $N$ bytes with $O(1)$ table lookups per byte. |
| **Decompression** | $O(N)$ | $O(k)$ | Tree traversal consuming 1 bit per branch, outputting $N$ bytes. |

Since $k \le 256$ is bounded by the fixed size of an 8-bit byte alphabet:
- Building the tree takes at most $256 \log 256 \approx 2048$ operations, which is $O(1)$ constant time!
- Overall runtime is strictly **linear $O(N)$** with respect to the input file size.
