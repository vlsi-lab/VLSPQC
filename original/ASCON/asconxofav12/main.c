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
    const unsigned char input[CRYPTO_BYTES-1] = {
        0x29, 0x73, 0x82, 0x3A, 0xEE, 0x04, 0x1C, 0x87, 0x60, 0x21, 0x64, 0x87, 0x41, 0x43, 0x41, 0xC8,
        0x81, 0xA5, 0x77, 0x04, 0xA5, 0x91, 0x95, 0xD2, 0x6E, 0x3E, 0x5A, 0x06, 0x12, 0xD5, 0x04
    };
    unsigned long long input_len = 31;
    
    // Buffer to store the hash output
    unsigned char hash_output[CRYPTO_BYTES];
    
    // Golden result to compare with (the expected hash output)
    unsigned char golden_result[CRYPTO_BYTES] = {
        0xDA, 0x63, 0x3D, 0x1B, 0xA3, 0x05, 0xDA, 0x7D, 0x64, 0xA3, 0xD7, 0xA3, 0xD5, 0x55, 0x00, 0x3C,
        0x7D, 0x11, 0xAF, 0x7B, 0xCF, 0x7A, 0x6E, 0x87, 0x51, 0xD0, 0x61, 0xDE, 0x23, 0x4B, 0xC1, 0x2B
    };


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
    
    //printf("XOF output is:\n");
    //for (int i = 0; i < CRYPTO_BYTES; i++) {
    //    printf("%02X", hash_output[i]);
    //}
    //printf("\n");
//
    //printf("Golden result is:\n");
    //for (int i = 0; i < CRYPTO_BYTES; i++) {
    //    printf("%02X", golden_result[i]);
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
