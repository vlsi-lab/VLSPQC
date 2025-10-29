#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* ---------------- Types (2 shares, 2 words per share) ---------------- */

typedef struct { uint32_t w[2]; } word_share_t;
typedef struct { word_share_t s[2]; } x_share_t;   // ns = 2
typedef struct { x_share_t   x[6]; } state_t;      // 6 lanes x[0..5]

/* ---------------- Function under test ---------------- */

static inline state_t AFFINE2(state_t s, int i, int d) {
  s.x[2].s[d].w[i] ^= s.x[5].s[d].w[i];
  s.x[1].s[d].w[i] ^= s.x[0].s[d].w[i];
  s.x[0].s[d].w[i] ^= s.x[4].s[d].w[i];
  s.x[3].s[d].w[i] ^= s.x[2].s[d].w[i];
  return s;
}

/* ---------------- Utilities ---------------- */

static void print_state(const char* tag, const state_t* s) {
  printf("=== %s ===\n", tag);
  for (int xi = 0; xi < 6; ++xi) {
    for (int si = 0; si < 2; ++si) {
      printf("x[%d].s[%d].w[0]=0x%08x  x[%d].s[%d].w[1]=0x%08x\n",
             xi, si, s->x[xi].s[si].w[0],
             xi, si, s->x[xi].s[si].w[1]);
    }
  }
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

/* ---------------- Main test ---------------- */

int main(void) {
  state_t s = {0}, golden = {0};

  /* Input (“AFFINE2 share=0 (before)”) */
  uint32_t init_vals[6][2][2] = {
    {{0x41255755, 0xbbe5c273}, {0xe71abffe, 0xd73fb82f}},
    {{0xe0970a16, 0xe620b2ec}, {0x5385db92, 0xe2abd2a6}},
    {{0xbe64296f, 0x9d5c622d}, {0x4648ba57, 0x04b41430}},
    {{0x42491bb2, 0xf0c89485}, {0xe1dda674, 0xd3ee7e58}},
    {{0xe616ab53, 0xe3709536}, {0x2f43cbc7, 0x1f192722}},
    {{0x0b3643f1, 0x7bb12867}, {0x46da7eb9, 0x43260cf7}}
  };
  memcpy(&s.x, init_vals, sizeof(init_vals));

  /* Golden (“AFFINE2 share=0 (after)”) */
  uint32_t gold_vals[6][2][2] = {
    {{0x41255755, 0x58955745}, {0xe71abffe, 0xd73fb82f}},
    {{0xe0970a16, 0x5dc5709f}, {0x5385db92, 0xe2abd2a6}},
    {{0xbe64296f, 0xe6ed4a4a}, {0x4648ba57, 0x04b41430}},
    {{0x42491bb2, 0x1625decf}, {0xe1dda674, 0xd3ee7e58}},
    {{0xe616ab53, 0xe3709536}, {0x2f43cbc7, 0x1f192722}},
    {{0x0b3643f1, 0x7bb12867}, {0x46da7eb9, 0x43260cf7}}
  };
  memcpy(&golden.x, gold_vals, sizeof(gold_vals));

  /* Print, run, compare */
  //print_state("AFFINE2 input", &s);

  int i = 1, d = 0;  // “AFFINE 2 called for share 0 wit i=1”
  state_t out = AFFINE2(s, i, d);

  //print_state("AFFINE2 output", &out);

  if (compare_states(&out, &golden)) {
    printf("\n✅ AFFINE2 test PASSED\n");
    return 0;
  } else {
    printf("\n❌ AFFINE2 test FAILED\n");
    return 1;
  }
}
