#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "api.h"
#include "crypto_aead.h"

#ifdef PERF_CNT_CYCLES
    #include "core_v_mini_mcu.h"
    #include "csr.h"
#endif


void init_buffer(unsigned char* buffer, unsigned long long numbytes);

void init_buffer(unsigned char* buffer, unsigned long long numbytes) {
  unsigned long long i;
  for (i = 0; i < numbytes; i++) buffer[i] = (unsigned char)i;
}


int main() {

    unsigned char* msg;
    unsigned char* msg2;
    unsigned char* ad;
    unsigned char* ct;
    unsigned long long mlen, adlen;
    unsigned long long clen, mlen2;

    unsigned char key[CRYPTO_KEYBYTES];
    unsigned char nonce[CRYPTO_NPUBBYTES];

    #if PERF_CNT_CYCLES
        unsigned int cycles, cycles2;
    #endif
    

    init_buffer(key, sizeof(key));
    init_buffer(nonce, sizeof(nonce));

    mlen=128;
    msg = malloc(mlen);
    msg2 = malloc(mlen);
    ct = malloc(mlen + CRYPTO_ABYTES);
    init_buffer(msg, mlen);

    ad=0;
    ad = malloc(adlen);
    init_buffer(ad, adlen);

    printf("Test started!\n");
    #ifdef PERF_CNT_CYCLES
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    crypto_aead_encrypt(ct, &clen, msg, mlen, ad, adlen, NULL, nonce, key);
    #ifdef PERF_CNT_CYCLES
        CSR_READ(CSR_REG_MCYCLE, &cycles);
        printf("Number of clock cycles for encryption: %d\n", cycles);
    #endif

    #ifdef PERF_CNT_CYCLES
        CSR_CLEAR_BITS(CSR_REG_MCOUNTINHIBIT, 0x1);
        CSR_WRITE(CSR_REG_MCYCLE, 0);
    #endif
    crypto_aead_decrypt(msg2, &mlen2, NULL, ct, clen, ad, adlen, nonce, key);
    #ifdef PERF_CNT_CYCLES
        CSR_READ(CSR_REG_MCYCLE, &cycles2);
        printf("Number of clock cycles for encryption: %d\n", cycles2);
    #endif

    //printf("Original msg: ");
    //for (int i=0; i<mlen; i++){
    //    printf("%02X", msg[i]);
    //}
    //    printf("\n");
    //printf("Decrypt msg: ");
    //for (int i=0; i<mlen; i++){
    //    printf("%02X", msg2[i]);
    //}
    //    printf("\n");
    
    if (mlen != mlen2) {
        printf("Crypto_aead_decrypt returned bad 'mlen': Got <%" PRIu32">, expected <%" PRIu32 ">\n", (uint32_t)mlen2, (uint32_t)mlen);
        free(ad);
    }

    if (memcmp(msg, msg2, mlen)) {
        printf("Crypto_aead_decrypt did not recover the plaintext\n");
        free(ad);
    }
    else{
        printf("Crypto_aead_decrypt recover the plaintext\n");
    }

    free(msg);
    free(msg2);
    free(ct);


    printf("Test: terminated\n");
    return 0;
}

