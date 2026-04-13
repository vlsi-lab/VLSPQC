/*
    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2017 NewAE Technology Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "api.h"
#include "crypto_aead.h"


#ifndef SS_SHARED
#define SS_SHARED 0
#endif

#if SS_SHARED

  #include "crypto_aead_shared.h"
  // number of masked 32-bit structs (with 64-bit padding)
  #define LENGTH(len) ((((len) + 7) / 8) * 2)
  mask_m_uint32_t* m = NULL;
  mask_c_uint32_t* c = NULL;
  mask_ad_uint32_t* ad = NULL;
  mask_npub_uint32_t* n = NULL;
  mask_key_uint32_t* k = NULL;

#else

  // number of bytes
  #define LENGTH(len) (len)

  unsigned char* m = NULL;
  unsigned char* c = NULL;
  unsigned char* ad = NULL;
  unsigned char* n = NULL;
  unsigned char* k = NULL;


#endif



void init_buffer(unsigned char* buffer, unsigned long long numbytes);

void init_buffer(unsigned char* buffer, unsigned long long numbytes) {
  unsigned long long i;
  for (i = 0; i < numbytes; i++) buffer[i] = (unsigned char)i;
}


int main() {

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
    m = malloc(mlen);
    c = malloc(mlen); // + CRYPTO_ABYTES);
    init_buffer(m, mlen);

    ad=0;
    ad = malloc(adlen);
    init_buffer(ad, adlen);

    printf("Test started!\n");

    #if SS_SHARED
      crypto_aead_encrypt(c, &clen, m, mlen, ad, adlen, nonce, key);
    #else
      crypto_aead_encrypt(c, &clen, m, mlen, ad, adlen, NULL, nonce, key);
    #endif
    
    #if SS_SHARED
     crypto_aead_decrypt(c, &mlen2, c, clen, ad, adlen, nonce, key);
    #else
      crypto_aead_decrypt(c, &mlen2, NULL, c, clen, ad, adlen, nonce, key);
    #endif
    


    printf("Original msg: ");
    for (int i=0; i<mlen; i++){
        printf("%02X", m[i]);
    }
        printf("\n");
    printf("Decrypt msg: ");
    for (int i=0; i<mlen; i++){
        printf("%02X", c[i]);
    }
        printf("\n");
    
    if (mlen != mlen2) {
        printf("Crypto_aead_decrypt returned bad 'mlen': Got <%" PRIu32">, expected <%" PRIu32 ">\n", (uint32_t)mlen2, (uint32_t)mlen);
        free(ad);
    }

    if (memcmp(m, c, mlen)) {
        printf("Crypto_aead_decrypt did not recover the plaintext\n");
        free(ad);
    }
    else{
        printf("Crypto_aead_decrypt recover the plaintext\n");
    }

    free(m);
    free(c);


    printf("Test: terminated\n");
    return 0;
}
