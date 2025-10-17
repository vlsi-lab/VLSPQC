#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
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
  unsigned long long clen;
  int result = 0;


  printf("input:\n");
  print('k', k, CRYPTO_KEYBYTES);
  print('n', n, CRYPTO_NPUBBYTES);
  print('a', a, alen);
  print('m', m, mlen);
 


  result = crypto_aead_encrypt(c, &clen, m, mlen, a, alen, 0, n, k);

  print('c', c, clen - CRYPTO_ABYTES);
  print('t', c + clen - CRYPTO_ABYTES, CRYPTO_ABYTES);

  result |= crypto_aead_decrypt(m, &mlen, 0, c, clen, a, alen, n, k);

  print('m', m, mlen);

  return 0;
  
}