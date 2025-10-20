
#include "shares.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void generate_shares(uint32_t* s, int num_shares, const uint8_t* data,
                     uint64_t len);

void combine_shares(uint8_t* data, uint64_t len, const uint32_t* s,
                    int num_shares);

void generate_shares_encrypt(const unsigned char* m, mask_m_uint32_t* ms,
                             const unsigned long long mlen,
                             const unsigned char* ad, mask_ad_uint32_t* ads,
                             const unsigned long long adlen,
                             const unsigned char* npub,
                             mask_npub_uint32_t* npubs, const unsigned char* k,
                             mask_key_uint32_t* ks) {

  generate_shares((uint32_t*)ks, NUM_SHARES_KEY, k, CRYPTO_KEYBYTES);
  generate_shares((uint32_t*)npubs, NUM_SHARES_NPUB, npub, CRYPTO_NPUBBYTES);
  generate_shares((uint32_t*)ads, NUM_SHARES_AD, ad, adlen);
  generate_shares((uint32_t*)ms, NUM_SHARES_M, m, mlen);


  #if DEBUG_CODE==1
    printf("\n========== MASKED SHARE DEBUG ==========\n");

    printf("[KEY SHARES] (%llu bytes)\n", (unsigned long long)CRYPTO_KEYBYTES);
    for (int i = 0; i < NUM_SHARES_KEY; i++) {
      printf("  Share %d:\n    ", i);
      for (size_t j = 0; j < CRYPTO_KEYBYTES / 4; j++) {
        printf("%08X ", ks[j].shares[i]);
      }
      printf("\n");
    }

    printf("\n[NPUB SHARES] (%llu bytes)\n", (unsigned long long)CRYPTO_NPUBBYTES);
    for (int i = 0; i < NUM_SHARES_NPUB; i++) {
      printf("  Share %d:\n    ", i);
      for (size_t j = 0; j < CRYPTO_NPUBBYTES / 4; j++) {
        printf("%08X ", npubs[j].shares[i]);
      }
      printf("\n");
    }

    printf("\n[AD SHARES] (%llu bytes)\n", (unsigned long long)adlen);
    for (int i = 0; i < NUM_SHARES_AD; i++) {
      printf("  Share %d:\n    ", i);
      for (size_t j = 0; j < adlen / 4; j++) {
        printf("%08X ", ads[j].shares[i]);
      }
      printf("\n");
    }

    printf("\n[MESSAGE SHARES] (%llu bytes)\n", (unsigned long long)mlen);
    for (int i = 0; i < NUM_SHARES_M; i++) {
      printf("  Share %d:\n    ", i);
      for (size_t j = 0; j < mlen / 4; j++) {
        printf("%08X ", ms[j].shares[i]);
      }
      printf("\n");
    }

    printf("========================================\n\n");
  #endif
}


void generate_shares_decrypt(const unsigned char* c, mask_c_uint32_t* cs,
                             const unsigned long long clen,
                             const unsigned char* ad, mask_ad_uint32_t* ads,
                             const unsigned long long adlen,
                             const unsigned char* npub,
                             mask_npub_uint32_t* npubs, const unsigned char* k,
                             mask_key_uint32_t* ks) {
  unsigned long long mlen = clen - CRYPTO_ABYTES;
  mask_c_uint32_t* ts = cs + NUM_WORDS(mlen);
  generate_shares((uint32_t*)ks, NUM_SHARES_KEY, k, CRYPTO_KEYBYTES);
  generate_shares((uint32_t*)npubs, NUM_SHARES_NPUB, npub, CRYPTO_NPUBBYTES);
  generate_shares((uint32_t*)ads, NUM_SHARES_AD, ad, adlen);
  generate_shares((uint32_t*)cs, NUM_SHARES_C, c, mlen);
  generate_shares((uint32_t*)ts, NUM_SHARES_C, c + mlen, CRYPTO_ABYTES);
}

void combine_shares_encrypt(const mask_c_uint32_t* cs, unsigned char* c,
                            unsigned long long clen) {
  unsigned long long mlen = clen - CRYPTO_ABYTES;
  const mask_c_uint32_t* ts = cs + NUM_WORDS(mlen);
  combine_shares(c, mlen, (uint32_t*)cs, NUM_SHARES_C);
  combine_shares(c + mlen, CRYPTO_ABYTES, (uint32_t*)ts, NUM_SHARES_C);
}

void combine_shares_decrypt(const mask_m_uint32_t* ms, unsigned char* m,
                            unsigned long long mlen) {
  combine_shares(m, mlen, (uint32_t*)ms, NUM_SHARES_M);
}

void generate_shares(uint32_t* s, int num_shares, const uint8_t* data,
                     uint64_t len) {
  uint32_t rnd0, rnd1;
  uint64_t rnd, i;
  /* generate random shares */
  for (i = 0; i < NUM_WORDS(len); i += 2) {
    s[(i + 0) * num_shares + 0] = 0;
    s[(i + 1) * num_shares + 0] = 0;
    for (int d = 1; d < num_shares; ++d) {
      RND(rnd0);
      RND(rnd1);
      s[(i + 0) * num_shares + d] = rnd0;
      s[(i + 1) * num_shares + d] = rnd1;
#if ASCON_EXTERN_BI
      s[(i + 0) * num_shares + 0] ^= ROR32(rnd0, ROT(d));
      s[(i + 1) * num_shares + 0] ^= ROR32(rnd1, ROT(d));
#else
      rnd = ROR64((uint64_t)rnd1 << 32 | rnd0, ROT(2 * d));
      s[(i + 0) * num_shares + 0] ^= (uint32_t)rnd;
      s[(i + 1) * num_shares + 0] ^= (uint32_t)(rnd >> 32);
#endif
    }
  }
  /* mask complete words */
  for (i = 0; i < len / 8; ++i) {
    uint64_t x;
    memcpy(&x, data + i * 8, 8); 
    x = U64BIG(x);
#if ASCON_EXTERN_BI
    x = TOBI(x);
#endif
    s[(2 * i + 0) * num_shares + 0] ^= (uint32_t)x;
    s[(2 * i + 1) * num_shares + 0] ^= (uint32_t)(x >> 32);
  }
  /* mask remaining bytes */
  if ((len / 8 * 8) != len) {
    uint64_t x = 0;
    for (i = (len / 8) * 8; i < len; ++i) {
      x ^= (uint64_t)data[i] << ((i % 8) * 8);
    }
    x = U64BIG(x);
#if ASCON_EXTERN_BI
    x = TOBI(x);
#endif
    s[(2 * (len / 8) + 0) * num_shares + 0] ^= (uint32_t)x;
    s[(2 * (len / 8) + 1) * num_shares + 0] ^= (uint32_t)(x >> 32);
  }
}

void combine_shares(uint8_t* data, uint64_t len, const uint32_t* s,
                    int num_shares) {
  uint32_t rnd0, rnd1;
  uint64_t i;
  /* unmask complete words */
  for (i = 0; i < len / 8; ++i) {
    uint64_t x = 0;
    for (int d = 0; d < num_shares; ++d) {
      rnd0 = s[(2 * i + 0) * num_shares + d];
      rnd1 = s[(2 * i + 1) * num_shares + d];
#if ASCON_EXTERN_BI
      x ^= (uint64_t)ROR32(rnd0, ROT(d));
      x ^= (uint64_t)ROR32(rnd1, ROT(d)) << 32;
#else
      x ^= ROR64((uint64_t)rnd1 << 32 | rnd0, ROT(2 * d));
#endif
    }
#if ASCON_EXTERN_BI
    x = FROMBI(x);
#endif
    x = U64BIG(x);
    memcpy(data + i * 8, &x, 8);
  }
  /* unmask remaining bytes */
  if ((len / 8 * 8) != len) {
    uint64_t x = 0;
    for (int d = 0; d < num_shares; ++d) {
      rnd0 = s[(2 * (len / 8) + 0) * num_shares + d];
      rnd1 = s[(2 * (len / 8) + 1) * num_shares + d];
#if ASCON_EXTERN_BI
      x ^= (uint64_t)ROR32(rnd0, ROT(d));
      x ^= (uint64_t)ROR32(rnd1, ROT(d)) << 32;
#else
      x ^= ROR64((uint64_t)rnd1 << 32 | rnd0, ROT(2 * d));
#endif
    }
#if ASCON_EXTERN_BI
    x = FROMBI(x);
#endif
    x = U64BIG(x);
    for (i = (len / 8) * 8; i < len; ++i) {
      data[i] = x >> ((i % 8) * 8);
    }
  }
}
