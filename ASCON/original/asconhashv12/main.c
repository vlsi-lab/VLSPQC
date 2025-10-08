#include <stdio.h>
#include <string.h>
#include "crypto_hash.h"

#ifdef PERF_CNT_CYCLES
    #include "core_v_mini_mcu.h"
    #include "csr.h"
#endif


#define CRYPTO_BYTES 32  // Assume the hash output size is 32 bytes

int main() {
    // Example input data
    const unsigned char input[CRYPTO_BYTES] = { 0x50, 0xE3, 0x66, 0xAB, 0x09, 0x18, 0x56, 0xCF, 0xE1, 0xE2, 0x15, 0x90, 0x75, 0xCE, 0x88, 0x65,
    0x50, 0x79, 0x4B, 0xA3, 0x37, 0xF8, 0xFA, 0x88, 0x0E, 0x1A, 0x0D, 0x71, 0x99, 0x51, 0x68, 0x7B};

    unsigned long long input_len = 32;
    //strlen(input);
    
    // Buffer to store the hash output
    unsigned char hash_output[CRYPTO_BYTES];
    
    // Golden result to compare with (the expected hash output)
    unsigned char golden_result[CRYPTO_BYTES] = { 0x78, 0xd2, 0x2a, 0x55, 0x9d, 0x50, 0xfc, 0x0b, 0x0b, 0xab, 0x66, 0x53, 0x80, 0xa9, 0x09, 0x54, 0x5f, 0x53, 0xf7, 0xe4, 0xeb, 0xd5, 0x09, 0x96, 0xe9, 0x80, 0x59, 0x46, 0x4d, 0xc8, 0x15, 0x8d};
    
    #if PERF_CNT_CYCLES
        unsigned int cycles;
    #endif
    
    printf("Test started!\n");
    #ifdef PERF_CNT_CYCLES
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    crypto_hash(hash_output, input, input_len);
    #ifdef PERF_CNT_CYCLES
        CSR_READ(CSR_REG_MCYCLE, &cycles);
        printf("Number of clock cycles for test-1 : %d\n", cycles);
    #endif
    
    // Print the hash output
    //printf("Hash output is:\n");
    //for (int i = 0; i < CRYPTO_BYTES; i++) {
    //    printf("%02x", hash_output[i]);
    //}
    //printf("\n");
//
    //printf("Golden model is:\n");
    //for (int i = 0; i < CRYPTO_BYTES; i++) {
    //    printf("%02x", golden_result[i]);
    //}
    //printf("\n");
    
    // Compare with the golden result
    if (memcmp(hash_output, golden_result, CRYPTO_BYTES) == 0) {
        printf("Hash matches the golden result!\n");
    } else {
        printf("Hash does NOT match the golden result.\n");
    }
    
    printf("Test: terminated\n");

    return 0;
}
