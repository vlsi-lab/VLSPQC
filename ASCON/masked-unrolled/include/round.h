#ifndef ROUND_H_
#define ROUND_H_


#include "constants.h"
#include "word.h"
#include "ascon.h"

#include <stdint.h>


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

//Per riordinare o selezionare elementi specifici da più vettori, puoi utilizzare 
//l'intrinsic __riscv_vrgather_vv_*, che raccoglie elementi da un vettore sorgente in base a un indice fornito da un altro vettore:
/*vuint32m1_t src = ...;       // Vettore sorgente
vuint32m1_t indices = ...;    // Vettore degli indici
vuint32m1_t result = __riscv_vrgather_vv_u32m1(src, indices, vl);
*/
  if (ns >= 1) s = AFFINE2(s, i, 0);
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


static inline word_t MTOFFOLI(word_t a, word_t b, word_t c, int share, int ns);



static inline void ROUND_(state_t* p, uint8_t C_o, uint8_t C_e, int ns) {
  state_t s = *p;
  /* constant and sbox layer*/
  s.x[2].s[0].w[0] ^= C_e;

  //s = SBOX(s, 0, ns);
  //s = AFFINE1(s, 0, 0);
  s.x[2].s[0].w[0] ^= s.x[1].s[0].w[0];
  s.x[0].s[0].w[0] ^= s.x[4].s[0].w[0];
  s.x[4].s[0].w[0] ^= s.x[3].s[0].w[0];

  //s = AFFINE1(s, 0, 1);
  s.x[2].s[1].w[0] ^= s.x[1].s[1].w[0];
  s.x[0].s[1].w[0] ^= s.x[4].s[1].w[0];
  s.x[4].s[1].w[0] ^= s.x[3].s[1].w[0];


  s.x[5] = MXORBIC (s.x[5], s.x[4], s.x[3], 0, ns);
  s.x[4] = MXORBIC(s.x[4], s.x[1], s.x[0], 0, ns);
  s.x[1] = MXORBIC(s.x[1], s.x[3], s.x[2], 0, ns);
  s.x[3] = MXORBIC(s.x[3], s.x[0], s.x[4], 0, ns);
  s.x[0] = MXORBIC(s.x[0], s.x[2], s.x[1], 0, ns);
  s = AFFINE2(s, 0, 0);
  s.x[2].s[0].w[0] = ~s.x[2].s[0].w[0];
  s = AFFINE2(s, 0, 1);

  s.x[2].s[0].w[1] ^= C_o;

  //s = SBOX(s, 1, ns);
  //s = AFFINE1(s, 1, 0);
  s.x[2].s[0].w[1] ^= s.x[1].s[0].w[1];
  s.x[0].s[0].w[1] ^= s.x[4].s[0].w[1];
  s.x[4].s[0].w[1] ^= s.x[3].s[0].w[1];

  //s = AFFINE1(s, 1, 1);
  s.x[2].s[1].w[1] ^= s.x[1].s[1].w[1];
  s.x[0].s[1].w[1] ^= s.x[4].s[1].w[1];
  s.x[4].s[1].w[1] ^= s.x[3].s[1].w[1];

  s.x[5] = MXORBIC (s.x[5], s.x[4], s.x[3], 1, ns);
  s.x[4] = MXORBIC(s.x[4], s.x[1], s.x[0], 1, ns);
  s.x[1] = MXORBIC(s.x[1], s.x[3], s.x[2], 1, ns);
  s.x[3] = MXORBIC(s.x[3], s.x[0], s.x[4], 1, ns);
  s.x[0] = MXORBIC(s.x[0], s.x[2], s.x[1], 1, ns);
  s = AFFINE2(s, 1, 0);
  s.x[2].s[0].w[1] = ~s.x[2].s[0].w[1];
  s = AFFINE2(s, 1, 1);

  /* reuse rotated randomness */
  s.x[5] = MREUSE(s.x[5], 0, ns);

  /* linear layer*/
  s = LINEAR(s, 1);
  s = LINEAR(s, 0);
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
