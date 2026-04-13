#ifndef ASCON_H_
#define ASCON_H_

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "shares.h"
#include "word.h"
#include "permutations.h"
#include "constants.h"
//#include "round.h"





/* 
 * Print the content of one masked 64-bit word (two 32-bit halves)
 * for ns = 2 shares. It also reconstructs the unmasked value
 * using the rotation-aware rule of MZERO (share0 ^ ROR32(share1, ROT(1))).
 *
 * Assumes:
 *  - word_t has structure word_t.s[share].w[0..1]
 *  - ROT(1) macro is defined in your codebase
 *  - ROR32(x, r) rotates right by r bits
 */
static inline void dbg_print_word_ns2(const char* label, const word_t* w)
{
  printf("%s\n", label);
  printf("  share[0]: %08X %08X\n", w->s[0].w[0], w->s[0].w[1]);
  printf("  share[1]: %08X %08X\n", w->s[1].w[0], w->s[1].w[1]);
}

/*typedef struct {
  word_t x[6];
} state_t;*/

/* expected value of x3,x4 for P(0) */
#if ASCON_PB_ROUNDS == 1
static const uint32_t c[4] = {0x4b000009, 0x1c800003, 0x00000000, 0x00000000};
#elif ASCON_PB_ROUNDS == 2
static const uint32_t c[4] = {0x5d2d1034, 0x76fa81d1, 0x0cc1c9ef, 0xdb30a503};
#elif ASCON_PB_ROUNDS == 3
static const uint32_t c[4] = {0xbcaa1d46, 0xf1d0bde9, 0x32c4e651, 0x7b797cd9};
#elif ASCON_PB_ROUNDS == 4
static const uint32_t c[4] = {0xf7820616, 0xeffead2d, 0x94846901, 0xd4895cf5};
#elif ASCON_PB_ROUNDS == 5
static const uint32_t c[4] = {0x9e5ce5e3, 0xd40e9b87, 0x0bfc74af, 0xf8e408a9};
#else /* ASCON_PB_ROUNDS == 6 */
static const uint32_t c[4] = {0x11874f08, 0x7520afef, 0xa4dd41b4, 0x4bd6f9a4};
#endif

static inline void ascon_initaead(state_t* s, const mask_npub_uint32_t* n,
                    const mask_key_uint32_t* k) {
  word_t N0, N1;
  word_t K1, K2;
  /* randomize the initial state */
  s->x[5] = MZERO(NUM_SHARES_KEY);
  s->x[0] = MZERO(NUM_SHARES_KEY);
  /* set the initial value */
  s->x[0].s[0].w[0] ^= 0x08220000;
  s->x[0].s[0].w[1] ^= 0x80210000;
  /* set the nonce */
  s->x[3] = N0 = MLOAD((uint32_t*)n, NUM_SHARES_NPUB);
  s->x[4] = N1 = MLOAD((uint32_t*)(n + 2), NUM_SHARES_NPUB);
  /* first key xor */
  s->x[1] = K1 = MLOAD((uint32_t*)k, NUM_SHARES_KEY);
  s->x[2] = K2 = MLOAD((uint32_t*)(k + 2), NUM_SHARES_KEY);
  /* compute the permutation */
  P(s, ASCON_PA_ROUNDS, NUM_SHARES_KEY);
  /* second key xor */
  s->x[3] = MXOR(s->x[3], K1, NUM_SHARES_KEY);
  s->x[4] = MXOR(s->x[4], K2, NUM_SHARES_KEY);
}




static inline void ascon_adata(state_t* s, const mask_ad_uint32_t* ad, uint64_t adlen) {
  const int nr = ASCON_PB_ROUNDS;

#if DEBUG_CODE==1
  printf("\n[DEBUG] === AD ABSORPTION START (adlen=%llu) ===\n", (unsigned long long)adlen);
  dbg_print_word_ns2("x[0] (pre-AD)", &s->x[0]);
  dbg_print_word_ns2("x[4] (pre-AD)", &s->x[4]);
#endif

  if (adlen) {
    while (adlen >= ASCON_AEAD_RATE) {
      word_t as = MLOAD((uint32_t*)ad, NUM_SHARES_AD);
#if DEBUG_CODE==1
      printf("[DEBUG] Full AD block absorb\n");
      // If you want to show the raw masked words being absorbed:
      dbg_print_word_ns2("AD block (as)", &as);
#endif
      s->x[0] = MXOR(s->x[0], as, NUM_SHARES_AD);
#if DEBUG_CODE==1
      dbg_print_word_ns2("x[0] after absorb", &s->x[0]);
#endif
      P(s, nr, NUM_SHARES_AD);
#if DEBUG_CODE==1
      dbg_print_word_ns2("x[0] after P^b", &s->x[0]);
      dbg_print_word_ns2("x[1] after P^b", &s->x[1]);
      dbg_print_word_ns2("x[2] after P^b", &s->x[2]);
      dbg_print_word_ns2("x[3] after P^b", &s->x[3]);
      dbg_print_word_ns2("x[4] after P^b", &s->x[4]);
      dbg_print_word_ns2("x[5] after P^b", &s->x[5]);
#endif
      adlen -= ASCON_AEAD_RATE;
      ad += 2;
    }

    // Final/partial block padding
    s->x[0].s[0].w[1] ^= 0x80000000 >> (adlen * 4);
#if DEBUG_CODE==1
    printf("[DEBUG] After AD padding (10* nibble)\n");
    dbg_print_word_ns2("x[0] (after pad)", &s->x[0]);
#endif

    // Absorb trailing bytes if present
    if (adlen) {
      word_t as = MLOAD((uint32_t*)ad, NUM_SHARES_AD);
#if DEBUG_CODE==1
      dbg_print_word_ns2("Trailing AD (as)", &as);
#endif
      s->x[0] = MXOR(s->x[0], as, NUM_SHARES_AD);
#if DEBUG_CODE==1
      dbg_print_word_ns2("x[0] after trailing absorb", &s->x[0]);
#endif
    }

    // Final permutation after last block
    P(s, nr, NUM_SHARES_AD);
#if DEBUG_CODE==1
    printf("[DEBUG] After final P^b on AD\n");
    dbg_print_word_ns2("x[0]", &s->x[0]);
    dbg_print_word_ns2("x[1]", &s->x[1]);
    dbg_print_word_ns2("x[2]", &s->x[2]);
    dbg_print_word_ns2("x[3]", &s->x[3]);
    dbg_print_word_ns2("x[4]", &s->x[4]);
    dbg_print_word_ns2("x[5]", &s->x[5]);
#endif
  }

  // Domain separation
  s->x[4].s[0].w[0] ^= 1;
#if DEBUG_CODE==1
  printf("[DEBUG] Domain separation: x[4].s[0].w[0] ^= 1\n");
  dbg_print_word_ns2("x[4] (post-domain sep)", &s->x[4]);
  printf("[DEBUG] === AD ABSORPTION END ===\n\n");
#endif
}


static inline void ascon_encrypt(state_t* s,
                                 mask_c_uint32_t* c,
                                 const mask_m_uint32_t* m,
                                 uint64_t mlen) {
  const int nr = ASCON_PB_ROUNDS;

  #if DEBUG_CODE==1
    printf("\n[DEBUG] === ENCRYPT START (mlen=%llu) ===\n", (unsigned long long)mlen);
    dbg_print_word_ns2("x[0] (pre-enc)", &s->x[0]);
  #endif

    /* 1) Full plaintext blocks */
    while (mlen >= ASCON_AEAD_RATE) {
      /* Load one masked message block into a word_t */
      word_t msblk = MLOAD((uint32_t*)m, NUM_SHARES_M);
  #if DEBUG_CODE==1
      dbg_print_word_ns2("MLOAD msblk (plaintext block)", &msblk);
  #endif

      /* XOR-absorb into x[0] and output ciphertext = current x[0] */
      s->x[0] = MXOR(s->x[0], msblk, NUM_SHARES_M);
  #if DEBUG_CODE==1
      dbg_print_word_ns2("x[0] after absorb (pre-store)", &s->x[0]);
  #endif

      /* Store masked ciphertext shares for this block */
      MSTORE((uint32_t*)c, s->x[0], NUM_SHARES_C);
  #if DEBUG_CODE==1
      // The stored ciphertext equals x[0] at this point
      printf("[DEBUG] MSTORE ciphertext block written\n");
      dbg_print_word_ns2("ct block (= x[0])", &s->x[0]);
  #endif

      /* Permute after each full-rate block */
      P(s, nr, NUM_SHARES_M);
  #if DEBUG_CODE==1
      printf("[DEBUG] After P^b (post full block)\n");
      dbg_print_word_ns2("x[0]", &s->x[0]);
      dbg_print_word_ns2("x[1]", &s->x[1]);
      dbg_print_word_ns2("x[2]", &s->x[2]);
      dbg_print_word_ns2("x[3]", &s->x[3]);
      dbg_print_word_ns2("x[4]", &s->x[4]);
      dbg_print_word_ns2("x[5]", &s->x[5]);
  #endif

      /* Advance */
      mlen -= ASCON_AEAD_RATE;
      m    += 2;   // advance by 64 bits (2×32b words)
      c    += 2;   // same for ciphertext buffer
    }

    /* 2) Final partial block (if any) */
    /* Pad at the correct nibble position inside x[0] */
    s->x[0].s[0].w[1] ^= 0x80000000u >> (mlen * 4);
  #if DEBUG_CODE==1
    printf("[DEBUG] Applied 10* padding for final block (mlen rem=%llu nibbles)\n",
          (unsigned long long)(mlen * 2));
    dbg_print_word_ns2("x[0] after pad", &s->x[0]);
  #endif

    if (mlen) {
      /* Load trailing masked message data */
      word_t msblk = MLOAD((uint32_t*)m, NUM_SHARES_M);
  #if DEBUG_CODE==1
      dbg_print_word_ns2("MLOAD msblk (final partial)", &msblk);
  #endif

      /* Absorb and output ciphertext shares for the partial block */
      s->x[0] = MXOR(s->x[0], msblk, NUM_SHARES_M);
  #if DEBUG_CODE==1
      dbg_print_word_ns2("x[0] after final absorb (pre-store)", &s->x[0]);
  #endif

      MSTORE((uint32_t*)c, s->x[0], NUM_SHARES_C);
  #if DEBUG_CODE==1
      printf("[DEBUG] MSTORE partial ciphertext block written\n");
      dbg_print_word_ns2("ct partial block (= x[0])", &s->x[0]);
  #endif
    }

  #if DEBUG_CODE==1
    printf("[DEBUG] === ENCRYPT END ===\n\n");
  #endif
}

void ascon_decrypt(state_t* s, mask_m_uint32_t* m, const mask_c_uint32_t* c,
                   uint64_t clen) {
  const int nr = ASCON_PB_ROUNDS;
  /* full ciphertext blocks */
  while (clen >= ASCON_AEAD_RATE) {
    word_t cx = MLOAD((uint32_t*)c, NUM_SHARES_C);
    s->x[0] = MXOR(s->x[0], cx, NUM_SHARES_C);
    MSTORE((uint32_t*)m, s->x[0], NUM_SHARES_M);
    s->x[0] = cx;
    P(s, nr, NUM_SHARES_M);
    clen -= ASCON_AEAD_RATE;
    c += 2;
    m += 2;
  }
  /* final ciphertext block */
  s->x[0].s[0].w[1] ^= 0x80000000 >> (clen * 4);
  if (clen) {
    word_t cx = MLOAD((uint32_t*)c, NUM_SHARES_C);
    s->x[0] = MXOR(s->x[0], cx, NUM_SHARES_C);
    MSTORE((uint32_t*)m, s->x[0], NUM_SHARES_M);
    word_t mask = MMASK(clen, NUM_SHARES_C);
    s->x[0] = MXORAND(cx, s->x[0], mask, NUM_SHARES_C);
  }
}


static inline void ascon_final(state_t* s, const mask_key_uint32_t* k) {
  word_t K1, K2;

  /* Load masked key halves (128-bit key as two 64-bit lanes) */
  K1 = MLOAD((uint32_t*)k,       NUM_SHARES_KEY);   // low 64 bits
#if DEBUG_CODE==1
  printf("\n[DEBUG] === FINAL START: load K1 ===\n");
  dbg_print_word_ns2("K1 (key low 64)", &K1);
#endif

  K2 = MLOAD((uint32_t*)(k + 2), NUM_SHARES_KEY);   // high 64 bits
#if DEBUG_CODE==1
  printf("[DEBUG] Load K2\n");
  dbg_print_word_ns2("K2 (key high 64)", &K2);
#endif

  /* First key XOR: absorb key into x[1], x[2] */
  s->x[1] = MXOR(s->x[1], K1, NUM_SHARES_KEY);
#if DEBUG_CODE==1
  printf("[DEBUG] After x[1] ^= K1\n");
  dbg_print_word_ns2("x[1]", &s->x[1]);
#endif

  s->x[2] = MXOR(s->x[2], K2, NUM_SHARES_KEY);
#if DEBUG_CODE==1
  printf("[DEBUG] After x[2] ^= K2\n");
  dbg_print_word_ns2("x[2]", &s->x[2]);
#endif

  /* Permutation with a-rounds (same as init) */
  P(s, ASCON_PA_ROUNDS, NUM_SHARES_KEY);
#if DEBUG_CODE==1
  printf("[DEBUG] After P^a in final\n");
  dbg_print_word_ns2("x[0]", &s->x[0]);
  dbg_print_word_ns2("x[1]", &s->x[1]);
  dbg_print_word_ns2("x[2]", &s->x[2]);
  dbg_print_word_ns2("x[3]", &s->x[3]);
  dbg_print_word_ns2("x[4]", &s->x[4]);
  dbg_print_word_ns2("x[5]", &s->x[5]);
#endif

  /* Second key XOR: finalize into x[3], x[4] (tag lanes) */
  s->x[3] = MXOR(s->x[3], K1, NUM_SHARES_KEY);
#if DEBUG_CODE==1
  printf("[DEBUG] After x[3] ^= K1\n");
  dbg_print_word_ns2("x[3]", &s->x[3]);
#endif

  s->x[4] = MXOR(s->x[4], K2, NUM_SHARES_KEY);
#if DEBUG_CODE==1
  printf("[DEBUG] After x[4] ^= K2\n");
  dbg_print_word_ns2("x[4]", &s->x[4]);
  printf("[DEBUG] === FINAL END ===\n\n");
#endif
}


static inline void ascon_settag(state_t* s, mask_c_uint32_t* t) {
#if DEBUG_CODE==1
  printf("\n[DEBUG] === SETTAG START ===\n");
  dbg_print_word_ns2("tag lane x[3] (pre-store)", &s->x[3]);
  dbg_print_word_ns2("tag lane x[4] (pre-store)", &s->x[4]);
#endif

  MSTORE((uint32_t*)t,       s->x[3], NUM_SHARES_C);
  MSTORE((uint32_t*)(t + 2), s->x[4], NUM_SHARES_C);

#if DEBUG_CODE==1
  printf("[DEBUG] Tag stored at output buffer (t..t+3 32-bit words)\n");
  // If you keep buffers as arrays of mask_c_uint32_t (AoS), you can also print them:
  // print_mask_c(t, 2 /* two 64-bit words = 4x32-bit */);
  printf("[DEBUG] === SETTAG END ===\n\n");
#endif
}

static inline int ascon_iszero(state_t* s) {
  #if NUM_SHARES_KEY >= 2
    s->x[3].s[0].w[0] ^= ROR32(s->x[3].s[1].w[0], ROT(1));
    s->x[3].s[0].w[1] ^= ROR32(s->x[3].s[1].w[1], ROT(1));
    s->x[4].s[0].w[0] ^= ROR32(s->x[4].s[1].w[0], ROT(1));
    s->x[4].s[0].w[1] ^= ROR32(s->x[4].s[1].w[1], ROT(1));
  #endif
  #if NUM_SHARES_KEY >= 3
    s->x[3].s[0].w[0] ^= ROR32(s->x[3].s[2].w[0], ROT(2));
    s->x[3].s[0].w[1] ^= ROR32(s->x[3].s[2].w[1], ROT(2));
    s->x[4].s[0].w[0] ^= ROR32(s->x[4].s[2].w[0], ROT(2));
    s->x[4].s[0].w[1] ^= ROR32(s->x[4].s[2].w[1], ROT(2));
  #endif
  #if NUM_SHARES_KEY >= 4
    s->x[3].s[0].w[0] ^= ROR32(s->x[3].s[3].w[0], ROT(3));
    s->x[3].s[0].w[1] ^= ROR32(s->x[3].s[3].w[1], ROT(3));
    s->x[4].s[0].w[0] ^= ROR32(s->x[4].s[3].w[0], ROT(3));
    s->x[4].s[0].w[1] ^= ROR32(s->x[4].s[3].w[1], ROT(3));
  #endif
    uint32_t result;
    result = s->x[3].s[0].w[0] ^ s->x[3].s[0].w[1];
    result ^= s->x[4].s[0].w[0] ^ s->x[4].s[0].w[1];
    result |= result >> 16;
    result |= result >> 8;
    return ((((int)(result & 0xff) - 1) >> 8) & 1) - 1;
  }
  


static inline void ascon_xortag(state_t* s, const mask_c_uint32_t* t) {
  /* set x0, x1, x2 to zero */
  s->x[0] = MREUSE(s->x[0], 0, NUM_SHARES_KEY);
  s->x[1] = MREUSE(s->x[1], 0, NUM_SHARES_KEY);
  s->x[2] = MREUSE(s->x[2], 0, NUM_SHARES_KEY);
  /* xor tag to x3, x4 */
  word_t t0 = MLOAD((uint32_t*)t, NUM_SHARES_C);
  s->x[3] = MXOR(s->x[3], t0, NUM_SHARES_C);
  word_t t1 = MLOAD((uint32_t*)(t + 2), NUM_SHARES_C);
  s->x[4] = MXOR(s->x[4], t1, NUM_SHARES_C);
  /* compute P(0) if tags are equal */
  P(s, ASCON_PB_ROUNDS, NUM_SHARES_KEY);
  /* xor expected result to x3, x4 */
  s->x[3].s[0].w[0] ^= c[0];
  s->x[3].s[0].w[1] ^= c[1];
  s->x[4].s[0].w[0] ^= c[2];
  s->x[4].s[0].w[1] ^= c[3];
}

static inline int ascon_verify(state_t* s, const mask_c_uint32_t* t) {
  ascon_xortag(s, t);
  return ascon_iszero(s);
}

void ascon_level_adata(state_t* s);
void ascon_level_encdec(state_t* s);
void ascon_level_final(state_t* s);

#endif /* ASCON_H_ */
