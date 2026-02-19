# Kyber KAT Tests - Quick Reference Guide

## Overview
This guide shows you how to run Kyber512, Kyber768, and Kyber1024 with Known Answer Tests (KAT).
Two implementations are available:
- **ref/** - Standard Kyber using FIPS-202 (Keccak/SHA3/SHAKE)
- **ref_ascon/** - Kyber-Ascon using Ascon lightweight cryptography (Kyber-512 only)

The test_kyber.c file has been enhanced with print statements to show pk, sk, key_a, and key_b.

---

## Part 1: Standard Kyber (FIPS-202/Keccak)

### Option 1: Run All KAT Tests at Once (Recommended)
```bash
cd ref
./run_all_kyber_kat_tests.sh
```

### Option 2: Run All Regular Tests at Once (Shows key verification)
```bash
cd ref
./run_all_kyber_tests.sh
```

### Option 3: Run Individual KAT Tests

**Kyber512 KAT:**
```bash
cd ref/nistkat
./PQCgenKAT_kem512
```
This generates:
- `PQCkemKAT_1632.req` - Request file
- `PQCkemKAT_1632.rsp` - Response file with test vectors

**Kyber768 KAT:**
```bash
cd ref/nistkat
./PQCgenKAT_kem768
```
This generates:
- `PQCkemKAT_2400.req` - Request file
- `PQCkemKAT_2400.rsp` - Response file with test vectors

**Kyber1024 KAT:**
```bash
cd ref/nistkat
./PQCgenKAT_kem1024
```
This generates:
- `PQCkemKAT_3168.req` - Request file
- `PQCkemKAT_3168.rsp` - Response file with test vectors

### Option 4: Run Individual Regular Tests (with key verification prints)

**Kyber512 Test:**
```bash
cd ref/test
./test_kyber512
```

**Kyber768 Test:**
```bash
cd ref/test
./test_kyber768
```

**Kyber1024 Test:**
```bash
cd ref/test
./test_kyber1024
```

---

## Part 2: Kyber-Ascon (Lightweight Implementation)

### Why Kyber-Ascon Only Supports Kyber-512 (Security Level 1)

**Ascon** is a family of lightweight authenticated encryption and hashing algorithms that won the NIST Lightweight Cryptography (LWC) competition. It was specifically designed for resource-constrained devices (IoT, embedded systems, smart cards).

**Key Architectural Differences:**

| Feature | Keccak (FIPS-202) | Ascon |
|---------|-------------------|-------|
| **Primary Focus** | Standardized, general-purpose security | Lightweight, low-power embedded systems |
| **Internal State** | 1600 bits (200 bytes) | 320 bits (40 bytes) |
| **Security Level** | Up to 256-bit | 128-bit |
| **Hash Function** | SHA3-256, SHA3-512 | Ascon-Hash, Ascon-XOF |
| **XOF/PRF** | SHAKE128, SHAKE256 | Ascon-XOF |

**Why Only 128-bit Security (Kyber-512)?**

1. **Ascon's Design Target**: Ascon was designed to provide **128-bit security** - optimal for lightweight/embedded applications where higher security levels add unnecessary overhead.

2. **Internal State Size**: Ascon's 320-bit state is much smaller than Keccak's 1600-bit state. While this makes Ascon more efficient on constrained devices, it limits the achievable security margin for higher security levels.

3. **Security Level Mapping**:
   - **Kyber-512** → 128-bit classical security → ✅ Matches Ascon's 128-bit primitives
   - **Kyber-768** → 192-bit classical security → ❌ Exceeds Ascon's design target
   - **Kyber-1024** → 256-bit classical security → ❌ Exceeds Ascon's design target

4. **NIST Recommendation**: For ML-KEM (the standardized version of Kyber), using Ascon is only appropriate for Security Level 1 (equivalent to AES-128).

### Commands to Run Kyber-Ascon Tests

**Run Kyber-512-Ascon Test:**
```bash
cd ref_ascon
./run_kyber_ascon_tests.sh
```

Or manually:
```bash
cd ref_ascon/test
./test_kyber512_ascon
```

**Run Kyber-512-Ascon KAT Test:**
```bash
cd ref_ascon
./run_kyber_ascon_kat.sh
```

Or manually:
```bash
cd ref_ascon/nistkat
./PQCgenKAT_kem512_ascon
```
This generates:
- `PQCkemKAT_1632.req` - Request file
- `PQCkemKAT_1632.rsp` - Response file with Ascon-based test vectors

### Rebuilding Kyber-Ascon

```bash
cd ref_ascon
make clean
make test      # Build test binary
make nistkat   # Build KAT generator
```

### Primitive Mapping (Keccak → Ascon)

| Kyber Function | Keccak (ref/) | Ascon (ref_ascon/) |
|----------------|---------------|---------------------|
| H (32-byte hash) | SHA3-256 | Ascon-Hash |
| G (64-byte hash) | SHA3-512 | Ascon-XOF (64 bytes) |
| XOF (sampling) | SHAKE128 | Ascon-XOF |
| PRF (noise) | SHAKE256 | Ascon-XOF |

---

## What the Tests Show

The enhanced test_kyber.c now prints (for the first iteration):
- **Public Key (pk)**: First 32 bytes in hex
- **Secret Key (sk)**: First 32 bytes in hex
- **Shared Key A (Alice)**: Full key in hex
- **Shared Key B (Bob)**: Full key in hex
- **Verification**: ✓ SUCCESS message if keys match

Example output:
```
=== Key Exchange Verification ===
Public Key (pk): 1ae823ee69bd77e31991e035994bb51a621019f7323fd4ba7df537f5d33f6f60...
Secret Key (sk): 0eb8b3fbd30cba10a186506745025589055c0184315828c5c4e52bdeb986f7f9...
Shared Key A (Alice): 9bab0179228e58270ba125f7e556ddcede6c510b756accb39c5060850ecc76d5
Shared Key B (Bob): 9bab0179228e58270ba125f7e556ddcede6c510b756accb39c5060850ecc76d5
✓ SUCCESS: Keys match! Key exchange successful.
=================================
```

---

## Rebuilding Tests (Standard Kyber)

If you need to rebuild all tests:
```bash
cd ref
make clean
make test      # Build regular tests
make nistkat   # Build KAT tests
```

---

## Key Sizes

| Variant | Secret Key | Public Key | Ciphertext | Security Level |
|---------|------------|------------|------------|----------------|
| Kyber512 | 1632 bytes | 800 bytes | 768 bytes | 128-bit (Level 1) |
| Kyber768 | 2400 bytes | 1184 bytes | 1088 bytes | 192-bit (Level 3) |
| Kyber1024 | 3168 bytes | 1568 bytes | 1568 bytes | 256-bit (Level 5) |

**Kyber-Ascon**: Only Kyber-512 is supported (128-bit security matches Ascon's design).

---

## Files Modified/Created

### Standard Kyber (ref/)
- `ref/test/test_kyber.c` - Enhanced with key printing
- `ref/run_all_kyber_kat_tests.sh` - Script to run all KAT tests
- `ref/run_all_kyber_tests.sh` - Script to run all regular tests

### Kyber-Ascon (ref_ascon/)
- `ref_ascon/ascon.h` - Ascon XOF/Hash interface
- `ref_ascon/ascon.c` - Ascon implementation with incremental absorb/squeeze
- `ref_ascon/symmetric-ascon.c` - Kyber-specific Ascon wrappers
- `ref_ascon/symmetric.h` - Updated to use Ascon instead of Keccak
- `ref_ascon/indcpa.c` - Modified gen_matrix() for Ascon's 8-byte block size
- `ref_ascon/test/test_vectors.c` - Updated RNG to use Ascon-XOF
- `ref_ascon/run_kyber_ascon_tests.sh` - Script to run Ascon tests
- `ref_ascon/run_kyber_ascon_kat.sh` - Script to run Ascon KAT generator

