#include <stdio.h>
#include "api.h"
#include "crypto_aead_shared.h"
#include "word.h"


static inline int crypto_aead_encrypt(unsigned char* c, unsigned long long* clen,
                        const unsigned char* m, unsigned long long mlen,
                        const unsigned char* a, unsigned long long alen,
                        const unsigned char* nsec, const unsigned char* npub,
                        const unsigned char* k) {
  (void)nsec;


  #if DEBUG_CODE==1
    printf("\n🚀 Starting crypto_aead_encrypt()...\n\n");

    /* --------------------- Inputs --------------------- */
    printf("Plaintext (m, %llu bytes): ", mlen);
    for (unsigned long long i = 0; i < mlen; i++) printf("%02X", m[i]);
    printf("\n");
    printf("Associated Data (a, %llu bytes): ", alen);
    for (unsigned long long i = 0; i < alen; i++) printf("%02X", a[i]);
    printf("\n");
    printf("Nonce (npub): ");
    for (unsigned long long i = 0; i < CRYPTO_NPUBBYTES; i++) printf("%02X", npub[i]);
    printf("\n");
    printf("Key (k): ");
    for (unsigned long long i = 0; i < CRYPTO_KEYBYTES; i++) printf("%02X", k[i]);
    printf("\n\n");
  #endif

  
  /* dynamic allocation of input/output shares */
  mask_key_uint32_t* ks = (mask_key_uint32_t*) malloc(sizeof(*ks) * NUM_WORDS(CRYPTO_KEYBYTES));
  mask_npub_uint32_t* ns = (mask_npub_uint32_t*)malloc(sizeof(*ns) * NUM_WORDS(CRYPTO_NPUBBYTES));
  mask_ad_uint32_t* as = (mask_ad_uint32_t*)malloc(sizeof(*as) * NUM_WORDS(alen));
  mask_m_uint32_t* ms = (mask_m_uint32_t*)malloc(sizeof(*ms) * NUM_WORDS(mlen));
  mask_c_uint32_t* cs = (mask_c_uint32_t*)malloc(sizeof(*cs) * NUM_WORDS(mlen + CRYPTO_ABYTES));
  /* mask plain input data */


  #if DEBUG_CODE==1
    /* --------------------- Inputs --------------------- */
    printf("Plaintext (m, %llu bytes):\n", mlen);
    for (unsigned long long i = 0; i < mlen; i++) printf("%02X", m[i]);
    printf("\n");
    printf("Associated Data (a, %llu bytes):\n", alen);
    for (unsigned long long i = 0; i < alen; i++) printf("%02X", a[i]);
    printf("\n");
    printf("Nonce (npub):\n");
    for (unsigned long long i = 0; i < CRYPTO_NPUBBYTES; i++) printf("%02X", npub[i]);
    printf("\n");
    printf("Key (k):\n");
    for (unsigned long long i = 0; i < CRYPTO_KEYBYTES; i++) printf("%02X", k[i]);
    printf("\n\n");
  #endif
  
  generate_shares_encrypt(m, ms, mlen, a, as, alen, npub, ns, k, ks);

  /* call shared interface of ascon encrypt */
  //trigger_high();
  crypto_aead_encrypt_shared(cs, clen, ms, mlen, as, alen, ns, ks);
  //trigger_low();

  #if DEBUG_CODE==1
    printf("Ciphertext masked (total %llu bytes):\n", *clen);
    for (unsigned long long i = 0; i < *clen; i++) printf("%02X", cs[i]);
    printf("\n");
  #endif

  /* unmask shared output data */
  combine_shares_encrypt(cs, c, *clen);

  #if DEBUG_CODE==1
    printf("Ciphertext + Tag (total %llu bytes):\n", *clen);
    for (unsigned long long i = 0; i < *clen; i++) printf("%02X", c[i]);
    printf("\n\n");
  #endif
  /* free shares */
  free(ks);
  free(ns);
  free(as);
  free(ms);
  free(cs);
  return 0;
}

static inline int crypto_aead_decrypt(unsigned char* m, unsigned long long* mlen,
                        unsigned char* nsec, const unsigned char* c,
                        unsigned long long clen, const unsigned char* a,
                        unsigned long long alen, const unsigned char* npub,
                        const unsigned char* k) {
  int result = 0;
  (void)nsec;
  if (clen < CRYPTO_ABYTES) return -1;
  /* dynamic allocation of input/output shares */
  mask_key_uint32_t* ks =(mask_key_uint32_t*) malloc(sizeof(*ks) * NUM_WORDS(CRYPTO_KEYBYTES));
  mask_npub_uint32_t* ns =(mask_npub_uint32_t*) malloc(sizeof(*ns) * NUM_WORDS(CRYPTO_NPUBBYTES));
  mask_ad_uint32_t* as =(mask_ad_uint32_t*) malloc(sizeof(*as) * NUM_WORDS(alen));
  mask_m_uint32_t* ms = (mask_m_uint32_t*) malloc(sizeof(*ms) * NUM_WORDS(clen - CRYPTO_ABYTES));
  mask_c_uint32_t* cs = (mask_c_uint32_t*) malloc(sizeof(*cs) * NUM_WORDS(clen));
  /* mask plain input data */
  generate_shares_decrypt(c, cs, clen, a, as, alen, npub, ns, k, ks);
  /* call shared interface of ascon decrypt */
  result = crypto_aead_decrypt_shared(ms, mlen, cs, clen, as, alen, ns, ks);
  /* unmask shared output data */
  combine_shares_decrypt(ms, m, *mlen);
  /* free shares */
  free(ks);
  free(ns);
  free(as);
  free(ms);
  free(cs);
  return result;
}

