#include "profiler.h"
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

#include "test_vectors_shake256_128f_simple.h"
#include "profiler.h"

extern unsigned long long horcrux_gen_chain_calls;
extern unsigned long long horcrux_thash_calls;
extern unsigned long long horcrux_wots_pk_from_sig_calls;
extern unsigned long long horcrux_compute_root_calls;
extern unsigned long long horcrux_treehash_calls;


#define TEST_KEY  1
#define TEST_SIGN  1
#define TEST_SIGN_OPEN 1

#define MLEN_KAT 33
uint8_t keypair_rnd[CRYPTO_SEEDBYTES];
uint8_t signature_rnd[SPX_N];

#define CRYPTO_SECRETKEYBYTES SPX_SK_BYTES
#define CRYPTO_PUBLICKEYBYTES SPX_PK_BYTES
#define CRYPTO_BYTES SPX_BYTES
#define CRYPTO_SEEDBYTES 3*SPX_N

void printVect(char* name, uint8_t* buf, size_t size) {
	PQC_PROFILE_FUNCTION();
    printf("%s = ", name);
    for (int i=0; i<size; i++){
        printf("%02X", buf[i]);
    }
    printf("\n");
}

int main(void)
{
	PQC_PROFILE_FUNCTION();

    uint8_t sk[CRYPTO_SECRETKEYBYTES];
    uint8_t pk[CRYPTO_PUBLICKEYBYTES];

    uint8_t m[MLEN_KAT];
    uint8_t sm[MLEN_KAT + CRYPTO_BYTES];
    uint8_t m1[MLEN_KAT + CRYPTO_BYTES];

        
    unsigned long long  mlen, smlen, mlen1;

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
        memcpy(keypair_rnd , TVEC_IN_SIGN_KEYPAIR, 48);
        pqc_profile_reset();
    
        crypto_sign_keypair(pk, sk, keypair_rnd);
        printf("Keygen function calls:\n");
        pqc_profile_dump(stdout);

        if(memcmp(pk, TVEC_OUT_PK, CRYPTO_PUBLICKEYBYTES)) { printf("ERROR: PK mismatch\n"); }
        if(memcmp(sk, TVEC_OUT_SK, CRYPTO_SECRETKEYBYTES)) { printf("ERROR: SK mismatch\n");}
        //printVect("pk", pk, CRYPTO_PUBLICKEYBYTES);
        //printVect("sk", sk, CRYPTO_SECRETKEYBYTES);
    #endif /* TEST_KEY */

    //************************************************* 
    // SIGN
    //*************************************************
    #if TEST_SIGN
        // Setting up Input test vectors
        memcpy(m, TVEC_IN_M_SIGN, MLEN_KAT);
        memcpy(signature_rnd , TVEC_IN_SIGN_SIGNATURE, SPX_N);

        #if TEST_KEY == 0
          // In case we do not generate PK in this test we take it from the test vectors
          memcpy(sk, TVEC_OUT_SK, CRYPTO_SECRETKEYBYTES);
        #endif /* TEST_KEY */

                pqc_profile_reset();

        crypto_sign(sm, &smlen, m, MLEN_KAT, sk, signature_rnd);
                printf("Sign function calls:\n");
                pqc_profile_dump(stdout);

        if(memcmp(sm, TVEC_IN_SM_SIGN, SPX_BYTES)) { printf("ERROR: SM mismatch\n");}
        //printf("mlen1: %llu\n", SPX_BYTES);
        //printf("sm: ");
        //for (int i = 0; i < SPX_BYTES; i++) {
	PQC_PROFILE_FUNCTION();
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
          memcpy(pk, TVEC_OUT_PK, CRYPTO_PUBLICKEYBYTES);
        #endif /* TEST_KEY */

        #if TEST_SIGN == 0
          memcpy(sm, TVEC_IN_SM_SIGN, 17121);
        #endif /* TEST_SIGN */

                pqc_profile_reset();
      
        crypto_sign_open(m1, &mlen1, sm, 17121, pk);
                printf("Sign open function calls:\n");
                pqc_profile_dump(stdout);

        if(memcmp(m1, TVEC_IN_M_SIGN, MLEN_KAT)) { printf("ERROR: M mismatch\n");}

    #endif /* TEST_SIGN_OPEN */

    printf("HORCRUX gen_chain %llu\n", horcrux_gen_chain_calls);
    printf("HORCRUX thash %llu\n", horcrux_thash_calls);
    printf("HORCRUX wots_pk_from_sig %llu\n", horcrux_wots_pk_from_sig_calls);
    printf("HORCRUX compute_root %llu\n", horcrux_compute_root_calls);
    printf("HORCRUX treehash %llu\n", horcrux_treehash_calls);
    
    printf("OK\n");

    printf("Test Successful\n");

    return 0;


}

