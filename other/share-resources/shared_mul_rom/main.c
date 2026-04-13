#include "kyber.h"
#include "dilithium.h"
#include "bfu_rom.h"
#include <stdio.h>

// Debug flag: 1 = Print all coefficients, 0 = Print only errors
#define DEBUG 0

// Comparison helper for consistent output
int compare_coeffs(const char* test_name, int idx, int32_t got, int32_t exp) {
    int match = (got == exp);
    #if DEBUG
        printf("%s [%3d] -> Got: %8d | Exp: %8d %s\n", 
                test_name, idx, got, exp, match ? " " : "[MISMATCH]");
    #endif
    if (!match && !DEBUG) {
        printf("[%s ERROR] Mismatch @ idx %3d: Got %8d, Expected %8d\n", 
                test_name, idx, got, exp);
    }
    return match;
}


/************************************************************** */
int test_kyber_ntt_sw() {
    printf("\n--- Kyber NTT (Software) ---\n");
    kyber_poly r = kyber_tv_ntt_in;
    kyber_ntt(r.coeffs);
    int ok = 1;
    for (int i = 0; i < KEM_N; i++) ok &= compare_coeffs("K-NTT-SW", i, r.coeffs[i], kyber_tv_ntt_out.coeffs[i]);
    return ok;
}

int test_dilithium_ntt_sw() {
    printf("\n--- Dilithium NTT (Software) ---\n");
    dsa_poly a = dsa_tv_ntt_in;
    dilithium_ntt(a.coeffs);
    int ok = 1;
    for (int i = 0; i < DSA_N; i++) ok &= compare_coeffs("D-NTT-SW", i, a.coeffs[i], dsa_tv_ntt_out.coeffs[i]);
    return ok;
}


int test_kyber_intt_sw() {
    printf("\n--- Kyber INTT (Software) ---\n");
    // Load input test vector
    kyber_poly r = kyber_tv_intt_in;
    
    // Execute software INTT
    kyber_invntt(r.coeffs);
    
    int ok = 1;
    for (int i = 0; i < KEM_N; i++) {
        // Cast to int32_t for comparison printing consistency
        ok &= compare_coeffs("K-INTT-SW", i, (int32_t)r.coeffs[i], (int32_t)kyber_tv_intt_out.coeffs[i]);
    }
    printf("Result: %s\n", ok ? "PASS" : "FAIL");
    return ok;
}

int test_dilithium_intt_sw() {
    printf("\n--- Dilithium INTT (Software) ---\n");
    // Load input test vector
    dsa_poly a = dsa_tv_intt_in;
    
    // Execute software INTT
    dilithium_invntt(a.coeffs);
    
    int ok = 1;
    for (int i = 0; i < DSA_N; i++) {
        ok &= compare_coeffs("D-INTT-SW", i, a.coeffs[i], dsa_tv_intt_out.coeffs[i]);
    }
    printf("Result: %s\n", ok ? "PASS" : "FAIL");
    return ok;
}



/************************************************************** */
int test_kyber_ntt_rom() {
    printf("\n--- Kyber NTT (ROM-based BFU) ---\n");
    kyber_poly r = kyber_tv_ntt_in;
    uint32_t k = 1;
    for (int len = 128; len >= 2; len >>= 1) {
        for (int start = 0; start < 256; start = start + 2 * len) {
            uint32_t tw_idx = k++;
            for (int j = start; j < start + len; j += 2) {
                uint32_t rs1 = ((uint32_t)(uint16_t)r.coeffs[j+1] << 16) | (uint32_t)(uint16_t)r.coeffs[j];
                uint32_t rs2 = ((uint32_t)(uint16_t)r.coeffs[j+1+len] << 16) | (uint32_t)(uint16_t)r.coeffs[j+len];
                bfu_result_t out = bfu_unit_rom(rs1, rs2, tw_idx, OP_BFNTTK);
                r.coeffs[j] = (int16_t)(out.rd_lo & 0xFFFF);
                r.coeffs[j+1] = (int16_t)(out.rd_lo >> 16);
                r.coeffs[j+len] = (int16_t)(out.rd_hi & 0xFFFF);
                r.coeffs[j+1+len] = (int16_t)(out.rd_hi >> 16);
            }
        }
    }
    int ok = 1;
    for (int i = 0; i < 256; i++) ok &= compare_coeffs("K-NTT-ROM", i, r.coeffs[i], kyber_tv_ntt_out.coeffs[i]);
    return ok;
}

int test_dilithium_ntt_rom() {
    printf("\n--- Dilithium NTT (ROM-based BFU) ---\n");
    dsa_poly a = dsa_tv_ntt_in;
    uint32_t k = 0;
    for (int len = 128; len > 0; len >>= 1) {
        for (int start = 0; start < 256; start = start + 2 * len) {
            uint32_t tw_idx = ++k;
            for (int j = start; j < start + len; ++j) {
                bfu_result_t out = bfu_unit_rom((uint32_t)a.coeffs[j], (uint32_t)a.coeffs[j+len], tw_idx, OP_BFNTTD);
                a.coeffs[j] = (int32_t)out.rd_lo;
                a.coeffs[j+len] = (int32_t)out.rd_hi;
            }
        }
    }
    int ok = 1;
    for (int i = 0; i < 256; i++) ok &= compare_coeffs("D-NTT-ROM", i, a.coeffs[i], dsa_tv_ntt_out.coeffs[i]);
    return ok;
}

int test_kyber_intt_rom() {
    printf("\n--- Kyber INTT (ROM-based BFU) ---\n");
    kyber_poly r = kyber_tv_intt_in;
    uint32_t k = 127;
    for (int len = 2; len <= 128; len <<= 1) {
        for (int start = 0; start < 256; start = start + 2 * len) {
            uint32_t tw_idx = k--;
            for (int j = start; j < start + len; j += 2) {
                uint32_t rs1 = ((uint32_t)(uint16_t)r.coeffs[j+1] << 16) | (uint32_t)(uint16_t)r.coeffs[j];
                uint32_t rs2 = ((uint32_t)(uint16_t)r.coeffs[j+1+len] << 16) | (uint32_t)(uint16_t)r.coeffs[j+len];
                bfu_result_t out = bfu_unit_rom(rs1, rs2, tw_idx, OP_BFINTTK);
                r.coeffs[j] = (int16_t)(out.rd_lo & 0xFFFF);
                r.coeffs[j+1] = (int16_t)(out.rd_lo >> 16);
                r.coeffs[j+len] = (int16_t)(out.rd_hi & 0xFFFF);
                r.coeffs[j+1+len] = (int16_t)(out.rd_hi >> 16);
            }
        }
    }
    // Final Scaling: r[j] = fqmul(r[j], 1441)
    for (int j = 0; j < 256; j++) {
        int32_t prod = (int32_t)r.coeffs[j] * 1441;
        int16_t t = (int16_t)prod * (int16_t)KEM_QINV;
        r.coeffs[j] = (int16_t)((prod - (int32_t)t * (int32_t)KEM_Q) >> 16);
    }
    int ok = 1;
    for (int i = 0; i < 256; i++) ok &= compare_coeffs("K-INTT-ROM", i, r.coeffs[i], kyber_tv_intt_out.coeffs[i]);
    return ok;
}

int test_dilithium_intt_rom() {
    printf("\n--- Dilithium INTT (ROM-based BFU) ---\n");
    dsa_poly a = dsa_tv_intt_in;
    uint32_t k = 256;
    for (int len = 1; len < 256; len <<= 1) {
        for (int start = 0; start < 256; start = start + 2 * len) {
            uint32_t tw_idx = --k;
            for (int j = start; j < start + len; ++j) {
                bfu_result_t out = bfu_unit_rom((uint32_t)a.coeffs[j], (uint32_t)a.coeffs[j+len], tw_idx, OP_BFINTTD);
                a.coeffs[j] = (int32_t)out.rd_lo;
                a.coeffs[j+len] = (int32_t)out.rd_hi;
            }
        }
    }
    // Final Scaling: a[j] = fqmul(a[j], 41978)
    for (int j = 0; j < 256; j++) {
        int64_t prod = (int64_t)a.coeffs[j] * 41978;
        int32_t t = (int32_t)prod * (int32_t)DSA_QINV;
        a.coeffs[j] = (int32_t)((prod - (int64_t)t * (int64_t)DSA_Q) >> 32);
    }
    int ok = 1;
    for (int i = 0; i < 256; i++) ok &= compare_coeffs("D-INTT-ROM", i, a.coeffs[i], dsa_tv_intt_out.coeffs[i]);
    return ok;
}


/************************************************************** */
int main() {
    int all_passed = 1;

    printf("======================================\n");
    printf("   PQ-CRYPTO SOFTWARE MODULE TESTS    \n");
    printf("======================================\n");

    // 1. Dilithium SW Tests
    all_passed &= test_dilithium_ntt_sw();
    all_passed &= test_dilithium_intt_sw();

    // 2. Kyber SW Tests
    all_passed &= test_kyber_ntt_sw();
    all_passed &= test_kyber_intt_sw();

    printf("\n======================================\n");
    printf("   BFU HARDWARE EMULATOR TESTS WITH ROM  \n");
    printf("======================================\n");

    // 3. BFU Emulator Tests
    all_passed &= test_kyber_ntt_rom();
    all_passed &= test_dilithium_ntt_rom();
    all_passed &= test_dilithium_intt_rom();
    all_passed &= test_kyber_intt_rom();


    printf("\n======================================\n");
    if (all_passed) {
        printf("FINAL STATUS: ALL TESTS PASSED\n");
    } else {
        printf("FINAL STATUS: TEST FAILURE DETECTED\n");
    }
    printf("======================================\n");

    return all_passed ? 0 : 1;
}