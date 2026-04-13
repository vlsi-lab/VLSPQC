///////////////////////////////////////////////////////////////////////////////////////
//                                                                                   //
// Auth: Alessandra Dolmeta, Valeria Piscopo - Politecnico di Torino                 //
// Date: September 2025                                                              //
// Desc: Entry point for testing the ML-DSA implementation using input and output    //
//       test vector coming from NIST                                                //
//                                                                                   //
///////////////////////////////////////////////////////////////////////////////////////




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "api.h"

#include "test_vectors_shake256_128f_robust.h"

#ifdef PERF_CNT_CYCLES
    #include "core_v_mini_mcu.h"
    #include "csr.h"
#endif


#define TEST_KEY  1
#define TEST_SIGN  1
#define TEST_SIGN_OPEN 1




#define MLEN_KAT 33
uint8_t keypair_rnd[CRYPTO_SEEDBYTES];
uint8_t signature_rnd[SPX_N];

#if PRINT_VECT
void printVect(char* name, uint8_t* buf, size_t size) {
    printf("%s = ", name);
    for (int i=0; i<size; i++){
        printf("%02X", buf[i]);
    }
    printf("\n");
}
#endif


int main(void)
{

    uint8_t sk[CRYPTO_SECRETKEYBYTES];
    uint8_t pk[CRYPTO_PUBLICKEYBYTES];

    uint8_t m[MLEN_KAT];
    uint8_t sm[MLEN_KAT + CRYPTO_BYTES];
    uint8_t m1[MLEN_KAT + CRYPTO_BYTES];

        
    unsigned long long  mlen, smlen, mlen1;
    int r;
    size_t i, k;

    char test_str[50] = "Testing";
    #if TEST_KEY
        strcat(test_str," keypair");
    #endif
    #if TEST_SIGN
        strcat(test_str," sign.");
    #endif
    #if TEST_SIGN_OPEN
        strcat(test_str," sign_open.");
    #endif
        printf("%s\n", test_str);

    //************************************************* 
    // KEY
    //*************************************************

    #if TEST_KEY
        // Filling coin vector with indcpa_keypair and kem_keypair seeds
        memcpy(keypair_rnd , TVEC_IN_SIGN_KEYPAIR[i], CRYPTO_SEEDBYTES);
        
         #if PERF_CNT_CYCLES == 1
          CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif
            
        crypto_sign_keypair(pk, sk, keypair_rnd);
        
         #if PERF_CNT_CYCLES == 1
          CSR_READ(CSR_REG_MCYCLE, &cycles_keygen);
          printf("Keygen cycles: %u\n", cycles_keygen);
        #endif

        if(memcmp(pk, TVEC_OUT_PK[i], CRYPTO_PUBLICKEYBYTES)) { printf("ERROR: PK mismatch\n"); return -1;}
        if(memcmp(sk, TVEC_OUT_SK[i], CRYPTO_SECRETKEYBYTES)) { printf("ERROR: SK mismatch\n"); return -1;}
    #endif /* TEST_KEY */

    //************************************************* 
    // SIGN
    //*************************************************
    #if TEST_SIGN
        // Setting up Input test vectors
        memcpy(m, TVEC_IN_M_SIGN[i], MLEN_KAT);
        memcpy(signature_rnd , TVEC_IN_SIGN_SIGNATURE[i], SPX_N);

        #if TEST_KEY == 0
          // In case we do not generate PK in this test we take it from the test vectors
          memcpy(sk, TVEC_OUT_SK[i], CRYPTO_SECRETKEYBYTES);
        #endif /* TEST_KEY */

         #if PERF_CNT_CYCLES == 1
          CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif

        crypto_sign(sm, &smlen, m, MLEN_KAT, sk, signature_rnd);
        
         #if PERF_CNT_CYCLES == 1
          CSR_READ(CSR_REG_MCYCLE, &cycles_sign);
          printf("Sign cycles: %u\n", cycles_sign);
        #endif

        if(memcmp(sm, TVEC_IN_SM_SIGN[i], SPX_BYTES)) { printf("ERROR: SM mismatch\n"); return -1;}
        //printf("mlen1: %llu\n", SPX_BYTES);
        //printf("sm: ");
        //for (int i = 0; i < SPX_BYTES; i++) {
        //    printf("%02x", sm[i]);
        //}
        //printf("\n");

    #endif /* TEST_SIGN */
    
    //************************************************* 
    // IGN_OPEN
    //*************************************************
    #if TEST_SIGN_OPEN

        #if TEST_KEY == 0
          // In case we do not generate PK in this test we take it from the test vectors
          memcpy(pk, TVEC_OUT_PK[i], CRYPTO_PUBLICKEYBYTES);
        #endif /* TEST_KEY */

        #if TEST_SIGN == 0
          memcpy(sm, TVEC_IN_SM_SIGN[i], 17121);
        #endif /* TEST_SIGN */
        
         #if PERF_CNT_CYCLES == 1
          CSR_WRITE(CSR_REG_MCYCLE, 0);
        #endif

        crypto_sign_open(m1, &mlen1, sm, 17121, pk);
        
         #if PERF_CNT_CYCLES == 1
          CSR_READ(CSR_REG_MCYCLE, &cycles_sign_open);
          printf("Sign open cycles: %u\n", cycles_sign_open);
        #endif

        if(memcmp(m1, TVEC_IN_M_SIGN[i], MLEN_KAT)) { printf("ERROR: M mismatch\n"); return -1;}

    #endif /* TEST_SIGN_OPEN */
    
    printf("OK\n");

    printf("Test Successful\n");

    return 0;


}

