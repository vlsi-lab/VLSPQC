#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* ---------------- Types (ns = 2 shares, 2 words each) ---------------- */

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


/* ---------------- Function under test -------------------------------- */
static inline word_t MXORBIC(word_t c, word_t a, word_t b, int i, int ns) {
  uint32_t tmp;
  if (ns == 1) {
    EOR_BIC_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[0].w[i], 0, &tmp);
  }
  if (ns == 2) {
    EOR_BIC_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[0].w[i], 0,      &tmp);
    EOR_BIC_ROR(&c.s[1].w[i], a.s[1].w[i], b.s[0].w[i], 0 - 1,  &tmp);
    CLEAR();
    EOR_AND_ROR(&c.s[1].w[i], a.s[1].w[i], b.s[1].w[i], 0,      &tmp);
    EOR_AND_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[1].w[i], 1 - 0,  &tmp);
    CLEAR();
  }
  if (ns == 3) {
    EOR_AND_ROR(&c.s[0].w[i], b.s[0].w[i], a.s[1].w[i], 1 - 0, &tmp);
    EOR_BIC_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[0].w[i], 0,     &tmp);
    EOR_AND_ROR(&c.s[0].w[i], b.s[0].w[i], a.s[2].w[i], 2 - 0, &tmp);
    EOR_AND_ROR(&c.s[1].w[i], b.s[1].w[i], a.s[2].w[i], 2 - 1, &tmp);
    EOR_BIC_ROR(&c.s[1].w[i], a.s[1].w[i], b.s[1].w[i], 0,     &tmp);
    EOR_AND_ROR(&c.s[1].w[i], b.s[1].w[i], a.s[0].w[i], 0 - 1, &tmp);
    EOR_BIC_ROR(&c.s[2].w[i], b.s[2].w[i], a.s[0].w[i], 0 - 2, &tmp);
    EOR_ORR_ROR(&c.s[2].w[i], a.s[2].w[i], b.s[2].w[i], 0,     &tmp);
    EOR_AND_ROR(&c.s[2].w[i], b.s[2].w[i], a.s[1].w[i], 1 - 2, &tmp);
  }
  if (ns == 4) {
    EOR_BIC_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[0].w[i], 0,     &tmp);
    EOR_BIC_ROR(&c.s[1].w[i], a.s[1].w[i], b.s[0].w[i], 0 - 1, &tmp);
    EOR_BIC_ROR(&c.s[2].w[i], a.s[2].w[i], b.s[0].w[i], 0 - 2, &tmp);
    EOR_BIC_ROR(&c.s[3].w[i], a.s[3].w[i], b.s[0].w[i], 0 - 3, &tmp);
    EOR_AND_ROR(&c.s[1].w[i], a.s[1].w[i], b.s[1].w[i], 0,     &tmp);
    EOR_AND_ROR(&c.s[2].w[i], a.s[2].w[i], b.s[1].w[i], 1 - 2, &tmp);
    EOR_AND_ROR(&c.s[3].w[i], a.s[3].w[i], b.s[1].w[i], 1 - 3, &tmp);
    EOR_AND_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[1].w[i], 1 - 0, &tmp);
    EOR_AND_ROR(&c.s[2].w[i], a.s[2].w[i], b.s[2].w[i], 0,     &tmp);
    EOR_AND_ROR(&c.s[3].w[i], a.s[3].w[i], b.s[2].w[i], 2 - 3, &tmp);
    EOR_AND_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[2].w[i], 2 - 0, &tmp);
    EOR_AND_ROR(&c.s[1].w[i], a.s[1].w[i], b.s[2].w[i], 2 - 1, &tmp);
    EOR_AND_ROR(&c.s[3].w[i], a.s[3].w[i], b.s[3].w[i], 0,     &tmp);
    EOR_AND_ROR(&c.s[0].w[i], a.s[0].w[i], b.s[3].w[i], 3 - 0, &tmp);
    EOR_AND_ROR(&c.s[1].w[i], a.s[1].w[i], b.s[3].w[i], 3 - 1, &tmp);
    EOR_AND_ROR(&c.s[2].w[i], a.s[2].w[i], b.s[3].w[i], 3 - 2, &tmp);
  }
  return c;
}

/* ---------------- Utilities ------------------------------------------ */

static void print_state(const char* tag, const state_t* s) {
  printf("=== %s ===\n", tag);
  for (int xi = 0; xi < 6; ++xi)
    for (int si = 0; si < 2; ++si)
      printf("x[%d].s[%d].w[0]=0x%08x  x[%d].s[%d].w[1]=0x%08x\n",
             xi, si, s->x[xi].s[si].w[0],
             xi, si, s->x[xi].s[si].w[1]);
}

static int compare_states(const state_t* a, const state_t* b) {
  for (int xi = 0; xi < 6; ++xi)
    for (int si = 0; si < 2; ++si)
      for (int wi = 0; wi < 2; ++wi)
        if (a->x[xi].s[si].w[wi] != b->x[xi].s[si].w[wi]) {
          printf("Mismatch at x[%d].s[%d].w[%d]: got 0x%08x, expected 0x%08x\n",
                 xi, si, wi, a->x[xi].s[si].w[wi], b->x[xi].s[si].w[wi]);
          return 0;
        }
  return 1;
}

/* ---------------- Main test ------------------------------------------ */

int main(void) {
  state_t s = {0}, golden = {0};

  /* Input: “MXORBIC x5 = f(x5,x4,x3) (before)” */
  uint32_t init_vals[6][2][2] = {
    {{0x41255755, 0xb7adc077}, {0xe71abffe, 0xd73fa82f}},
    {{0xe0970a16, 0x06a4a69d}, {0x5385db92, 0xb269d4fe}},
    {{0xbe64296f, 0x9d5c622d}, {0x4648ba57, 0x04b41430}},
    {{0x42491bb2, 0xf4ed14f5}, {0xe1dda674, 0x55da5659}},
    {{0xe616ab53, 0xe5d017bf}, {0x2f43cbc7, 0x8f717704}},
    {{0x0b3643f1, 0xba2139df}, {0x46da7eb9, 0x44273bf7}}
  };
  memcpy(&s.x, init_vals, sizeof(init_vals));

  /* Golden: “MXORBIC x5 = f(x5,x4,x3) (after)” */
  uint32_t gold_vals[6][2][2] = {
    {{0x41255755, 0xb7adc077}, {0xe71abffe, 0xd73fa82f}},
    {{0xe0970a16, 0x06a4a69d}, {0x5385db92, 0xb269d4fe}},
    {{0xbe64296f, 0x9d5c622d}, {0x4648ba57, 0x04b41430}},
    {{0x42491bb2, 0xf4ed14f5}, {0xe1dda674, 0x55da5659}},
    {{0xe616ab53, 0xe5d017bf}, {0x2f43cbc7, 0x8f717704}},
    {{0x0b3643f1, 0x7bb12867}, {0x46da7eb9, 0x43260cf7}}
  };
  memcpy(&golden.x, gold_vals, sizeof(gold_vals));

  //print_state("MXORBIC input", &s);

  /* Under test: x5 := MXORBIC(x5, x4, x3) with i=1, ns=2 */
  const int i = 1;
  const int ns = 2;
  s.x[5] = MXORBIC(s.x[5], s.x[4], s.x[3], i, ns);

  //print_state("MXORBIC output", &s);

  if (compare_states(&s, &golden)) {
    printf("\n✅ MXORBIC test PASSED\n");
    return 0;
  } else {
    printf("\n❌ MXORBIC test FAILED\n");
    return 1;
  }
}
