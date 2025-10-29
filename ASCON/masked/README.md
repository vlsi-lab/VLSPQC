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

```c
static inline  word_t MREUSE(word_t w, uint64_t val, int ns) {
  w.s[0].w[0] = (uint32_t)val;
  w.s[0].w[1] = val >> 32;
  w = MMIX(w, ns);
  //if (ns >= 2) w.s[1].w[0] = ROR32(w.s[1].w[0], 7); with ROR32 being x >> n | x << (-n & 31);
  //if (ns >= 2) w.s[1].w[1] = ROR32(w.s[1].w[1], 7);
  w = MEXPAND(w, 1, ns);
  //return MREDUCE(w, nso, nsi); and MREDUCE being:
  //  if (nsi >= 2 && nso < 2) w.s[0].w[0] ^= ROR32(w.s[1].w[0], ROT(1));
  //if (nsi >= 2 && nso < 2) w.s[0].w[1] ^= ROR32(w.s[1].w[1], ROT(1));
  return w;
}
```

#### Linear Diffusion Layer
The `LINEAR()` function performs rotations and XORs within each lane to spread bits across positions.
```c
if (ns >= 2) s = LINEAR(s, 1)
```
It operates per share `(d)`, so each share is diffused independently:
- Each lane `x[j]` is transformed by a sequence of XORs with rotated versions of itself (`ROR32()`).
- Then a second round of XORs is performed mixing intermediate values from `t.x[j]`.
- This step corresponds to ASCON’s linear diffusion layer, which provides avalanche and bit-mixing properties.

```c
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
```

### 🔁 The permutation sequence (PROUNDS)

Finally, the permutation is called with:
```c
PROUNDS(state_t* s, int nr, int ns)
```

This runs nr rounds, using constants provided by macros like `RC(i), START(nr), INC, and END`:
```c
int i = START(nr);
do {
  ROUND_(s, RC(i), ns);
  i += INC;
} while (i != END);
```

Each call to `ROUND_()` applies one masked nonlinear + linear round with unique round constants.


### 🔐 Summary — What happens when `ns = 2`
| Layer                           | Operation                            | Masking detail                                            |
| ------------------------------- | ------------------------------------ | --------------------------------------------------------- |
| **Constant addition**           | XOR round constants to `x[2].share0` | Constants are public — only share 0 is updated.           |
| **Affine1 + Toffoli + Affine2** | Implements S-box across lanes        | Nonlinear layer done with `MXORBIC`, preserving masking.  |
| **Bitwise NOT on x2.s[0]**      | Inversion as per ASCON S-box         | Applied to share 0 (no leakage since it’s linear).        |
| **Random reuse**                | Reuses `s.x[5]` as randomness pool   | Maintains fresh domain masks for next round.              |
| **Linear diffusion**            | XOR with rotated versions of itself  | Each share is diffused independently to preserve masking. |



## `crypto_aead_encrypt_shared()`

```c
static inline int crypto_aead_encrypt_shared(mask_c_uint32_t* cs, unsigned long long* clen,
                               const mask_m_uint32_t* ms,
                               unsigned long long mlen,
                               const mask_ad_uint32_t* ads,
                               unsigned long long adlen,
                               const mask_npub_uint32_t* npubs,
                               const mask_key_uint32_t* ks) {
  state_t s;
  *clen = mlen + CRYPTO_ABYTES;

  // 1) Init with key+nonce
  ascon_initaead(&s, npubs, ks);
  // 2) Absorb associated data
  ascon_adata(&s, ads, adlen);
  // 3) Encrypt message → ciphertext shares in cs[0 .. NUM_WORDS(mlen)-1]
  ascon_encrypt(&s, cs, ms, mlen);
  // 4) Finalize (keyed)
  ascon_final(&s, ks);
  // 5) Write authentication tag after ciphertext
  ascon_settag(&s, cs + NUM_WORDS(mlen));

  return 0;
}

```

### `ascon_initaead()` — Function Description

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



### `ascon_adata()` — Function Description

The **associated data absorption** (`ascon_adata`) mixes the (non-encrypted) AD into the masked ASCON state before message processing.  
It processes **full-rate blocks** of AD, then a **final partial block** with domain padding, and finally applies a **domain-separation bit**.

All operations are performed in the **masked domain** with `ns = 2` shares.

---

```c
static inline void ascon_adata(state_t* s, const mask_ad_uint32_t* ad, uint64_t adlen) {
  const int nr = ASCON_PB_ROUNDS;              // permutation rounds for data phase

  if (adlen) {
    /* 1) Absorb full AD blocks: for ASCON-128 rate, each block is 64 bits (2×32b words) */
    while (adlen >= ASCON_AEAD_RATE) {
      word_t as = MLOAD((uint32_t*)ad, NUM_SHARES_AD); // load one masked AD block
      s->x[0] = MXOR(s->x[0], as, NUM_SHARES_AD);      // absorb into x[0]
      P(s, nr, NUM_SHARES_AD);                         // permute after each full block
      adlen -= ASCON_AEAD_RATE;
      ad += 2;                                         // advance by 2×32-bit words = 64 bits
    }

    /* 2) Final (possibly partial) AD block: apply padding then absorb remaining bytes */
    s->x[0].s[0].w[1] ^= 0x80000000 >> (adlen * 4);    // 10* padding nibble at rate position
    if (adlen) {
      word_t as = MLOAD((uint32_t*)ad, NUM_SHARES_AD); // load trailing masked AD data
      s->x[0] = MXOR(s->x[0], as, NUM_SHARES_AD);      // absorb remainder
    }
    P(s, nr, NUM_SHARES_AD);                           // permute after final/partial block
  }

  /* 3) Domain separation between AD and message phases */
  s->x[4].s[0].w[0] ^= 1;                              // flip a public bit in share 0
}
```

- In many ASCON configurations, ASCON_AEAD_RATE is one 64-bit word (ASCON-128); your code increments ad += 2 (i.e., two 32-bit words), which matches a 64-bit rate.
- The padding `0x8…` line places the standard 10* padding at the correct nibble position (each byte = 2 nibbles; hence the * 4 shift factor).
- The permutation uses `ASCON_PB_ROUNDS` (the b rounds for data phase), distinct from the a rounds used in initialization.

| Step                                  | Operation                                                                                      | Description                                                                                                                                                                                            |
| ------------------------------------- | ---------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **1️⃣ Full AD blocks**                | `as = MLOAD(ad, NUM_SHARES_AD)` → `x[0] = MXOR(x[0], as, …)` → `P(s, ASCON_PB_ROUNDS, …)`      | While `adlen ≥ rate`, load one **masked** AD block, XOR-absorb it into lane `x[0]`, then **permute** with `PB` rounds. Decrease `adlen` by the **rate** and advance `ad`.                              |
| **2️⃣ Final partial block + padding** | `x[0].s[0].w[1] ^= 0x80000000 >> (adlen*4)` → (if `adlen`) `x[0] ^= MLOAD(ad)` → `P(s, PB, …)` | Apply **10*** padding at the **nibble** position that follows the remaining AD nibbles. If some AD bytes remain (< rate), load them masked and absorb into `x[0]`. Then run one more `PB` permutation. |
| **3️⃣ Domain separation**             | `x[4].s[0].w[0] ^= 1`                                                                          | Toggle a **public** bit in `x[4]` (share 0). This marks the boundary between **AD** and **message** phases to the permutation.                                                                         |


### `ascon_encrypt()` — Function Description

The **message encryption/absorption** mixes masked plaintext blocks into the state and emits **ciphertext shares**.  
It processes all **full-rate blocks** first, and then a **final partial block** with padding.  
All operations are masked with `ns = 2`.

---

```c
void ascon_encrypt(state_t* s, mask_c_uint32_t* c, const mask_m_uint32_t* m,
                   uint64_t mlen) {
  const int nr = ASCON_PB_ROUNDS;
  /* full plaintext blocks */
  while (mlen >= ASCON_AEAD_RATE) {
    word_t ms = MLOAD((uint32_t*)m, NUM_SHARES_M);
    s->x[0] = MXOR(s->x[0], ms, NUM_SHARES_M);
    MSTORE((uint32_t*)c, s->x[0], NUM_SHARES_C);
    P(s, nr, NUM_SHARES_M);
    mlen -= ASCON_AEAD_RATE;
    m += 2;
    c += 2;
  }
  /* final plaintext block */
  s->x[0].s[0].w[1] ^= 0x80000000 >> (mlen * 4);
  if (mlen) {
    word_t ms = MLOAD((uint32_t*)m, NUM_SHARES_M);
    s->x[0] = MXOR(s->x[0], ms, NUM_SHARES_M);
    MSTORE((uint32_t*)c, s->x[0], NUM_SHARES_C);
  }
}
```

| Step                        | Operation                                                                           | Description                                                                                                                                                                                                  |
| --------------------------- | ----------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **1️⃣ Full blocks loop**    | `msblk = MLOAD(m)` → `x[0] = MXOR(x[0], msblk)` → `MSTORE(c, x[0])` → `P(s, PB, …)` | For each full-rate plaintext block: load the **masked** block, **absorb** it into lane `x[0]`, **write ciphertext shares** (which equal the current `x[0]`), then **permute** to prepare for the next block. |
| **2️⃣ Padding**             | `x[0].s[0].w[1] ^= 0x80000000 >> (mlen * 4)`                                        | Apply **10*** padding at the nibble position immediately following the remaining plaintext nibbles (each byte = 2 nibbles → multiply remaining bytes by 2 → multiply by 2 bits per nibble = shift by `*4`).  |
| **3️⃣ Final partial block** | If `mlen > 0`: `msblk = MLOAD(m)` → `x[0] = MXOR(x[0], msblk)` → `MSTORE(c, x[0])`  | Absorb the remaining plaintext (shorter than one full rate) and write the final ciphertext shares. (No permutation here; the next call, `ascon_final`, will handle keying for the tag.)                      |


### `ascon_final()` — Function Description

The **finalization** phase re-keys the masked state to derive the authentication tag.  
It loads the masked key, XORs it into lanes `x[1]` and `x[2]`, runs the **a-round** permutation, then XORs the key again into `x[3]` and `x[4]`.  
All operations are performed in the masked domain with `ns = 2`.

---

```c
static inline void ascon_final(state_t* s, const mask_key_uint32_t* k) {
  word_t K1, K2;
  K1 = MLOAD((uint32_t*)k, NUM_SHARES_KEY);
  K2 = MLOAD((uint32_t*)(k + 2), NUM_SHARES_KEY);
  /* first key xor (first 64-bit word) */
  s->x[1] = MXOR(s->x[1], K1, NUM_SHARES_KEY);
  /* first key xor (second 64-bit word) */
  s->x[2] = MXOR(s->x[2], K2, NUM_SHARES_KEY);
  /* compute the permutation */
  P(s, ASCON_PA_ROUNDS, NUM_SHARES_KEY);
  /* second key xor (first 64-bit word) */
  s->x[3] = MXOR(s->x[3], K1, NUM_SHARES_KEY);
  /* second key xor (second 64-bit word) */
  s->x[4] = MXOR(s->x[4], K2, NUM_SHARES_KEY);
}

```

| Step                    | Operation                           | Description                                                                                    |
| ----------------------- | ----------------------------------- | ---------------------------------------------------------------------------------------------- |
| **1️⃣ Load key shares** | `K1 = MLOAD(k)` • `K2 = MLOAD(k+2)` | Load the masked **128-bit key** as two 64-bit lanes (`K1`, `K2`).                              |
| **2️⃣ First key XOR**   | `x[1] ^= K1` • `x[2] ^= K2`         | Absorb the key into lanes `x[1]` and `x[2]` (masked XOR on both shares).                       |
| **3️⃣ Permutation**     | `P(s, ASCON_PA_ROUNDS, …)`          | Run the **a-round** permutation to mix the keyed state.                                        |
| **4️⃣ Second key XOR**  | `x[3] ^= K1` • `x[4] ^= K2`         | Re-inject the key into lanes that will hold the **authentication tag** (after `ascon_settag`). |

After `ascon_final()`, the state contains the tag material.
The subsequent call `ascon_settag(&s, cs + NUM_WORDS(mlen))` will serialize the masked tag shares to the output buffer.


### `ascon_settag()` and `ascon_iszero()`— Function Description

After finalization, the **authentication tag** resides in lanes `x[3]‖x[4]` of the masked state.  
`ascon_settag()` **serializes** these two 64-bit lanes (each masked) to the output buffer right **after the ciphertext**.  
`ascon_iszero()` is a **constant-time check** used for tag verification: it **recombines shares** (rotation-aware) into share 0, folds all 64+64 bits to a single flag, and returns **0 if tag==0**, **−1 otherwise**.

> ⚠️ `ascon_iszero()` **modifies** `s->x[3]` and `s->x[4]` (it folds other shares into `s[0]`). If you need a non-destructive check, see the alternative below.

---

```c
static inline void ascon_settag(state_t* s, mask_c_uint32_t* t) {
  MSTORE((uint32_t*)t, s->x[3], NUM_SHARES_C);
  MSTORE((uint32_t*)(t + 2), s->x[4], NUM_SHARES_C);
}
```

```c
static inline int ascon_iszero(state_t* s) {
#if NUM_SHARES_KEY >= 2
  s->x[3].s[0].w[0] ^= ROR32(s->x[3].s[1].w[0], ROT(1));
  s->x[3].s[0].w[1] ^= ROR32(s->x[3].s[1].w[1], ROT(1));
  s->x[4].s[0].w[0] ^= ROR32(s->x[4].s[1].w[0], ROT(1));
  s->x[4].s[0].w[1] ^= ROR32(s->x[4].s[1].w[1], ROT(1));
#endif
  uint32_t result;
  result  = s->x[3].s[0].w[0] ^ s->x[3].s[0].w[1];
  result ^= s->x[4].s[0].w[0] ^ s->x[4].s[0].w[1];
  result |= result >> 16;
  result |= result >> 8;
  return ((((int)(result & 0xff) - 1) >> 8) & 1) - 1;  // 0 if tag==0, else -1
}
```

| Function       | Step | Operation            | Description                                                                                                                                                                                |                      |                |
| -------------- | ---- | -------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | -------------------- | -------------- |
| `ascon_settag` | 1    | `MSTORE(t, x[3])`    | Store the **first 64-bit tag lane** (masked) to `t[0..1]` (two 32-bit words).                                                                                                              |                      |                |
|                | 2    | `MSTORE(t+2, x[4])`  | Store the **second 64-bit tag lane** to `t[2..3]`. Tag length is 16 bytes total.                                                                                                           |                      |                |
| `ascon_iszero` | 1    | Fold shares → `s[0]` | For each lane (`x[3]`, `x[4]`), XOR `s[0]` with **rotated** higher shares: for `ns=2`, `s[0] ^= ROR32(s[1], ROT(1))` on both halves. (For `ns≥3/4`, also fold shares 2/3 with `ROT(2/3)`.) |                      |                |
|                | 2    | Reduce lanes         | XOR low/high halves of `x[3].s[0]` and `x[4].s[0]` into `result`.                                                                                                                          |                      |                |
|                | 3    | Constant-time fold   | Propagate any set bit into the low byte with `result                                                                                                                                       | = result>>16; result | = result>>8;`. |
|                | 4    | Return flag          | `((((result&0xff)-1)>>8)&1)-1` → **returns 0 if `result==0`**, otherwise **−1**. No branches, constant-time.                                                                               |                      |                |

