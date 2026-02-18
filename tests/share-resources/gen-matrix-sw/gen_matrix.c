/*
 * Gen Matrix Test - SOFTWARE IMPLEMENTATION
 * Replicates Kyber gen_matrix functionality (gen_a and gen_at)
 * Uses SHAKE128 XOF for deterministic sampling
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "fips202.h"
#include "kyber_params.h"

/* Rejection sampling: convert uniform random bytes to uniform random coefficients mod KYBER_Q */
static unsigned int rej_uniform(int16_t *r,
                                unsigned int len,
                                const uint8_t *buf,
                                unsigned int buflen)
{
  unsigned int ctr, pos;
  uint16_t val0, val1;

  ctr = pos = 0;
  while(ctr < len && pos + 3 <= buflen) {
    val0 = ((buf[pos+0] >> 0) | ((uint16_t)buf[pos+1] << 8)) & 0xFFF;
    val1 = ((buf[pos+1] >> 4) | ((uint16_t)buf[pos+2] << 4)) & 0xFFF;
    pos += 3;

    if(val0 < KYBER_Q)
      r[ctr++] = val0;
    if(ctr < len && val1 < KYBER_Q)
      r[ctr++] = val1;
  }

  return ctr;
}

/* XOF absorption interface */
static void xof_absorb(xof_state *state, const uint8_t seed[KYBER_SYMBYTES], uint8_t i, uint8_t j)
{
  shake128_absorb_once(state, seed, KYBER_SYMBYTES);
  /* Domain separation: append (j, i) bytes */
  uint8_t ij[2] = {i, j};
  shake128_absorb(state, ij, 2);
  shake128_finalize(state);
}

/* XOF squeeze blocks */
static void xof_squeezeblocks(uint8_t *out, size_t nblocks, xof_state *state)
{
  shake128_squeezeblocks(out, nblocks, state);
}

#define GEN_MATRIX_NBLOCKS ((12*KYBER_N/8*(1 << 12)/KYBER_Q + XOF_BLOCKBYTES)/XOF_BLOCKBYTES)

/*
 * gen_matrix - Deterministically generate matrix A or A^T
 * Uses SHAKE128 XOF with rejection sampling
 */
void gen_matrix(polyvec *a, const uint8_t seed[KYBER_SYMBYTES], int transposed)
{
  unsigned int ctr, i, j;
  unsigned int buflen;
  uint8_t buf[GEN_MATRIX_NBLOCKS*XOF_BLOCKBYTES];
  xof_state state;

  for(i=0; i<KYBER_K; i++) {
    for(j=0; j<KYBER_K; j++) {
      if(transposed)
        xof_absorb(&state, seed, i, j);
      else
        xof_absorb(&state, seed, j, i);

      xof_squeezeblocks(buf, GEN_MATRIX_NBLOCKS, &state);
      buflen = GEN_MATRIX_NBLOCKS*XOF_BLOCKBYTES;
      ctr = rej_uniform(a[i].vec[j].coeffs, KYBER_N, buf, buflen);

      while(ctr < KYBER_N) {
        xof_squeezeblocks(buf, 1, &state);
        buflen = XOF_BLOCKBYTES;
        ctr += rej_uniform(a[i].vec[j].coeffs + ctr, KYBER_N - ctr, buf, buflen);
      }
    }
  }
}

/* Macro wrappers */
void gen_a(polyvec *a, const uint8_t seed[KYBER_SYMBYTES])
{
  gen_matrix(a, seed, 0);
}

void gen_at(polyvec *a, const uint8_t seed[KYBER_SYMBYTES])
{
  gen_matrix(a, seed, 1);
}
