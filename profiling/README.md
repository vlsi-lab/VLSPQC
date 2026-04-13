# Profiling

This folder contains **instrumented versions** of Post-Quantum Cryptography algorithms for function call profiling and performance analysis.

---

## 📁 Contents

| Folder/File | Description |
|-------------|-------------|
| `HQC-1/` | HQC-1 profiling implementation |
| `ML-DSA-2/` | ML-DSA-2 (Dilithium) profiling implementation |
| `ml-kem-512/` | ML-KEM-512 (Kyber) profiling implementation |
| `falcon-512/` | Falcon-512 profiling implementation |
| `SPHINCS+-128f-robust/` | SPHINCS+-128f-robust profiling implementation |
| `SPHINCS+-128f-simple/` | SPHINCS+-128f-simple profiling implementation |
| `pqc_profiling_analysis.md` | Comprehensive analysis of function call profiling results |
| `run_test.sh` | Interactive test launcher script |
| `*.txt` | Raw profiling output files for each algorithm |

---

## 🚀 Running Profiling Tests

### Interactive Mode

Run the test launcher script without arguments to use the interactive menu:

```bash
cd profiling
./run_test.sh
```

Or from the repository root:

```bash
./profiling/run_test.sh
```

The script will guide you through:
1. **Category selection**: `common` (original implementations) or `profiled` (instrumented versions)
2. **Algorithm family**: ML-KEM, ML-DSA, HQC, FALCON, SLH-DSA
3. **Variant selection**: specific parameter set (e.g., ml-kem-512)

### Command-Line Mode

You can also run tests directly by specifying arguments:

```bash
./run_test.sh <CATEGORY> <FAMILY> [TEST_NUMBER]
```

**Parameters:**
- `CATEGORY`: `common` | `profiled`
- `FAMILY`: Algorithm family name (e.g., `ML-KEM`, `FALCON`, `HQC`, `ML-DSA`, `SLH-DSA`)
- `TEST_NUMBER`: 1-based index of variant within the chosen family (optional)

**Examples:**

```bash
# Run profiled ML-KEM-512
./run_test.sh profiled ML-KEM 1

# Run profiled Falcon-512
./run_test.sh profiled FALCON 1

# Run profiled HQC-1
./run_test.sh profiled HQC 1

# Run profiled ML-DSA-2
./run_test.sh profiled ML-DSA 1

# Run profiled SPHINCS+-128f-simple
./run_test.sh profiled SLH-DSA 2
```

---

## 🔧 Building Individual Tests

Each algorithm has its own `Makefile`. To build and run manually:

```bash
cd <algorithm-folder>    # e.g., cd ml-kem-512
make clean
make
./<executable-name>      # e.g., ./ml-kem-512
```

---

## 📊 Profiling Results

The profiling output shows function call counts for each phase of the cryptographic protocol:
- **KeyGen**: Key generation
- **Encaps/Sign**: Encapsulation (KEM) or Signing (signature schemes)
- **Decaps/Verify**: Decapsulation (KEM) or Verification (signature schemes)

See [`pqc_profiling_analysis.md`](pqc_profiling_analysis.md) for a comprehensive analysis of profiling results across all algorithms, including:
- Function call frequency tables
- Bottleneck identification
- Cross-algorithm comparisons

---

## 📄 Raw Profiling Outputs

Raw profiling data for each algorithm is available in the corresponding `.txt` files:
- `falcon.txt` - Falcon-512 profiling output
- `hqc.txt` - HQC-1 profiling output
- `mldsa.txt` - ML-DSA-2 profiling output
- `mlkem.txt` - ML-KEM-512 profiling output
- `sphincs-robust.txt` - SPHINCS+-128f-robust profiling output
- `sphincs-simple.txt` - SPHINCS+-128f-simple profiling output
