#include "tests.h"
#include <stdio.h>



int main() {
    int all_passed = 1;

    printf("======================================\n");
    printf("   PQ-CRYPTO SOFTWARE MODULE TESTS    \n");
    printf("======================================\n");

    //all_passed &= test_dilithium_ntt_sw();
    //all_passed &= test_dilithium_intt_sw();
    //all_passed &= test_kyber_ntt_sw();
    //all_passed &= test_kyber_intt_sw();
    //all_passed &= test_mq_montymul_sw();
    //all_passed &= test_karats_sw();
    //all_passed &= test_gf_carryless_sw();
    //all_passed &= test_fqmul_sw();
    all_passed &= test_dilithium_reduce32();
    all_passed &= test_dilithium_caddq();

    printf("\n======================================\n");
    printf("   BFU HARDWARE EMULATOR TESTS       \n");
    printf("======================================\n");

    all_passed &= test_dilithium_ntt_bfu();
    all_passed &= test_dilithium_intt_bfu();
    all_passed &= test_kyber_ntt_bfu();
    all_passed &= test_kyber_intt_bfu();
    all_passed &= test_mq_montymul_bfu();
    all_passed &= test_karats_bfu();
    all_passed &= test_gf_carryless_bfu();
    all_passed &= test_fqmul_bfu();
    all_passed &= test_dilithium_reduce32_bfu();
    all_passed &= test_dilithium_caddq_bfu();

    printf("\n======================================\n");
    if (all_passed) {
        printf("FINAL STATUS: ALL TESTS PASSED\n");
    } else {
        printf("FINAL STATUS: TEST FAILURE DETECTED\n");
    }
    printf("======================================\n");

    return all_passed ? 0 : 1;
}