#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* -------------------------------------------------------------------------- */
/*  Definition of your data structures                                        */
/* -------------------------------------------------------------------------- */

typedef struct {
    uint32_t w[2];
} word_share_t;

typedef struct {
    word_share_t s[2];  // ns = 2 shares
} x_share_t;

typedef struct {
    x_share_t x[6];     // 6 words of state
} state_t;

/* -------------------------------------------------------------------------- */
/*  Function under test                                                       */
/* -------------------------------------------------------------------------- */

static inline state_t AFFINE1(state_t s, int i, int d) {
    s.x[2].s[d].w[i] ^= s.x[1].s[d].w[i];
    s.x[0].s[d].w[i] ^= s.x[4].s[d].w[i];
    s.x[4].s[d].w[i] ^= s.x[3].s[d].w[i];
    return s;
}

/* -------------------------------------------------------------------------- */
/*  Debug printing and comparison helpers                                     */
/* -------------------------------------------------------------------------- */

static void print_state(const char* tag, const state_t* s) {
    printf("=== %s ===\n", tag);
    for (int xi = 0; xi < 6; xi++) {
        for (int si = 0; si < 2; si++) {
            printf("x[%d].s[%d].w[0]=0x%08x  x[%d].s[%d].w[1]=0x%08x\n",
                   xi, si,
                   s->x[xi].s[si].w[0],
                   xi, si,
                   s->x[xi].s[si].w[1]);
        }
    }
}

static int compare_states(const state_t* a, const state_t* b) {
    for (int xi = 0; xi < 6; xi++) {
        for (int si = 0; si < 2; si++) {
            for (int wi = 0; wi < 2; wi++) {
                if (a->x[xi].s[si].w[wi] != b->x[xi].s[si].w[wi]) {
                    printf("Mismatch at x[%d].s[%d].w[%d]: got 0x%08x, expected 0x%08x\n",
                           xi, si, wi,
                           a->x[xi].s[si].w[wi],
                           b->x[xi].s[si].w[wi]);
                    return 0;
                }
            }
        }
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/*  Main test                                                                 */
/* -------------------------------------------------------------------------- */

int main(void) {
    state_t s = {0}, golden = {0};

    /* --- Initialize input state --- */
    uint32_t init_vals[6][2][2] = {
        {{0x41255755, 0xa690c33d}, {0xe71abffe, 0x0d948972}},
        {{0xe0970a16, 0x06a4a69d}, {0x5385db92, 0xb269d4fe}},
        {{0xbe64296f, 0x9bf8c4b0}, {0x4648ba57, 0xb6ddc0ce}},
        {{0x42491bb2, 0xf4ed14f5}, {0xe1dda674, 0x55da5659}},
        {{0xe616ab53, 0x113d034a}, {0x2f43cbc7, 0xdaab215d}},
        {{0x0b3643f1, 0xba2139df}, {0x46da7eb9, 0x44273bf7}}
    };

    memcpy(&s.x, init_vals, sizeof(init_vals));

    /* --- Initialize golden output --- */
    uint32_t gold_vals[6][2][2] = {
        {{0x41255755, 0xb7adc077}, {0xe71abffe, 0x0d948972}},
        {{0xe0970a16, 0x06a4a69d}, {0x5385db92, 0xb269d4fe}},
        {{0xbe64296f, 0x9d5c622d}, {0x4648ba57, 0xb6ddc0ce}},
        {{0x42491bb2, 0xf4ed14f5}, {0xe1dda674, 0x55da5659}},
        {{0xe616ab53, 0xe5d017bf}, {0x2f43cbc7, 0xdaab215d}},
        {{0x0b3643f1, 0xba2139df}, {0x46da7eb9, 0x44273bf7}}
    };

    memcpy(&golden.x, gold_vals, sizeof(gold_vals));

    /* --- Print initial state --- */
    //print_state("AFFINE1 input", &s);
    /* --- Run function under test --- */
    state_t out = AFFINE1(s, 1, 0);

    /* --- Print output state --- */
    //print_state("AFFINE1 output", &out);

    /* --- Compare with golden --- */
    if (compare_states(&out, &golden)) {
        printf("\n✅ AFFINE1 test PASSED\n");
        return 0;
    } else {
        printf("\n❌ AFFINE1 test FAILED\n");
        return 1;
    }
}
