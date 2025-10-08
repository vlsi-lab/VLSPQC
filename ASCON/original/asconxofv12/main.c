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
    unsigned char input[CRYPTO_BYTES-1] = {0xD4, 0xD2, 0x7D, 0x38, 0x95, 0x6E, 0xFF, 0x69, 0x0E, 0xE9, 0x5E, 0xFE, 0x7D, 0x5F, 0x5A, 0x17,
    0x02, 0x90, 0xCE, 0x1C, 0x1D, 0x84, 0xBB, 0x11, 0x29, 0xCA, 0x23, 0xBA, 0xD6, 0xCC, 0x4C};
    unsigned long long input_len = 31;
    
    // Buffer to store the hash output
    unsigned char hash_output[CRYPTO_BYTES];
    
    // Golden result to compare with (the expected hash output)
    unsigned char golden_result[CRYPTO_BYTES] = { 0x2C, 0xBF, 0x66, 0x8C, 0x85, 0x3C, 0x00, 0xFC, 0xB5, 0xFD, 0xD9, 0x52, 0xD3, 0xBE, 0x9D, 0x93,
    0x10, 0x4B, 0xF1, 0x3A, 0x2C, 0x3D, 0x88, 0x34, 0x4B, 0x2C, 0x1C, 0x29, 0x5C, 0x37, 0x1B, 0xF5};

    #if PERF_CNT_CYCLES
        unsigned int cycles;
    #endif
    
    printf("Test started!\n");
    #ifdef PERF_CNT_CYCLES
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    crypto_hash(hash_output, (const unsigned char*)input, input_len);
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
    //printf("Golden output is:\n");
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
