#include "tests.h"
#include "kyber.h"
#include "dilithium.h"
#include "bfu.h"
#include "falcon.h"
#include "hqc.h"
#include <stdio.h>

#define DEBUG 0

// Helper for logging
static int compare_scalar(const char* test_name, int index, int32_t input, int32_t got, int32_t expected) {
    if (got != expected) {
        printf("FAILED: %s [%d] for input %d: Got %d, Expected %d\n", test_name, index, input, got, expected);
        return 0;
    }
    return 1;
}

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

// --- SOFTWARE TESTS ---

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
    kyber_poly r = kyber_tv_intt_in;
    kyber_invntt(r.coeffs);
    int ok = 1;
    for (int i = 0; i < KEM_N; i++) ok &= compare_coeffs("K-INTT-SW", i, (int32_t)r.coeffs[i], (int32_t)kyber_tv_intt_out.coeffs[i]);
    return ok;
}

int test_dilithium_intt_sw() {
    printf("\n--- Dilithium INTT (Software) ---\n");
    dsa_poly a = dsa_tv_intt_in;
    dilithium_invntt(a.coeffs);
    int ok = 1;
    for (int i = 0; i < DSA_N; i++) ok &= compare_coeffs("D-INTT-SW", i, a.coeffs[i], dsa_tv_intt_out.coeffs[i]);
    return ok;
}

int test_mq_montymul_sw() {
    printf("\n--- Falcon mq_montymul (Software) ---\n");
    int ok = 1;
    for (int i = 0; i < 100; i++) {
        uint32_t got = mq_montymul_sw(falcon_source1[i], falcon_source0[i]);
        ok &= compare_coeffs("F-MUL-SW", i, (int32_t)got, (int32_t)falcon_golden[i]);
    }
    return ok;
}

int test_karats_sw() {
    printf("\n--- HQC Karatsuba 64x64 (Software) ---\n");
    int ok = 1;
    for (int i = 0; i < NUM_HQC_TESTS; i++) {
        uint64_t res[2];
        gf2_mul64_sw(res, hqc_a[i], hqc_b[i]);
        if (res[0] != hqc_out_ref[i].val[0] || res[1] != hqc_out_ref[i].val[1]) ok = 0;
    }
    return ok;
}

int test_gf_carryless_sw() {
    printf("\n--- HQC 8-bit Carry-less (Software) ---\n");
    int ok = 1;
    for (int i = 0; i < NTESTS_8BIT; i++) {
        uint8_t res[2];
        gf_carryless_mul_sw(res, hqc_a_8bit[i], hqc_b_8bit[i]);
        if (res[0] != hqc_c0_expected[i] || res[1] != hqc_c1_expected[i]) ok = 0;
    }
    return ok;
}


int test_fqmul_sw() {
    printf("\n--- Kyber fqmul (Montgomery Reduction) Test ---\n");

    fqmul_test_vector tests[] = {
        {-758, -2, -129},
        {-758,  0,    0},
        {-758, -1, 1600},
        {-758,  1, -1600},
        {-758,  2,  129},
        {-758, -3, 1471},
        {-758,  3, -1471}
    };

    int num_tests = 5;
    int ok = 1;

    for (int i = 0; i < num_tests; i++) {
        // Call the software implementation
        int16_t result = fqmul(tests[i].a, tests[i].b);

        // Check result
        if (result != tests[i].expected) {
            printf("  [FAIL] Test %d: fqmul(%d, %d)\n", i, tests[i].a, tests[i].b);
            printf("         Expected: %d, Got: %d\n", tests[i].expected, result);
            ok = 0;
        } else {
            // Optional: verbose success output
            //printf("  [OK] fqmul(%5d, %5d) = %5d\n", tests[i].a, tests[i].b, result);
        }
    }

    if (ok) {
        printf("--- fqmul Test PASSED (%d cases) ---\n", num_tests);
    } else {
        printf("--- fqmul Test FAILED ---\n");
    }

    return ok;
}


int test_dilithium_reduce32() {
    printf("\n--- Dilithium reduce32 ---\n");
    int ok = 1;
    for (int i = 0; i < 20; i++) {
        int32_t result = reduce32(dsa_tv_scalars[i].input);
        ok &= compare_scalar("D-REDUCE32", i, dsa_tv_scalars[i].input, result, dsa_tv_scalars[i].expected_reduce);
    }
    if (ok) printf("PASSED: Dilithium reduce32\n");
    return ok;
}

int test_dilithium_caddq() {
    printf("\n--- Dilithium caddq ---\n");
    int ok = 1;
    for (int i = 0; i < 20; i++) {
        int32_t result = caddq(dsa_tv_scalars[i].input);
        ok &= compare_scalar("D-CADDQ", i, dsa_tv_scalars[i].input, result, dsa_tv_scalars[i].expected_caddq);
    }
    if (ok) printf("PASSED: Dilithium caddq\n");
    return ok;
}


// --- BFU TESTS (32-bit SISO) ---

int test_kyber_ntt_bfu() {
    printf("\n--- Kyber NTT (32-bit SISO BFU) ---\n");
    kyber_poly r = kyber_tv_ntt_in;
    int k = 1;
    for (int len = 128; len >= 2; len >>= 1) {
        for (int start = 0; start < 256; start = start + 2 * len) {
            int16_t zeta = zetas_KEM[k++];
            //printf(" Stage len=%3d, start=%3d, zeta=%6d, k=%3d\n", len, start, zeta, k);
            for (int j = start; j < start + len; j++) {
                //uint32_t rs1 = ((uint32_t)(uint16_t)r.coeffs[j+len] << 16) | (uint32_t)(uint16_t)r.coeffs[j];
                uint32_t rs1 = (uint32_t)(uint16_t)r.coeffs[j];
                uint32_t rs2 = (uint32_t)(uint16_t)r.coeffs[j+len];
                
                bfu_result_t out = bfu_unit(rs1, rs2, (int32_t)zeta, OP_BFNTTK);
                r.coeffs[j] = (int16_t)(out.rd_lo & 0xFFFF);
                r.coeffs[j+len] = (int16_t)(out.rd_lo >> 16);
            }
        }
    }
    int ok = 1;
    for (int i = 0; i < 256; i++) ok &= compare_coeffs("K-NTT-BFU", i, r.coeffs[i], kyber_tv_ntt_out.coeffs[i]);
    return ok;
}

int test_dilithium_ntt_bfu() {
    printf("\n--- Dilithium NTT (32-bit BFU) ---\n");
    dsa_poly a = dsa_tv_ntt_in;
    int k = 0;
    for (int len = 128; len > 0; len >>= 1) {
        for (int start = 0; start < 256; start = start + 2 * len) {
            int32_t zeta = zetas_DSA[++k];
            for (int j = start; j < start + len; ++j) {
                bfu_result_t out = bfu_unit((uint32_t)a.coeffs[j], (uint32_t)a.coeffs[j+len], zeta, OP_BFNTTD);
                a.coeffs[j] = (int32_t)out.rd_lo;
                a.coeffs[j+len] = (int32_t)out.rd_hi;
            }
        }
    }
    int ok = 1;
    for (int i = 0; i < 256; i++) ok &= compare_coeffs("D-NTT-BFU", i, a.coeffs[i], dsa_tv_ntt_out.coeffs[i]);
    return ok;
}

int test_kyber_intt_bfu() {
    printf("\n--- Kyber INTT (32-bit SISO BFU) ---\n");
    kyber_poly r = kyber_tv_intt_in;
    int k = 127;
    for (int len = 2; len <= 128; len <<= 1) {
        for (int start = 0; start < 256; start = start + 2 * len) {
            int16_t zeta = zetas_KEM[k--];
            for (int j = start; j < start + len; j++) {
                uint32_t rs1 = ((uint32_t)(uint16_t)r.coeffs[j+len] << 16) | (uint32_t)(uint16_t)r.coeffs[j];
                bfu_result_t out = bfu_unit(rs1, 0, (int32_t)zeta, OP_BFINTTK);
                r.coeffs[j] = (int16_t)(out.rd_lo & 0xFFFF);
                r.coeffs[j+len] = (int16_t)(out.rd_lo >> 16);
            }
        }
    }

    //for (int j = 0; j < 256; j++) {
    //    // Here we reuse the BFU's internal logic for standalone modular multiplication
    //    // Note: The hardware would do this as: r[j] = fqmul(r[j], f)
    //    // For emulation, we'll manually call the 16-bit reduction
    //    int32_t prod = (int32_t)r.coeffs[j] * f;
    //    int16_t t = (int16_t)prod * KEM_QINV;
    //    r.coeffs[j] = (int16_t)((prod - (int32_t)t * KEM_Q) >> 16);
    //}

    for (int j = 0; j < 256; j++) {
        // rs1 = current coeff, rs2 = scaling factor f (1441)
        bfu_result_t out = bfu_unit((uint32_t)r.coeffs[j], 1441, 0, OP_MQMULK);
        r.coeffs[j] = (int16_t)out.rd_lo;
    }
    
    int ok = 1;
    for (int i = 0; i < 256; i++) ok &= compare_coeffs("K-INTT-BFU", i, r.coeffs[i], kyber_tv_intt_out.coeffs[i]);
    return ok;
}

int test_dilithium_intt_bfu() {
    printf("\n--- Dilithium INTT (32-bit BFU Emulator) ---\n");
    dsa_poly a = dsa_tv_intt_in;
    int k = 256; // Start at the end of the zeta table
    const int32_t f = 41978; // Scaling factor: mont^2/256

    // 1. Inverse NTT GS-Butterfly Stages
    for (int len = 1; len < 256; len <<= 1) {
        for (int start = 0; start < 256; start = start + 2 * len) {
            // Note: Dilithium INTT uses negative zetas
            int32_t zeta = -zetas_DSA[--k];
            for (int j = start; j < start + len; ++j) {
                uint32_t rs1 = (uint32_t)a.coeffs[j];
                uint32_t rs2 = (uint32_t)a.coeffs[j+len];
                
                bfu_result_t out = bfu_unit(rs1, rs2, zeta, OP_BFINTTD);

                a.coeffs[j]     = (int32_t)out.rd_lo;
                a.coeffs[j+len] = (int32_t)out.rd_hi;
            }
        }
    }

    // 2. Final Scaling Stage (Hardware would use fqmul32 instruction)
    //for (int j = 0; j < 256; ++j) {
    //    int64_t prod = (int64_t)a.coeffs[j] * f;
    //    int32_t t = (int32_t)prod * (int32_t)DSA_QINV;
    //    a.coeffs[j] = (int32_t)((prod - (int64_t)t * DSA_Q) >> 32);
    //}
    for (int j = 0; j < 256; ++j) {
        // rs1 = current coeff, rs2 = scaling factor f (41978)
        bfu_result_t out = bfu_unit((uint32_t)a.coeffs[j], 41978, 0, OP_MQMULD);
        a.coeffs[j] = (int32_t)out.rd_lo;
    }

    int ok = 1;
    for (int i = 0; i < 256; i++) 
        ok &= compare_coeffs("D-INTT-BFU-32", i, a.coeffs[i], dsa_tv_intt_out.coeffs[i]);
    return ok;
}




int test_mq_montymul_bfu() {
    printf("\n--- Falcon mq_montymul (BFU) ---\n");
    int success = 1;
    for (int i = 0; i < 100; i++) {
        bfu_result_t out = bfu_unit(falcon_source1[i], falcon_source0[i], 0, OP_MQMULF);
        success &= compare_coeffs("F-MUL-BFU", i, (int32_t)out.rd_lo, (int32_t)falcon_golden[i]);
    }
    return success;
}

int test_karats_bfu() {
    printf("\n--- HQC Karatsuba 64x64 (BFU) ---\n");
    int ok = 1;
    for (int i = 0; i < NUM_HQC_TESTS; i++) {
        bfu_result_t out1 = bfu_unit((uint32_t)hqc_a[i], (uint32_t)hqc_b[i], 0, OP_KARATS_1);
        bfu_result_t out2 = bfu_unit((uint32_t)(hqc_a[i] >> 32), (uint32_t)(hqc_b[i] >> 32), 0, OP_KARATS_2);
        bfu_result_t out3 = bfu_unit((uint32_t)hqc_a[i], (uint32_t)hqc_b[i], 0, OP_KARATS_3);
        uint64_t c0 = out1.rd_lo | ((uint64_t)out3.rd_lo << 32);
        uint64_t c1 = out3.rd_hi | ((uint64_t)out2.rd_hi << 32);
        if (c0 != hqc_out_ref[i].val[0] || c1 != hqc_out_ref[i].val[1]) ok = 0;

        uint32_t a_lo = (uint32_t)hqc_a[i];
        uint32_t a_hi = (uint32_t)(hqc_a[i] >> 32);
        uint32_t b_lo = (uint32_t)hqc_b[i];
        uint32_t b_hi = (uint32_t)(hqc_b[i] >> 32);
        // Print inputs (split 64-bit into two 32-bit prints)
        //printf("[%d] Inputs:\n", i);
        //printf("     a    = 0x%08X%08X\n", a_hi, a_lo);
        //printf("     b    = 0x%08X%08X\n", b_hi, b_lo);
        ////Print intermediate results from each instruction
        //printf("     KARATS_1 rd (P0_lo, c0_lo)     = 0x%08X\n", out1.rd_lo);
        //printf("     KARATS_2 rd (P2_hi, c1_hi)     = 0x%08X\n", out2.rd_lo);
        //printf("     KARATS_3 rd (c0_hi)            = 0x%08X\n", out3.rd_lo);
        //printf("     KARATS_4 rd (c1_lo)            = 0x%08X\n", out3.rd_hi);
    }
    return ok;
}

int test_gf_carryless_bfu() {
    printf("\n--- HQC 8-bit Carry-less (BFU) ---\n");
    int ok = 1;
    for (int i = 0; i < NTESTS_8BIT; i++) {
        bfu_result_t out = bfu_unit(hqc_a_8bit[i], hqc_b_8bit[i], 0, OP_GFMUL8);
        if ((uint8_t)out.rd_lo != hqc_c0_expected[i] || (uint8_t)(out.rd_lo >> 8) != hqc_c1_expected[i]) ok = 0;
    }
    return ok;
}


int test_fqmul_bfu() {
    printf("\n--- Kyber fqmul (Montgomery Reduction) Test (BFU) ---\n");

    fqmul_test_vector tests[] = {
        {-758, -2, -129},
        {-758,  0,    0},
        {-758, -1, 1600},
        {-758,  1, -1600},
        {-758,  2,  129},
        {-758, -3, 1471},
        {-758,  3, -1471}
    };

    int num_tests = 5;
    int ok = 1;

    for (int i = 0; i < num_tests; i++) {
        // Call the software implementation
        //int16_t result = fqmul(tests[i].a, tests[i].b);
        bfu_result_t out = bfu_unit(tests[i].a, tests[i].b, 0, OP_MQMULK);
        int16_t result = (int16_t)out.rd_lo;

        // Check result
        if (result != tests[i].expected) {
            printf("  [FAIL] Test %d: fqmul(%d, %d)\n", i, tests[i].a, tests[i].b);
            printf("         Expected: %d, Got: %d\n", tests[i].expected, result);
            ok = 0;
        } else {
            // Optional: verbose success output
            //printf("  [OK] fqmul(%5d, %5d) = %5d\n", tests[i].a, tests[i].b, result);
        }
    }

    if (ok) {
        printf("\n--- fqmul Test PASSED (%d cases) ---\n", num_tests);
    } else {
        printf("\n--- fqmul Test FAILED ---\n");
    }

    return ok;
}

int test_dilithium_reduce32_bfu() {
    int ok = 1;
    for (int i = 0; i < 20; i++) {
        // Call BFU with OP_RED32. rs2 and zeta are unused (0).
        bfu_result_t res = bfu_unit((uint32_t)dsa_tv_scalars[i].input, 0, 0, OP_RED32);
        
        int32_t got = (int32_t)res.rd_lo;
        int32_t expected = dsa_tv_scalars[i].expected_reduce;
        
        if (got != expected) {
            printf("FAILED: D-RED32-BFU [%d] Input %d: Got %d, Expected %d\n", 
                   i, dsa_tv_scalars[i].input, got, expected);
            ok = 0;
        }
    }
    if (ok) printf("\nPASSED: Dilithium reduce32 (BFU)\n");
    return ok;
}

int test_dilithium_caddq_bfu() {
    int ok = 1;
    for (int i = 0; i < 20; i++) {
        // Call BFU with OP_CADDQ.
        bfu_result_t res = bfu_unit((uint32_t)dsa_tv_scalars[i].input, 0, 0, OP_CADDQ);
        
        int32_t got = (int32_t)res.rd_lo;
        int32_t expected = dsa_tv_scalars[i].expected_caddq;
        
        if (got != expected) {
            printf("FAILED: D-CADDQ-BFU [%d] Input %d: Got %d, Expected %d\n", 
                   i, dsa_tv_scalars[i].input, got, expected);
            ok = 0;
        }
    }
    if (ok) printf("\nPASSED: Dilithium caddq (BFU)\n");
    return ok;
}