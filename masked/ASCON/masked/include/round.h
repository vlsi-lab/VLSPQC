#ifndef ROUND_H_
#define ROUND_H_


#include "constants.h"
#include "word.h"
#include "ascon.h"

#include <stdint.h>

#if DEBUG_CODE==1
  static inline void debug_print_state(const char* tag, const state_t* s, int ns) {
    // assumes x[0..5], shares up to ns, and two 32-bit words per share (w[0], w[1])
    printf("=== %s ===\n", tag);
    for (int x = 0; x < 6; ++x) {
      for (int d = 0; d < ns; ++d) {
        uint32_t w0 = s->x[x].s[d].w[0];
        uint32_t w1 = s->x[x].s[d].w[1];
        printf("x[%d].s[%d].w[0]=0x%08" PRIx32 "  x[%d].s[%d].w[1]=0x%08" PRIx32 "\n",
              x, d, w0, x, d, w1);
      }
    }
    printf("====================\n");
  }

  // Trace a full state around a function that returns a state_t
#define TRACE_STATE_CALL(tag, S_expr, ns)            \
  do {                                               \
    debug_print_state(tag " (before)", &(S_expr), ns); \
  } while (0)

// Use this when you do: s = FUNC(s, ...);
#define TRACE_AFTER_STATE(tag, S_expr, ns)           \
  do {                                               \
    debug_print_state(tag " (after)", &(S_expr), ns);  \
  } while (0)

// For one-line updates that modify s in-place (e.g., MXORBIC to a lane)
#define TRACE_STATE_LINE_BEFORE(tag, s_ref, ns) \
  do { debug_print_state(tag " (before)", &(s_ref), ns); } while (0)

#define TRACE_STATE_LINE_AFTER(tag, s_ref, ns) \
  do { debug_print_state(tag " (after)",  &(s_ref), ns); } while (0)


#endif
/* Your original function, now portable with a scalar fallback */
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

  if (ns >= 1){
    TRACE_STATE_CALL("AFFINE1 share=0", s, ns);
    printf("AFFINE 1 called for share 0 wit i=%d\n", 1);
    s = AFFINE1(s, i, 0);
    TRACE_AFTER_STATE("AFFINE1 share=0", s, ns);
  }

  if (ns >= 2) s = AFFINE1(s, i, 1);
  if (ns >= 3) s = AFFINE1(s, i, 2);
  if (ns >= 4) s = AFFINE1(s, i, 3);
  

  /* Toffoli gates */
  TRACE_STATE_LINE_BEFORE("MXORBIC x5 = f(x5,x4,x3)", s, ns);
  s.x[5] = MXORBIC(s.x[5], s.x[4], s.x[3], i, ns);
  TRACE_STATE_LINE_AFTER ("MXORBIC x5 = f(x5,x4,x3)", s, ns);

  s.x[4] = MXORBIC(s.x[4], s.x[1], s.x[0], i, ns);
  s.x[1] = MXORBIC(s.x[1], s.x[3], s.x[2], i, ns);
  s.x[3] = MXORBIC(s.x[3], s.x[0], s.x[4], i, ns);
  s.x[0] = MXORBIC(s.x[0], s.x[2], s.x[1], i, ns);
  /* affine layer 2 */
  if (ns >= 1){
    TRACE_STATE_CALL("AFFINE2 share=0", s, ns);
    printf("AFFINE 2 called for share 0 wit i=%d\n", 1);
    s = AFFINE2(s, i, 0);
    TRACE_AFTER_STATE("AFFINE2 share=0", s, ns);
  }
  s.x[2].s[0].w[i] = ~s.x[2].s[0].w[i];
  if (ns >= 2) s = AFFINE2(s, i, 1);
  if (ns >= 3) s = AFFINE2(s, i, 2);
  if (ns >= 4) s = AFFINE2(s, i, 3);
  return s;
}

static inline state_t LINEAR(state_t s, int d) {
  state_t t;
  t.x[0].s[d].w[0] = s.x[0].s[d].w[0] ^ ROR32(s.x[0].s[d].w[1], 4);
  t.x[0].s[d].w[1] = s.x[0].s[d].w[1] ^ ROR32(s.x[0].s[d].w[0], 5);
  t.x[1].s[d].w[0] = s.x[1].s[d].w[0] ^ ROR32(s.x[1].s[d].w[0], 11);
  t.x[1].s[d].w[1] = s.x[1].s[d].w[1] ^ ROR32(s.x[1].s[d].w[1], 11);
  t.x[2].s[d].w[0] = s.x[2].s[d].w[0] ^ ROR32(s.x[2].s[d].w[1], 2);
  t.x[2].s[d].w[1] = s.x[2].s[d].w[1] ^ ROR32(s.x[2].s[d].w[0], 3);
  t.x[3].s[d].w[0] = s.x[3].s[d].w[0] ^ ROR32(s.x[3].s[d].w[1], 3);
  t.x[3].s[d].w[1] = s.x[3].s[d].w[1] ^ ROR32(s.x[3].s[d].w[0], 4);
  t.x[4].s[d].w[0] = s.x[4].s[d].w[0] ^ ROR32(s.x[4].s[d].w[0], 17);
  t.x[4].s[d].w[1] = s.x[4].s[d].w[1] ^ ROR32(s.x[4].s[d].w[1], 17);
  s.x[0].s[d].w[0] ^= ROR32(t.x[0].s[d].w[1], 9);
  s.x[0].s[d].w[1] ^= ROR32(t.x[0].s[d].w[0], 10);
  s.x[1].s[d].w[0] ^= ROR32(t.x[1].s[d].w[1], 19);
  s.x[1].s[d].w[1] ^= ROR32(t.x[1].s[d].w[0], 20);
  s.x[2].s[d].w[0] ^= ROR32(t.x[2].s[d].w[1], 0);
  s.x[2].s[d].w[1] ^= ROR32(t.x[2].s[d].w[0], 1);
  s.x[3].s[d].w[0] ^= ROR32(t.x[3].s[d].w[0], 5);
  s.x[3].s[d].w[1] ^= ROR32(t.x[3].s[d].w[1], 5);
  s.x[4].s[d].w[0] ^= ROR32(t.x[4].s[d].w[1], 3);
  s.x[4].s[d].w[1] ^= ROR32(t.x[4].s[d].w[0], 4);
  return s;
}

static inline void ROUND_(state_t* p, uint8_t C_o, uint8_t C_e, int ns) {
  state_t s = *p;
  /* constant and sbox layer*/
  s.x[2].s[0].w[0] ^= C_e;

  TRACE_STATE_CALL("ROUND_: SBOX i=0", s, ns);
  s = SBOX(s, 0, ns);
  TRACE_AFTER_STATE("ROUND_: SBOX i=0", s, ns);

  s.x[2].s[0].w[1] ^= C_o;
  s = SBOX(s, 1, ns);
  /* reuse rotated randomness */
  s.x[5] = MREUSE(s.x[5], 0, ns);
  /* linear layer*/
  if (ns >= 4) s = LINEAR(s, 3);
  if (ns >= 3) s = LINEAR(s, 2);
  if (ns >= 2) s = LINEAR(s, 1);
  if (ns >= 1) s = LINEAR(s, 0);
  *p = s;
}

static inline void ROUND(state_t* p, uint64_t C, int ns) {
  ROUND_(p, C >> 32, C, ns);
}

static inline void PROUNDS(state_t* s, int nr, int ns) {
  int i = START(nr);
  do {
    ROUND_(s, RC(i), ns);
    i += INC;
  } while (i != END);
}

#endif /* ROUND_H_ */
