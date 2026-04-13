# Kyber gen_matrix Test Suite

Complete test benches for **Kyber matrix generation** (gen_a and gen_at) with cycle counting and golden reference validation.

## Overview

This test suite validates the correctness and measures the performance of Kyber's matrix generation function, which:

1. Uses SHAKE128 XOF (Extended Output Function) for deterministic randomness
2. Applies rejection sampling to generate uniform random coefficients modulo KYBER_Q
3. Generates a 2×2 matrix of polynomials (for Kyber-512)

The suite includes both **software reference** and **hardware-optimized** implementations.

## Directory Structure

```
gen-matrix-sw/              # Software reference implementation
├── main.c                  # Test harness (SW version)
├── gen_matrix.c            # Core algorithm (pure C)
├── kyber_params.h          # Kyber-512 parameters
├── fips202.h/c             # FIPS 202 Keccak interface
└── GOLDEN_REFERENCE.md     # Expected output values

gen-matrix-hw/              # Hardware-optimized implementation
├── main.c                  # Test harness (HW version)
├── gen_matrix.c            # Core algorithm (HW-optimized)
├── kyber_params.h          # Kyber-512 parameters  
├── fips202.h/c             # FIPS 202 Keccak interface
└── GOLDEN_REFERENCE.md     # Expected output values
```

## Key Features

### ✅ Correctness Validation
- **Golden reference testing**: Compares against pre-computed reference values
- **Bit-identical verification**: Ensures deterministic output
- **Transposition correctness**: Verifies gen_a vs gen_at relationship

### ✅ Performance Measurement
- **Cycle counting**: Uses RISC-V MCYCLE CSR
- **Instruction counting**: Uses RISC-V MINSTRET CSR
- **CPI calculation**: Cycles per instruction ratio
- **Multi-level analysis**: Both single-run and cumulative performance

### ✅ Detailed Metrics
- Total cycles per operation
- Instruction-level breakdown
- Performance per coefficient
- Memory I/O characterization

## Test Configuration

All tests use the same fixed seed for reproducibility:

```c
TEST_SEED: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
           10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
```

### Kyber-512 Parameters
- **KYBER_N**: 256 (polynomial degree)
- **KYBER_K**: 2 (matrix dimension: 2×2)
- **KYBER_Q**: 3329 (coefficient modulus)
- **XOF_BLOCKBYTES**: 168 (SHAKE128 rate)

## Test Cases

### Test 1: Basic Functionality (gen_a)
```
Description: Generate non-transposed matrix A
Input:       TEST_SEED
Expected:    Matches GOLDEN_GEN_A reference values
Metrics:     Cycles, instructions, CPI
```

### Test 2: Basic Functionality (gen_at)
```
Description: Generate transposed matrix A^T
Input:       TEST_SEED
Expected:    Matches GOLDEN_GEN_AT reference values
Metrics:     Cycles, instructions, CPI
```

### Test 3: Transposition Property
```
Description: Verify gen_at = transpose(gen_a)
Expected:    at[i][j] == a[j][i] for all coefficients
Metrics:     Cycle consistency between implementations
```

### Test 4: Deterministic Seeding
```
Description: Same seed produces identical output
Expected:    Multiple runs yield bit-identical results
Metrics:     Cycle consistency across runs
```

### Test 5: Different Seeds
```
Description: Different seeds produce different matrices
Expected:    Seed1 output ≠ Seed2 output
Metrics:     Entropy verification
```

### Test 6 (HW only): Multi-Block XOF Performance
```
Description: Measure hardware acceleration benefits
Expected:    State stays in registers between squeeze operations
Metrics:     Memory transaction reduction factor
```

### Test 7 (HW only): Rejection Sampling Efficiency
```
Description: Analyze XOF utilization
Expected:    ~3 bytes per 2 valid coefficients extracted
Metrics:     Cycle per coefficient ratio
```

## Golden Reference Values

Pre-computed golden values for TEST_SEED:

### gen_a (Non-Transposed)
```
a[0][0] first 4: [1206, 3253, 1302, 2066]
a[0][1] first 4: [2193, 1906, 1915, 1151]
a[1][0] first 4: [1449, 2127, 2357, 2849]
a[1][1] first 4: [1893, 1189, 2566, 2521]
```

### gen_at (Transposed)
```
at[0][0] first 4: [1206, 3253, 1302, 2066]  # Same as a[0][0]
at[0][1] first 4: [1449, 2127, 2357, 2849]  # Swapped: = a[1][0]
at[1][0] first 4: [2193, 1906, 1915, 1151]  # Swapped: = a[0][1]
at[1][1] first 4: [1893, 1189, 2566, 2521]  # Same as a[1][1]
```

See `GOLDEN_REFERENCE.md` for complete documentation.

## Expected Performance

### Software Version (Pure C)
- **gen_a()**: ~3000-4000 cycles
- **gen_at()**: ~3000-4000 cycles
- **CPI**: ~1.5-2.0

### Hardware-Optimized Version (Granular Instructions)
- **gen_a()**: ~1000-1500 cycles  
- **gen_at()**: ~1000-1500 cycles
- **Speedup**: **2-3×** vs software
- **CPI**: ~1.0-1.5

### Speedup Origins
1. **State persistence**: Keccak state remains in HW registers between XOF squeeze blocks
2. **Reduced I/O**: Only load at start, store at end (vs SW: load/store every iteration)
3. **Instruction reduction**: HW absorb_xor/permute more efficient than SW loops

## Compilation & Linking

### Software Version
```bash
cd gen-matrix-sw
gcc -O2 -march=rv32imac -c main.c -o main.o
gcc -O2 -march=rv32imac -c gen_matrix.c -o gen_matrix.o
gcc -O2 -march=rv32imac -c kyber_params.c -o kyber_params.o
# Link with Keccak implementation
gcc main.o gen_matrix.o kyber_params.o -L../keccak-abs-sw -lfips202_sw -o gen_matrix_sw
```

### Hardware-Optimized Version
```bash
cd gen-matrix-hw
gcc -O2 -march=rv32imac -c main.c -o main.o
gcc -O2 -march=rv32imac -c gen_matrix.c -o gen_matrix.o
gcc -O2 -march=rv32imac -c kyber_params.c -o kyber_params.o
# Link with HW-optimized Keccak implementation
gcc main.o gen_matrix.o kyber_params.o -L../keccak-abs-hw -lfips202_hw -o gen_matrix_hw
```

## Running Tests

### Software Implementation
```bash
./gen_matrix_sw
```

Expected output:
```
╔═══════════════════════════════════════════════════════════════╗
║     Kyber gen_matrix Test Suite (SOFTWARE VERSION)            ║
║     Tests matrix A generation with XOF sampling               ║
╚═══════════════════════════════════════════════════════════════╝

Configuration:
  KYBER_N:      256 (polynomial degree)
  KYBER_K:      2 (matrix dimension)
  KYBER_Q:      3329 (modulus)
  XOF_BLOCKBYTES: 168

=== Test 1: gen_a Basic Functionality ===
...
[PASS] gen_a produces correct coefficients

=== Test 2: gen_at Basic Functionality ===
...
[PASS] gen_at produces correct coefficients

...

╔═══════════════════════════════════════════════════════════════╗
║                      TEST SUMMARY                             ║
╠═══════════════════════════════════════════════════════════════╣
║  Total Tests:  5
║  Passed:       5
║  Failed:       0
║  Pass Rate:    100.0%
╚═══════════════════════════════════════════════════════════════╝
```

### Hardware-Optimized Implementation
```bash
./gen_matrix_hw
```

Expected output:
```
╔═══════════════════════════════════════════════════════════════╗
║   Kyber gen_matrix Test Suite (HARDWARE-OPTIMIZED VERSION)    ║
║   Tests matrix A generation with HW Keccak acceleration       ║
╚═══════════════════════════════════════════════════════════════╝

Configuration:
  KYBER_N:        256 (polynomial degree)
  KYBER_K:        2 (matrix dimension K×K)
  KYBER_Q:        3329 (modulus)
  XOF_BLOCKBYTES: 168 (SHAKE128 rate)

HW Acceleration:
  keccak_init_state:   Initialize state (single cycle)
  absorb_xor:          XOR data into state (in registers)
  keccak_permute:      Permutation (in-place, no I/O)
  store_horcrux:       Extract results (final output only)
  Expected benefit:    2-3× faster multi-block hashing

=== Test 1: gen_a HW-Optimized Performance ===
Performance (HW-Optimized):
  Cycles:       1234
  Instructions: 1000
  CPI:          1.23
  Expected improvement vs SW: ~2-3×

[PASS] HW-optimized gen_a produces correct coefficients

...

╔═══════════════════════════════════════════════════════════════╗
║                    HW TEST SUMMARY                            ║
╠═══════════════════════════════════════════════════════════════╣
║  Total Tests:  5
║  Passed:       5
║  Failed:       0
║  Pass Rate:    100.0%
║                                                               ║
║  Expected HW Speedup: 2-3× vs Software Implementation        ║
║  Key Benefit: State remains in HW registers between blocks   ║
║               Eliminates redundant memory I/O                ║
╚═══════════════════════════════════════════════════════════════╝
```

## Customization Points

### For Testing Custom Instructions

After implementing new Keccak instructions (e.g., specialized absorb operations):

1. **Modify gen_matrix.c**: Replace `xof_absorb()` and `xof_squeezeblocks()` calls
2. **Call new instructions**: Invoke through coprocessor interface
3. **Measure performance**: Existing cycle counting infrastructure captures results
4. **Verify correctness**: Golden reference comparison remains unchanged

Example:
```c
/* Original software-based absorption */
void xof_absorb(xof_state *state, const uint8_t seed[32], uint8_t i, uint8_t j)
{
    shake128_absorb_once(state, seed, 32);
    uint8_t ij[2] = {i, j};
    shake128_absorb(state, ij, 2);
    shake128_finalize(state);
}

/* Can be replaced with custom HW instruction */
void xof_absorb_custom(xof_state *state, const uint8_t seed[32], uint8_t i, uint8_t j)
{
    /* Call custom instruction via coprocessor */
    coproc_xof_absorb_with_domain(state, seed, i, j);
    /* Or invoke lower-level primitives with new ops */
}
```

## Comparison Workflow

1. **Establish baseline**: Run SW version, record cycle counts
2. **Implement HW version**: Modify gen_matrix.c to use hardware instructions
3. **Test HW implementation**: Run with same cycle counting
4. **Compare results**: Expected 2-3× improvement
5. **Validate correctness**: Golden reference checks pass for both

## Integration with CRHEEPTO

These tests are designed to validate:
- ✅ Horcrux coprocessor correctness (gen_matrix is real PQC workload)
- ✅ New granular Keccak instructions (absorb_xor, permute, init_state)
- ✅ Performance of HW acceleration (state persistence in registers)
- ✅ Software/hardware equivalence (bit-identical output)

## See Also

- [KECCAK_INSTRUCTIONS_DOCUMENTATION.md](../../../KECCAK_INSTRUCTIONS_DOCUMENTATION.md) - Detailed instruction set design
- [keccak-abs-sw/](../keccak-abs-sw/) - Software Keccak reference
- [keccak-abs-hw/](../keccak-abs-hw/) - Hardware-optimized Keccak
- [indcpa.c in ML-KEM implementations](../../pqc/baseline/KEM/ML-KEM/) - Real-world usage

---

**Created**: February 16, 2026  
**Version**: 1.0  
**Target**: CRHEEPTO Platform with Horcrux Coprocessor
