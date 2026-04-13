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
    const unsigned char input[CRYPTO_BYTES] = {
    0x9E, 0x58, 0xD4, 0x06, 0xA2, 0x9A, 0x43, 0xBC, 0x3D, 0xF8, 0x08, 0xC1, 0xD3, 0x58, 0xCD, 0x10,
    0x8E, 0x8D, 0x94, 0x94, 0x8E, 0x55, 0x0E, 0xE3, 0xAD, 0xE0, 0x7F, 0x7E, 0xBC, 0x79, 0x90, 0xF4
    };
    unsigned long long input_len = CRYPTO_BYTES;
    
    // Buffer to store the hash output
    unsigned char hash_output[CRYPTO_BYTES];
    
    // Golden result to compare with (the expected hash output)
    unsigned char golden_result[CRYPTO_BYTES] = {
    0x16, 0xD5, 0x5C, 0x66, 0x27, 0xA4, 0x1C, 0x45, 0xCB, 0x7C, 0xD3, 0x68, 0x5F, 0xF1, 0x51, 0x2D,
    0x7C, 0x77, 0x3C, 0x80, 0xAB, 0xA7, 0x9E, 0x6B, 0xE8, 0x6F, 0x81, 0xA4, 0x3B, 0xF9, 0xEE, 0x64
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
    
    // Print the hash output
   // printf("Hash output is:\n");
   // for (int i = 0; i < CRYPTO_BYTES; i++) {
   //     printf("%02x", hash_output[i]);
   // }
   // printf("\n");
//
   // printf("Golden result is:\n");
   // for (int i = 0; i < CRYPTO_BYTES; i++) {
   //     printf("%02x", golden_result[i]);
   // }
   // printf("\n");
    
    // Compare with the golden result
    if (memcmp(hash_output, golden_result, CRYPTO_BYTES) == 0) {
        printf("Hash matches the golden result!\n");
    } else {
        printf("Hash does NOT match the golden result.\n");
    }
    
    printf("Test: terminated\n");

    return 0;
}
