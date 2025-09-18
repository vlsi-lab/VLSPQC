// ****************************************************************************
//
// Name: VLSPQC
// Auth: Alessandra Dolmeta, Valeria Piscopo - Politecnico di Torino
// Date: September 2025
//
// Algorithm: HQC-192
//
// ****************************************************************************

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "parameters.h"
#include "api.h"

#include TEST_VECT_INCLUDE
#define TEST_KEY        1
#define TEST_ENC        1
#define TEST_DEC        1

uint8_t pk_seed[SEED_BYTES]; 
uint8_t sk_seed[SEED_BYTES]; 
uint64_t m[VEC_K_SIZE_64];


#ifdef PRINT_VECT
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
    uint8_t pk[CRYPTO_PUBLICKEYBYTES];
    uint8_t sk[CRYPTO_SECRETKEYBYTES];
    uint8_t ct[CRYPTO_CIPHERTEXTBYTES];
    uint8_t key_a[CRYPTO_BYTES];
    uint8_t key_b[CRYPTO_BYTES];

    printf("Started test.\n");

    char test_str[50] = "Testing";
    #ifdef TEST_KEY
        strcat(test_str," keypair");
    #endif
    #ifdef TEST_ENC
        strcat(test_str," encaps.");
    #endif
    #ifdef TEST_DEC
        strcat(test_str," decaps.");
    #endif
    printf("%s\n", test_str);

    //************************************************* 
    // KEY
    //*************************************************
    #ifdef TEST_KEY
        // Filling coin vector with kem_keypair seeds
        memcpy(sk_seed , TVEC_IN_SK_SEED[0], SEED_BYTES);
        memcpy(pk_seed , TVEC_IN_PK_SEED[0], SEED_BYTES);
            
        //Alice generates a public key
        crypto_kem_keypair(pk, sk, sk_seed, pk_seed);

        if(memcmp(pk, TVEC_OUT_PK[0], CRYPTO_PUBLICKEYBYTES)) { printf("\nERROR: PK mismatch\n"); return -1;}
        if(memcmp(sk, TVEC_OUT_SK[0], CRYPTO_SECRETKEYBYTES)) { printf("\nERROR: SK mismatch\n"); return -1;}
    #endif /* TEST_KEY */   

    //************************************************* 
    // ENCAPSULATION
    //*************************************************     
    #ifdef TEST_ENC
        memcpy(m, TVEC_M_ENC[0], CRYPTO_BYTES*2);       
    #ifndef TEST_KEY
        memcpy(pk, TVEC_OUT_PK[0], CRYPTO_PUBLICKEYBYTES);
    #endif  

    crypto_kem_enc(ct, key_b, pk, m);
    
    if(memcmp(ct, TVEC_OUT_CT[0], CRYPTO_CIPHERTEXTBYTES)) { printf("ERROR: CT mismatch\n"); return -1;}
    if(memcmp(key_b, TVEC_OUT_SS[0], CRYPTO_BYTES)) { printf("ERROR: SS mismatch\n"); return -1;}
    #endif /* TEST_ENC */

    //************************************************* 
    // DECAPSULATION
    //*************************************************

    #ifdef TEST_DEC
        #ifndef TEST_KEY
                memcpy(sk, TVEC_OUT_SK[0], CRYPTO_SECRETKEYBYTES);
        #endif 
        #ifndef TEST_ENC
                memcpy(ct, TVEC_OUT_CT[0], CRYPTO_CIPHERTEXTBYTES);
        #endif 

        crypto_kem_dec(key_a, ct, sk);

        if(memcmp(key_a, TVEC_OUT_SS[0], CRYPTO_BYTES)) { printf("ERROR: SS mismatch\n"); return -1;}
        #endif /* TEST_DEC */

        #ifdef PRINT_VECT
                printVect("pk", pk, CRYPTO_PUBLICKEYBYTES);
                printVect("sk", sk, CRYPTO_SECRETKEYBYTES);
                printVect("ct", ct, CRYPTO_CIPHERTEXTBYTES);
                printVect("key_a", key_a, CRYPTO_BYTES);
                printVect("key_b", key_b, CRYPTO_BYTES);
                printf("\n");
        #endif 


    printf("Test Successful\n");

    return 0;
}
