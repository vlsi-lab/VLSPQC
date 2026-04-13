#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CRYPTO_AEAD

#include "api.h"
#if defined(AVR_UART)
#include "avr_uart.h"
#endif
#if defined(CRYPTO_AEAD)
#include "crypto_aead.h"

#elif defined(CRYPTO_HASH)
#include "crypto_hash.h"
#elif defined(CRYPTO_AUTH)
#include "crypto_auth.h"
#endif


void print(unsigned char c, unsigned char* x, unsigned long long xlen) {
  unsigned long long i;
  printf("%c[%d]=", c, (int)xlen);
  for (i = 0; i < xlen; ++i) printf("%02x", x[i]);
  printf("\n");
}

int main() {
unsigned int cycles;

  unsigned char n[32] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                         11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                         22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
  unsigned char k[32] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                         11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                         22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
  unsigned char a[32] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                         11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                         22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
  unsigned char m[32] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                         11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                         22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
  unsigned char c[32], h[32], t[32];



  unsigned long long alen = 16;
  unsigned long long mlen = 16;
  unsigned long long clen, mlen2;

  int result = 0;


  printf("input parameters:\n");
  print('k', k, CRYPTO_KEYBYTES);   //Secret-key
  print('n', n, CRYPTO_NPUBBYTES);  //Public-nonce
  print('a', a, alen);              //Associated data  
  print('m', m, mlen);              //Plaintext message


  result = crypto_aead_encrypt(c, &clen, m, mlen, a, alen, 0, n, k);

  print('c', c, clen - CRYPTO_ABYTES);
  print('t', c + clen - CRYPTO_ABYTES, CRYPTO_ABYTES);

  result |= crypto_aead_decrypt(c, &mlen2, 0, c, clen, a, alen, n, k);


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
  }

  if (memcmp(m, c, mlen)) {
      printf("❌ Crypto_aead_decrypt did not recover the plaintext\n");
  }
  else{
      printf("✅ Crypto_aead_decrypt recover the plaintext\n");
    }

  return 0;
  
}