#include "shares.h"
#include "api.h"
#include "ascon.h"
#include "shares.h"
#include "word.h"

// ---- Type-specific printers for masked arrays (32-bit lane) -----------------
#if DEBUG_CODE==1
static inline void print_mask_key(const mask_key_uint32_t* a, size_t nwords) {
  printf("[KEY SHARES] words=%zu\n", nwords);
  for (int s = 0; s < NUM_SHARES_KEY; s++) {
    printf("  share[%d]: ", s);
    for (size_t w = 0; w < nwords; w++) printf("%08X ", a[w].shares[s]);
#ifdef DEBUG_RECONSTRUCT
    // Reconstruct per word for display (XOR across shares)
    printf(" | recon: ");
    for (size_t w = 0; w < nwords; w++) {
      uint32_t r = 0;
      for (int ss = 0; ss < NUM_SHARES_KEY; ss++) r ^= a[w].shares[ss];
      printf("%08X ", r);
    }
#endif
    printf("\n");
  }
}

static inline void print_mask_npub(const mask_npub_uint32_t* a, size_t nwords) {
  printf("[NPUB SHARES] words=%zu\n", nwords);
  for (int s = 0; s < NUM_SHARES_NPUB; s++) {
    printf("  share[%d]: ", s);
    for (size_t w = 0; w < nwords; w++) printf("%08X ", a[w].shares[s]);
#ifdef DEBUG_RECONSTRUCT
    printf(" | recon: ");
    for (size_t w = 0; w < nwords; w++) {
      uint32_t r = 0;
      for (int ss = 0; ss < NUM_SHARES_NPUB; ss++) r ^= a[w].shares[ss];
      printf("%08X ", r);
    }
#endif
    printf("\n");
  }
}

static inline void print_mask_ad(const mask_ad_uint32_t* a, size_t nwords) {
  printf("[AD SHARES] words=%zu\n", nwords);
  for (int s = 0; s < NUM_SHARES_AD; s++) {
    printf("  share[%d]: ", s);
    for (size_t w = 0; w < nwords; w++) printf("%08X ", a[w].shares[s]);
#ifdef DEBUG_RECONSTRUCT
    printf(" | recon: ");
    for (size_t w = 0; w < nwords; w++) {
      uint32_t r = 0;
      for (int ss = 0; ss < NUM_SHARES_AD; ss++) r ^= a[w].shares[ss];
      printf("%08X ", r);
    }
#endif
    printf("\n");
  }
}

static inline void print_mask_m(const mask_m_uint32_t* a, size_t nwords) {
  printf("[MESSAGE SHARES] words=%zu\n", nwords);
  for (int s = 0; s < NUM_SHARES_M; s++) {
    printf("  share[%d]: ", s);
    for (size_t w = 0; w < nwords; w++) printf("%08X ", a[w].shares[s]);
#ifdef DEBUG_RECONSTRUCT
    printf(" | recon: ");
    for (size_t w = 0; w < nwords; w++) {
      uint32_t r = 0;
      for (int ss = 0; ss < NUM_SHARES_M; ss++) r ^= a[w].shares[ss];
      printf("%08X ", r);
    }
#endif
    printf("\n");
  }
}

static inline void print_mask_c(const mask_c_uint32_t* a, size_t nwords) {
  printf("[CIPHERTEXT/OUT SHARES] words=%zu\n", nwords);
  for (int s = 0; s < NUM_SHARES_C; s++) {
    printf("  share[%d]: ", s);
    for (size_t w = 0; w < nwords; w++) printf("%08X ", a[w].shares[s]);
#ifdef DEBUG_RECONSTRUCT
    printf(" | recon: ");
    for (size_t w = 0; w < nwords; w++) {
      uint32_t r = 0;
      for (int ss = 0; ss < NUM_SHARES_C; ss++) r ^= a[w].shares[ss];
      printf("%08X ", r);
    }
#endif
    printf("\n");
  }
}
#endif // DEBUG_CODE==1

// ---- Your function with debug prints ----------------------------------------
static inline int crypto_aead_encrypt_shared(mask_c_uint32_t* cs, unsigned long long* clen,
                               const mask_m_uint32_t* ms,
                               unsigned long long mlen,
                               const mask_ad_uint32_t* ads,
                               unsigned long long adlen,
                               const mask_npub_uint32_t* npubs,
                               const mask_key_uint32_t* ks) {
  state_t s;
  *clen = mlen + CRYPTO_ABYTES;

  #if DEBUG_CODE==1
    printf("\n=== AEAD ENCRYPT (masked) ===\n");
    printf("mlen=%llu, adlen=%llu, CRYPTO_ABYTES=%d\n",
          (unsigned long long)mlen, (unsigned long long)adlen, (int)CRYPTO_ABYTES);
    // Show inputs (masked)
    print_mask_key(ks, NUM_WORDS(CRYPTO_KEYBYTES));
    print_mask_npub(npubs, NUM_WORDS(CRYPTO_NPUBBYTES));
    if (adlen)  print_mask_ad(ads, NUM_WORDS(adlen));
    if (mlen)   print_mask_m(ms,  NUM_WORDS(mlen));
  #endif

  // 1) Init with key+nonce
  ascon_initaead(&s, npubs, ks);
  #if DEBUG_CODE==1
    printf("[STEP] after ascon_initaead(): state initialized with key+nonce\n");
  #endif

  // 2) Absorb associated data
  ascon_adata(&s, ads, adlen);
  #if DEBUG_CODE==1
    printf("[STEP] after ascon_adata(): AD absorbed (adlen=%llu)\n", (unsigned long long)adlen);
  #endif

  // 3) Encrypt message → ciphertext shares in cs[0 .. NUM_WORDS(mlen)-1]
  ascon_encrypt(&s, cs, ms, mlen);
  #if DEBUG_CODE==1
    printf("[STEP] after ascon_encrypt(): ciphertext produced\n");
    if (mlen) print_mask_c(cs, NUM_WORDS(mlen));
  #endif

  // 4) Finalize (keyed)
  ascon_final(&s, ks);
  #if DEBUG_CODE==1
    printf("[STEP] after ascon_final(): finalized with key\n");
  #endif

  // 5) Write authentication tag after ciphertext
  ascon_settag(&s, cs + NUM_WORDS(mlen));
  #if DEBUG_CODE==1
    printf("[STEP] after ascon_settag(): tag written\n");
    // Print tag shares; tag length is CRYPTO_ABYTES
    print_mask_c(cs + NUM_WORDS(mlen), NUM_WORDS(CRYPTO_ABYTES));
    printf("=== END AEAD ENCRYPT ===\n\n");
  #endif

  return 0;
}

static inline int crypto_aead_decrypt_shared(mask_m_uint32_t* ms, unsigned long long* mlen,
                               const mask_c_uint32_t* cs,
                               unsigned long long clen,
                               const mask_ad_uint32_t* ads,
                               unsigned long long adlen,
                               const mask_npub_uint32_t* npubs,
                               const mask_key_uint32_t* ks) {
  state_t s;
  *mlen = clen - CRYPTO_ABYTES;
  ascon_initaead(&s, npubs, ks);
  ascon_adata(&s, ads, adlen);
  ascon_decrypt(&s, ms, cs, *mlen);
  ascon_final(&s, ks);
  return ascon_verify(&s, cs + NUM_WORDS(*mlen));
}
