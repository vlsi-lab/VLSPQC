#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* ---------------- Types (ns = 2 shares, 2 words per share) ---------------- */
typedef struct { uint32_t w[2]; } word_share_t;
typedef struct { word_share_t s[2]; } word_t;     // 2 shares
typedef struct { word_t x[6]; } state_t;          // 6 lanes x[0..5]

#define ASCON_ROR_SHARES 5


static inline uint32_t ROR32(uint32_t x, int n) {
  return x >> n | x << (-n & 31);
}

static inline uint32_t ROT(uint32_t i){
	return (((i) * (ASCON_ROR_SHARES)) & 31);
}

/* ---------------- External primitives (provided in your project) ----- */
static inline void EOR_AND_ROR(uint32_t *ce, uint32_t ae, uint32_t be, uint32_t imm, uint32_t *tmp) {
	uint32_t rot_imm;
	rot_imm = ROT(imm);
    //*tmp = ae & ((be >> rot_imm) | (be << (-rot_imm & 31)));
	*tmp = ae & ROR32(be, rot_imm);
    *ce ^= *tmp;
}

// Funzione per eseguire un BIC seguito da un XOR con rotazione a destra
static inline void EOR_BIC_ROR(uint32_t *ce, uint32_t ae, uint32_t be, uint32_t imm, uint32_t *tmp) {
	uint32_t rot_imm;
	rot_imm = ROT(imm);
    //*tmp = ae & ~((be >> rot_imm) | (be << (-rot_imm & 31)));
	*tmp = ae & ~(ROR32(be,rot_imm));
    *ce ^= *tmp;
}

static inline void EOR_ORR_ROR(uint32_t *ce, uint32_t ae, uint32_t be, uint32_t imm, uint32_t *tmp) {
	uint32_t rot_imm;
	rot_imm = ROT(imm);
    //*tmp = ae | ((be >> rot_imm) | (be << (-rot_imm & 31)));
	*tmp = ae | ROR32(be,rot_imm);
    *ce ^= *tmp;
	}

static inline void CLEAR() {
    uint32_t r, i = 0;
	r=i;
}


static inline  word_t MXORBIC(word_t c, word_t a, word_t b, int i, int ns) {
  uint32_t tmp;
  if (ns == 1) {
    EOR_BIC_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[0].w[i], 0, &tmp);
  }
  if (ns == 2) {
    EOR_BIC_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[0].w[i], 0, &tmp);
    EOR_BIC_ROR(&c.s[1].w[i], a.s[1].w[i], b.s[0].w[i], 0 - 1, &tmp);
    CLEAR();
    EOR_AND_ROR(&c.s[1].w[i], a.s[1].w[i], b.s[1].w[i], 0, &tmp);
    EOR_AND_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[1].w[i], 1 - 0, &tmp);
    CLEAR();
  }
  if (ns == 3) {
    EOR_AND_ROR(&c.s[0].w[i], b.s[0].w[i], a.s[1].w[i], 1 - 0, &tmp);
    EOR_BIC_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[0].w[i], 0, &tmp);
    EOR_AND_ROR(&c.s[0].w[i], b.s[0].w[i], a.s[2].w[i], 2 - 0, &tmp);
    EOR_AND_ROR(&c.s[1].w[i], b.s[1].w[i], a.s[2].w[i], 2 - 1, &tmp);
    EOR_BIC_ROR(&c.s[1].w[i], a.s[1].w[i], b.s[1].w[i], 0, &tmp);
    EOR_AND_ROR(&c.s[1].w[i], b.s[1].w[i], a.s[0].w[i], 0 - 1, &tmp);
    EOR_BIC_ROR(&c.s[2].w[i], b.s[2].w[i], a.s[0].w[i], 0 - 2, &tmp);
    EOR_ORR_ROR(&c.s[2].w[i], a.s[2].w[i], b.s[2].w[i], 0, &tmp);
    EOR_AND_ROR(&c.s[2].w[i], b.s[2].w[i], a.s[1].w[i], 1 - 2, &tmp);
  }
  if (ns == 4) {
    EOR_BIC_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[0].w[i], 0, &tmp);
    EOR_BIC_ROR(&c.s[1].w[i], a.s[1].w[i], b.s[0].w[i], 0 - 1, &tmp);
    EOR_BIC_ROR(&c.s[2].w[i], a.s[2].w[i], b.s[0].w[i], 0 - 2, &tmp);
    EOR_BIC_ROR(&c.s[3].w[i], a.s[3].w[i], b.s[0].w[i], 0 - 3, &tmp);
    EOR_AND_ROR(&c.s[1].w[i], a.s[1].w[i], b.s[1].w[i], 0, &tmp);
    EOR_AND_ROR(&c.s[2].w[i], a.s[2].w[i], b.s[1].w[i], 1 - 2, &tmp);
    EOR_AND_ROR(&c.s[3].w[i], a.s[3].w[i], b.s[1].w[i], 1 - 3, &tmp);
    EOR_AND_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[1].w[i], 1 - 0, &tmp);
    EOR_AND_ROR(&c.s[2].w[i], a.s[2].w[i], b.s[2].w[i], 0, &tmp);
    EOR_AND_ROR(&c.s[3].w[i], a.s[3].w[i], b.s[2].w[i], 2 - 3, &tmp);
    EOR_AND_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[2].w[i], 2 - 0, &tmp);
    EOR_AND_ROR(&c.s[1].w[i], a.s[1].w[i], b.s[2].w[i], 2 - 1, &tmp);
    EOR_AND_ROR(&c.s[3].w[i], a.s[3].w[i], b.s[3].w[i], 0, &tmp);
    EOR_AND_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[3].w[i], 3 - 0, &tmp);
    EOR_AND_ROR(&c.s[1].w[i], a.s[1].w[i], b.s[3].w[i], 3 - 1, &tmp);
    EOR_AND_ROR(&c.s[2].w[i], a.s[2].w[i], b.s[3].w[i], 3 - 2, &tmp);
  }
  return c;
}


state_t AFFINE1(state_t s, int i, int d) {

  s.x[2].s[d].w[i] ^= s.x[1].s[d].w[i];
  s.x[0].s[d].w[i] ^= s.x[4].s[d].w[i];
  s.x[4].s[d].w[i] ^= s.x[3].s[d].w[i];

    return s;
}

state_t AFFINE2(state_t s, int i, int d) {
  s.x[2].s[d].w[i] ^= s.x[5].s[d].w[i];
  s.x[1].s[d].w[i] ^= s.x[0].s[d].w[i];
  s.x[0].s[d].w[i] ^= s.x[4].s[d].w[i];
  s.x[3].s[d].w[i] ^= s.x[2].s[d].w[i];
  return s;
}


static inline state_t SBOX(state_t s, int i, int ns) {
  /* affine layer 1 */

  if (ns >= 1) s = AFFINE1(s, i, 0);
  if (ns >= 2) s = AFFINE1(s, i, 1);
  if (ns >= 3) s = AFFINE1(s, i, 2);
  if (ns >= 4) s = AFFINE1(s, i, 3);
  
  /* Toffoli gates */
  s.x[5] = MXORBIC(s.x[5], s.x[4], s.x[3], i, ns);
  s.x[4] = MXORBIC(s.x[4], s.x[1], s.x[0], i, ns);
  s.x[1] = MXORBIC(s.x[1], s.x[3], s.x[2], i, ns);
  s.x[3] = MXORBIC(s.x[3], s.x[0], s.x[4], i, ns);
  s.x[0] = MXORBIC(s.x[0], s.x[2], s.x[1], i, ns);
  /* affine layer 2 */
  if (ns >= 1) s = AFFINE2(s, i, 0);
  s.x[2].s[0].w[i] = ~s.x[2].s[0].w[i];
  if (ns >= 2) s = AFFINE2(s, i, 1);
  if (ns >= 3) s = AFFINE2(s, i, 2);
  if (ns >= 4) s = AFFINE2(s, i, 3);
  return s;
}

/* ---------------- Utilities ------------------------------------------------ */
static void print_state(const char* tag, const state_t* s, int ns) {
  printf("=== %s ===\n", tag);
  for (int xi = 0; xi < 6; ++xi) {
    for (int d = 0; d < ns; ++d) {
      printf("x[%d].s[%d].w[0]=0x%08x  x[%d].s[%d].w[1]=0x%08x\n",
             xi, d, s->x[xi].s[d].w[0], xi, d, s->x[xi].s[d].w[1]);
    }
  }
}

static int compare_states(const state_t* a, const state_t* b, int ns) {
  for (int xi = 0; xi < 6; ++xi)
    for (int d = 0; d < ns; ++d)
      for (int wi = 0; wi < 2; ++wi)
        if (a->x[xi].s[d].w[wi] != b->x[xi].s[d].w[wi]) {
          printf("Mismatch at x[%d].s[%d].w[%d]: got 0x%08x, expected 0x%08x\n",
                 xi, d, wi, a->x[xi].s[d].w[wi], b->x[xi].s[d].w[wi]);
          return 0;
        }
  return 1;
}

/* ---------------- Main test ------------------------------------------------ */
int main(void) {
  const int ns = 2;
  const int i  = 0;

  state_t s = {0}, golden = {0};

  /* Input: “ROUND_: SBOX i=0 (before)” */
  uint32_t init_vals[6][2][2] = {
    {{0xa6f00f89, 0x19fcd760}, {0x97c03767, 0x1a66bb7f}},
    {{0x1a637432, 0xed1e0e36}, {0x707ceda2, 0xab571ce9}},
    {{0xbaf9b2e7, 0x332bfb2e}, {0xa0c9a218, 0x9a809b49}},
    {{0xcf1d3526, 0xaf2bfdbc}, {0x7fc48555, 0xa5dc9609}},
    {{0x7ddf2201, 0xf0a3dfea}, {0x772c8ce1, 0x501fb91a}},
    {{0xe8dd6ce7, 0x413680b0}, {0x1bad9cfd, 0x26d01608}}
  };
  memcpy(&s.x, init_vals, sizeof(init_vals));

  /* Golden: “ROUND_: SBOX i=0 (after)” */
  uint32_t gold_vals[6][2][2] = {
    {{0xdb344a09, 0x19fcd760}, {0x48d5538a, 0x1a66bb7f}},
    {{0xe8d60c38, 0xed1e0e36}, {0x7c3552ef, 0xab571ce9}},
    {{0x25b853ee, 0x332bfb2e}, {0xcb90db77, 0x9a809b49}},
    {{0xdd70b0bf, 0xaf2bfdbc}, {0x34106da6, 0xa5dc9609}},
    {{0xa0812305, 0xf0a3dfea}, {0x789cec96, 0x501fb91a}},
    {{0x7add6ac4, 0x413680b0}, {0x1b2594cd, 0x26d01608}}
  };
  memcpy(&golden.x, gold_vals, sizeof(gold_vals));

  //print_state("ROUND_: SBOX i=0 (before)", &s, ns);

  /* Function under test */
  state_t out = SBOX(s, i, ns);

  //print_state("ROUND_: SBOX i=0 (after, got)", &out, ns);
  //print_state("ROUND_: SBOX i=0 (after, golden)", &golden, ns);

  if (compare_states(&out, &golden, ns)) {
    printf("\n✅ SBOX(i=0, ns=2) test PASSED\n");
    return 0;
  } else {
    printf("\n❌ SBOX(i=0, ns=2) test FAILED\n");
    return 1;
  }
}
