# 🧩 Masked ASCON AEAD

This document describes the initialization phase of a **masked ASCON AEAD** implementation, focusing on:
- The `ascon_initaead()` function — how it initializes the masked state
- The `MZERO()` routine — how it creates a “masked zero” for a given number of shares (`ns`)
- Debug print utilities for inspecting masked shares (with `ns = 2`)
- Rotation-aware reconstruction of masked values for debugging

These are the main structures used:
```c
typedef struct {
  uint32_t w[2];
} share_t;

typedef struct {
  share_t s[NUM_SHARES_KEY];
} word_t;

typedef struct {
  word_t x[6];
} state_t;
```

---

## `P()` - Function

The ASCON permutation transforms the internal state `s`, which consists of five 64-bit words (`x[0]–x[4]`), plus a temporary lane `x[5]` used for randomness reuse in masked versions.

Each lane is split into multiple shares for masking:
```c
s.x[j].s[d].w[i] //j is lane index, d is the share index {0,1 when ns=2}
// w[i] 32-bit half of the 64-bit lane (low or high part)
```
The permutation consists of rounds (each round uses a constant C) with three main layers:
- Constant addition
- Substitution (S-box)
- Linear diffusion

### ⚙️ Structure of one round
```c
ROUND_(state_t* p, uint8_t C_o, uint8_t C_e, int ns)
```

#### Round constants
```c
s.x[2].s[0].w[0] ^= C_e;
s = SBOX(s, 0, ns);
s.x[2].s[0].w[1] ^= C_o;
s = SBOX(s, 1, ns);
```
- Constants `C_e` and `C_o` (even/odd) are XORed into share 0 of `x[2]`.
- This injects the round constant only once per share, since constants are public values.
- After each constant injection, the S-box layer is applied (on both halves `i = 0, 1`).

#### Substitution (S-box) [non linear]

This is done by `SBOX(s, i, ns)`.

Inside SBOX:
- Affine layer 1:
```c
s = AFFINE1(s, i, d);
```
Applied for all shares d. It mixes certain lanes with XORs to prepare for the nonlinear operation. It compute:
```c
state_t AFFINE1(state_t s, int i, int d) { //d being equal to ns
    s.x[2].s[d].w[i] ^= s.x[1].s[d].w[i];
    s.x[0].s[d].w[i] ^= s.x[4].s[d].w[i];
    s.x[4].s[d].w[i] ^= s.x[3].s[d].w[i];
    return s;
}
```
This is equivalent to part of the linear transformation before the AND-based nonlinear part (Toffoli gates).

- Nonlinear Toffoli gates (masked)
```c
s.x[5] = MXORBIC(s.x[5], s.x[4], s.x[3], i, ns);
s.x[4] = MXORBIC(s.x[4], s.x[1], s.x[0], i, ns);
s.x[1] = MXORBIC(s.x[1], s.x[3], s.x[2], i, ns);
s.x[3] = MXORBIC(s.x[3], s.x[0], s.x[4], i, ns);
s.x[0] = MXORBIC(s.x[0], s.x[2], s.x[1], i, ns);
```
- `MXORBIC` implements a masked AND/XOR combination corresponding to the ASCON S-box logic.
- It uses Toffoli-like operations to mix lanes in a nonlinear way while respecting masking.
- With `ns=2`, each lane is represented by two shares (`s0, s1`). The operation computes bitwise nonlinear functions in a domain-oriented masking (DOM) style:
```c
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
  ....
```
where:
```c
static inline void EOR_BIC_ROR(uint32_t *ce, uint32_t ae, uint32_t be, uint32_t imm, uint32_t *tmp) {
	uint32_t rot_imm;
	rot_imm = ROT(imm);  //return (((imm) * (ASCON_ROR_SHARES)) & 31)
    //*tmp = ae & ~((be >> rot_imm) | (be << (-rot_imm & 31)));
	*tmp = ae & ~(ROR32(be,rot_imm)); //return be >> rot_imm | be << (-rot_imm & 31);
    *ce ^= *tmp;
}
```

- Affine layer 2:
```c
s = AFFINE2(s, i, d);
s.x[2].s[0].w[i] = ~s.x[2].s[0].w[i]; // inversion
```
The affine layer rearranges XOR dependencies again.
```c
state_t AFFINE2(state_t s, int i, int d) {
  s.x[2].s[d].w[i] ^= s.x[5].s[d].w[i];
  s.x[1].s[d].w[i] ^= s.x[0].s[d].w[i];
  s.x[0].s[d].w[i] ^= s.x[4].s[d].w[i];
  s.x[3].s[d].w[i] ^= s.x[2].s[d].w[i];
  return s;
}
```
Then the bitwise NOT (~) is applied on `x2`’s first share (completing the S-box inversion pattern). 
The combination of affine layers + Toffoli gates reproduces the ASCON S-box, but now in masked form.

#### Randomness reuse
```c
s.x[5] = MREUSE(s.x[5], 0, ns);
```
The temporary randomness used during masking (`s.x[5]`) is rotated/reused for the next round to avoid bias while minimizing RNG consumption.



` `

```c

```



## `ascon_initaead()` — Function Description


The ASCON AEAD initialization (`ascon_initaead`) sets up the internal masked state before encryption begins.  
It combines:
- A randomized starting state
- The ASCON IV and domain parameters
- The masked **key** and **nonce**
- The **permutation** and second key XOR step required by the ASCON specification

All operations are performed in a **masked domain**, where every word is represented as multiple shares (`ns` shares).  
For this build: `ns = 2`.

---


```c
static inline void ascon_initaead(state_t* s,
                                  const mask_npub_uint32_t* n,
                                  const mask_key_uint32_t* k) {
  word_t N0, N1;
  word_t K1, K2;

  /* 1️⃣ Randomize / masked-zero part of the state */
  s->x[5] = MZERO(NUM_SHARES_KEY);
  s->x[0] = MZERO(NUM_SHARES_KEY);

  /* 2️⃣ Inject ASCON IV/domain constants into share 0 of lane x[0] */
  s->x[0].s[0].w[0] ^= 0x08220000; //first share s[0] of word 0
  s->x[0].s[0].w[1] ^= 0x80210000; //first share s[0] of word 1

  /* 3️⃣ Load masked nonce (two 64-bit lanes) */
  /*MLOAD loads masked words from the nonce share array n.*/
  s->x[3] = N0 = MLOAD((uint32_t*)n, NUM_SHARES_NPUB);
  s->x[4] = N1 = MLOAD((uint32_t*)(n + 2), NUM_SHARES_NPUB);

  /* 4️⃣ Load masked key (two 64-bit lanes) */
  s->x[1] = K1 = MLOAD((uint32_t*)k, NUM_SHARES_KEY);
  s->x[2] = K2 = MLOAD((uint32_t*)(k + 2), NUM_SHARES_KEY);

  /* 5️⃣ Compute the ASCON permutation with 'a' rounds */
  /*Run the ASCON permutation with the a-rounds (initial round count). This mixes IV, key, and nonce in the state in a masked-safe way. */
  P(s, ASCON_PA_ROUNDS, NUM_SHARES_KEY);

  /* 6️⃣ XOR the key again (second key injection) */
  s->x[3] = MXOR(s->x[3], K1, NUM_SHARES_KEY);
  s->x[4] = MXOR(s->x[4], K2, NUM_SHARES_KEY);
}
```
---
| Step                            | Operation                                                                                                              | Description                                                                                                                                                                                                                           |
| ------------------------------- | ---------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **1️⃣ Randomize / Masked-Zero** | `s->x[5] = MZERO(NUM_SHARES_KEY);`<br>`s->x[0] = MZERO(NUM_SHARES_KEY);`                                               | Initializes lanes `x[5]` and `x[0]` to **masked zero**. For `ns = 2`, share1 is fresh random (rotated by 7), and share0 is `ROR32(share1, ROT(1))`, so the reconstruction rule `value = share0 ^ ROR32(share1, ROT(1))` yields **0**. |
| **2️⃣ Inject IV**               | `s->x[0].s[0].w[0] ^= 0x08220000;`<br>`s->x[0].s[0].w[1] ^= 0x80210000;`                                               | Writes ASCON IV/domain parameters into **share 0** of lane `x[0]`. Constants are public, so only share0 is modified to set the intended value while preserving masking.                                                               |
| **3️⃣ Load Nonce**              | `s->x[3] = N0 = MLOAD((uint32_t*)n, NUM_SHARES_NPUB);`<br>`s->x[4] = N1 = MLOAD((uint32_t*)(n + 2), NUM_SHARES_NPUB);` | Loads the 128-bit nonce into lanes `x[3]` and `x[4]` as **masked words**. `MLOAD` imports the share-structured data into the state (two 64-bit halves).                                                                               |
| **4️⃣ Load Key**                | `s->x[1] = K1 = MLOAD((uint32_t*)k, NUM_SHARES_KEY);`<br>`s->x[2] = K2 = MLOAD((uint32_t*)(k + 2), NUM_SHARES_KEY);`   | Loads the 128-bit masked key into lanes `x[1]` and `x[2]`. Temporaries `K1` and `K2` are kept for the second key XOR after permutation.                                                                                               |
| **5️⃣ Permutation**             | `P(s, ASCON_PA_ROUNDS, NUM_SHARES_KEY);`                                                                               | Applies the ASCON permutation with the **a-rounds**, mixing IV, key, and nonce in the **masked domain** (DOM-compatible operations).                                                                                                  |
| **6️⃣ Second Key XOR**          | `s->x[3] = MXOR(s->x[3], K1, NUM_SHARES_KEY);`<br>`s->x[4] = MXOR(s->x[4], K2, NUM_SHARES_KEY);`                       | Re-injects the key into lanes `x[3]`/`x[4]` after the permutation, completing ASCON’s initialization schedule. The state is now ready for AD absorption and encryption.                                                               |



 
```c

```