# ML-KEM & ML-DSA with Ascon - Reference Guide

## Overview

This repository contains implementations of post-quantum cryptographic schemes with two symmetric primitive backends:
- **FIPS-202 (Keccak/SHA3/SHAKE)** - Standard implementation
- **Ascon** - Lightweight cryptography alternative (128-bit security level only)

### Supported Schemes

| Scheme | NIST Standard | Purpose | Implementations |
|--------|---------------|---------|-----------------|
| **ML-KEM** (Kyber) | FIPS 203 | Key Encapsulation Mechanism | `ml-kem/ref/`, `ml-kem/ref_ascon/` |
| **ML-DSA** (Dilithium) | FIPS 204 | Digital Signature Algorithm | `ml-dsa/ref/`, `ml-dsa/ref_ascon/` |

---

## Why Ascon for Post-Quantum Cryptography?

**Ascon** won the NIST Lightweight Cryptography (LWC) competition and is designed for resource-constrained devices (IoT, embedded systems, smart cards).

### Architectural Comparison

| Feature | Keccak (FIPS-202) | Ascon |
|---------|-------------------|-------|
| **Primary Focus** | General-purpose, standardized | Lightweight, low-power embedded |
| **Internal State** | 1600 bits (200 bytes) | 320 bits (40 bytes) |
| **Security Level** | Up to 256-bit | 128-bit |
| **Hash Functions** | SHA3-256, SHA3-512 | Ascon-Hash, Ascon-XOF |
| **XOF/PRF** | SHAKE128, SHAKE256 | Ascon-XOF |

### Why Only Security Level 1 (128-bit)?

Ascon was designed to provide **128-bit security** - optimal for lightweight applications. This means:

- **ML-KEM-512 (Kyber-512)** → 128-bit classical security → ✅ Matches Ascon
- **ML-KEM-768/1024** → 192/256-bit security → ❌ Exceeds Ascon's design target
- **ML-DSA-44 (Dilithium-2)** → 128-bit classical security → ✅ Matches Ascon
- **ML-DSA-65/87** → 192/256-bit security → ❌ Exceeds Ascon's design target

---

# Part 1: ML-KEM (Kyber)

## Standard ML-KEM (FIPS-202/Keccak)

### Run All Tests
```bash
cd ml-kem/ref
./run_all_kyber_tests.sh      # Regular tests with key verification
./run_all_kyber_kat_tests.sh  # KAT tests
```

### Run Individual Tests

**ML-KEM-512:**
```bash
cd ml-kem/ref/test
./test_kyber512
```

**ML-KEM-768:**
```bash
cd ml-kem/ref/test
./test_kyber768
```

**ML-KEM-1024:**
```bash
cd ml-kem/ref/test
./test_kyber1024
```

### Run Individual KAT Generators
```bash
cd ml-kem/ref/nistkat
./PQCgenKAT_kem512   # Generates PQCkemKAT_1632.req/.rsp
./PQCgenKAT_kem768   # Generates PQCkemKAT_2400.req/.rsp
./PQCgenKAT_kem1024  # Generates PQCkemKAT_3168.req/.rsp
```

### Build Commands
```bash
cd ml-kem/ref
make clean && make test    # Build test binaries
make nistkat               # Build KAT generators
make speed                 # Build speed benchmarks
```

---

## ML-KEM-Ascon (Lightweight Implementation)

**Only ML-KEM-512 is supported** (128-bit security matches Ascon's design).

### Run Tests
```bash
cd ml-kem/ref_ascon
./run_kyber_ascon_tests.sh
```

Or manually:
```bash
cd ml-kem/ref_ascon/test
./test_kyber512_ascon
```

### Run KAT Generator
```bash
cd ml-kem/ref_ascon
./run_kyber_ascon_kat.sh
```

Or manually:
```bash
cd ml-kem/ref_ascon/nistkat
./PQCgenKAT_kem512_ascon
```

### Build Commands
```bash
cd ml-kem/ref_ascon
make clean && make test    # Build test binary
make nistkat               # Build KAT generator
```

### Primitive Mapping (Keccak → Ascon) for ML-KEM

| ML-KEM Function | Keccak (ref/) | Ascon (ref_ascon/) |
|-----------------|---------------|---------------------|
| H (32-byte hash) | SHA3-256 | Ascon-Hash |
| G (64-byte hash) | SHA3-512 | Ascon-XOF (64 bytes) |
| XOF (matrix sampling) | SHAKE128 | Ascon-XOF |
| PRF (noise generation) | SHAKE256 | Ascon-XOF |

---

# Part 2: ML-DSA (Dilithium)

## Standard ML-DSA (FIPS-202/Keccak)

### Run All Tests
```bash
cd ml-dsa/ref
make clean && make
./test/test_dilithium2   # ML-DSA-44 (Level 1)
./test/test_dilithium3   # ML-DSA-65 (Level 3)
./test/test_dilithium5   # ML-DSA-87 (Level 5)
```

### Run Test Vectors
```bash
cd ml-dsa/ref/test
./test_vectors2   # ML-DSA-44
./test_vectors3   # ML-DSA-65
./test_vectors5   # ML-DSA-87
```

### Run KAT Generators
```bash
cd ml-dsa/ref
make nistkat
cd nistkat
./PQCgenKAT_sign2   # Generates PQCsignKAT_*.req/.rsp for Dilithium2
./PQCgenKAT_sign3   # Generates PQCsignKAT_*.req/.rsp for Dilithium3
./PQCgenKAT_sign5   # Generates PQCsignKAT_*.req/.rsp for Dilithium5
```

### Build Commands
```bash
cd ml-dsa/ref
make clean && make         # Build all test binaries
make nistkat               # Build KAT generators
make speed                 # Build speed benchmarks
```

---

## ML-DSA-Ascon (Lightweight Implementation)

**Only ML-DSA-44 (Dilithium-2) is supported** (128-bit security matches Ascon's design).

### Run Tests
```bash
cd ml-dsa/ref_ascon
./run_dilithium_ascon_tests.sh
```

Or manually:
```bash
cd ml-dsa/ref_ascon/test
./test_dilithium2_ascon
```

### Run Test Vectors
```bash
cd ml-dsa/ref_ascon/test
./test_vectors2_ascon
```

### Run KAT Generator
```bash
cd ml-dsa/ref_ascon
./run_dilithium_ascon_kat.sh
```

Or manually:
```bash
cd ml-dsa/ref_ascon
make nistkat
cd nistkat
./PQCgenKAT_sign2_ascon
```

### Build Commands
```bash
cd ml-dsa/ref_ascon
make clean && make test    # Build test binary
make nistkat               # Build KAT generator
make speed                 # Build speed benchmarks
```

### Primitive Mapping (Keccak → Ascon) for ML-DSA

| ML-DSA Function | Keccak (ref/) | Ascon (ref_ascon/) |
|-----------------|---------------|---------------------|
| H (hashing) | SHA3-256/512 | Ascon-Hash/XOF |
| Stream128 (matrix expansion) | SHAKE128 | Ascon-XOF |
| Stream256 (sampling) | SHAKE256 | Ascon-XOF |
| Challenge generation | SHAKE256 | Ascon-XOF |

---

## Key/Signature Sizes

### ML-KEM (Key Encapsulation)

| Variant | Secret Key | Public Key | Ciphertext | Security Level |
|---------|------------|------------|------------|----------------|
| ML-KEM-512 | 1632 bytes | 800 bytes | 768 bytes | 128-bit (Level 1) |
| ML-KEM-768 | 2400 bytes | 1184 bytes | 1088 bytes | 192-bit (Level 3) |
| ML-KEM-1024 | 3168 bytes | 1568 bytes | 1568 bytes | 256-bit (Level 5) |

**ML-KEM-Ascon**: Only ML-KEM-512 supported.

### ML-DSA (Digital Signatures)

| Variant | Secret Key | Public Key | Signature | Security Level |
|---------|------------|------------|-----------|----------------|
| ML-DSA-44 (Dilithium2) | 2560 bytes | 1312 bytes | 2420 bytes | 128-bit (Level 1) |
| ML-DSA-65 (Dilithium3) | 4032 bytes | 1952 bytes | 3293 bytes | 192-bit (Level 3) |
| ML-DSA-87 (Dilithium5) | 4896 bytes | 2592 bytes | 4595 bytes | 256-bit (Level 5) |

**ML-DSA-Ascon**: Only ML-DSA-44 supported.

---

## Example Output

### ML-KEM Test (Key Exchange Verification)
```
=== Key Exchange Verification ===
Public Key (pk): 1ae823ee69bd77e31991e035994bb51a...
Secret Key (sk): 0eb8b3fbd30cba10a186506745025589...
Shared Key A (Alice): 9bab0179228e58270ba125f7e556ddcede6c510b756accb39c5060850ecc76d5
Shared Key B (Bob): 9bab0179228e58270ba125f7e556ddcede6c510b756accb39c5060850ecc76d5
✓ SUCCESS: Keys match! Key exchange successful.
```

### ML-DSA Test
```
CRYPTO_PUBLICKEYBYTES = 1312
CRYPTO_SECRETKEYBYTES = 2560
CRYPTO_BYTES = 2420
```

---

## Directory Structure

```
keccak-ascon/
├── ml-kem/
│   ├── ref/                    # Standard ML-KEM (Keccak/SHAKE)
│   │   ├── fips202.c/h         # Keccak implementation
│   │   ├── symmetric-shake.c   # SHAKE-based symmetric primitives
│   │   └── test/, nistkat/     # Tests and KAT generators
│   │
│   └── ref_ascon/              # ML-KEM-Ascon (Kyber-512 only)
│       ├── ascon.c/h           # Ascon XOF/Hash implementation
│       ├── symmetric-ascon.c   # Ascon-based symmetric primitives
│       └── test/, nistkat/     # Tests and KAT generators
│
├── ml-dsa/
│   ├── ref/                    # Standard ML-DSA (Keccak/SHAKE)
│   │   ├── fips202.c/h         # Keccak implementation
│   │   ├── symmetric-shake.c   # SHAKE-based symmetric primitives
│   │   └── test/, nistkat/     # Tests and KAT generators
│   │
│   └── ref_ascon/              # ML-DSA-Ascon (Dilithium-2 only)
│       ├── ascon.c/h           # Ascon XOF/Hash implementation
│       ├── symmetric-ascon.c   # Ascon-based symmetric primitives
│       └── test/, nistkat/     # Tests and KAT generators
│
└── KYBER_KAT_TESTS.md          # This documentation
```

---

## Files Modified/Created for Ascon Integration

### ML-KEM-Ascon (ml-kem/ref_ascon/)
- `ascon.h/c` - Ascon XOF/Hash interface with incremental absorb/squeeze
- `symmetric-ascon.c` - Kyber-specific Ascon wrappers
- `symmetric.h` - Updated to use Ascon instead of Keccak
- `indcpa.c` - Modified gen_matrix() for Ascon's 8-byte block size
- `test/test_vectors.c` - Updated RNG to use Ascon-XOF
- `run_kyber_ascon_tests.sh` - Test runner script
- `run_kyber_ascon_kat.sh` - KAT generator script

### ML-DSA-Ascon (ml-dsa/ref_ascon/)
- `ascon.h/c` - Ascon XOF/Hash interface with SHAKE-compatible API
- `symmetric-ascon.c` - Dilithium-specific Ascon stream init functions
- `symmetric.h` - Updated to use Ascon state and functions
- `sign.c` - Modified to include ascon.h instead of fips202.h
- `test/test_vectors.c` - Updated RNG to use Ascon-XOF
- `run_dilithium_ascon_tests.sh` - Test runner script
- `run_dilithium_ascon_kat.sh` - KAT generator script

