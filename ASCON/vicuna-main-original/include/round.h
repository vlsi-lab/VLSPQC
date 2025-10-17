#ifndef ROUND_H_
#define ROUND_H_


#include "constants.h"
#include "word.h"
#include "ascon.h"

//affine originale 
/* s.x[2].s[d].w[i] ^= s.x[1].s[d].w[i];
  s.x[0].s[d].w[i] ^= s.x[4].s[d].w[i];
  s.x[4].s[d].w[i] ^= s.x[3].s[d].w[i];
  */
  state_t AFFINE1(state_t s, int i, int d) {
  size_t vl = __riscv_vsetvl_e32m1(5);
  uint32_t src_array[5] = {s.x[0].s[d].w[i], s.x[1].s[d].w[i], s.x[2].s[d].w[i], s.x[3].s[d].w[i], s.x[4].s[d].w[i]};
  vuint32m1_t share = __riscv_vle32_v_u32m1(src_array, vl);
  vuint32m1_t v_res;
  uint32_t result[5];

  asm ("affine1.vv %0, %1\n" : "=vr"(v_res) : "vr" (share)); //c[16]===1ee34125fdba17443d01da8a0eefb045

  __riscv_vse32_v_u32m1(result, v_res, vl);


  s.x[0].s[d].w[i] = result[0];
  s.x[1].s[d].w[i] = result[1];
  s.x[2].s[d].w[i] = result[2];
  s.x[3].s[d].w[i] = result[3];
  s.x[4].s[d].w[i] = result[4];
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

static inline void ROUND_(state_t* p, uint8_t C_o, uint8_t C_e, int ns) {
  state_t s = *p;
  /* constant and sbox layer*/
  s.x[2].s[0].w[0] ^= C_e;
  s = SBOX(s, 0, ns);
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
