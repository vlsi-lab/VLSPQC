#include "dilithium_params.h"
#include "fips202.h"
#include <string.h>

unsigned int poly_uniform(poly *a, const uint8_t seed[SEEDBYTES], uint16_t nonce) {
    unsigned int ctr = 0, pos = 0;
    uint8_t buf[POLY_UNIFORM_NBLOCKS * SHAKE128_RATE];
    uint8_t state_input[34];
    keccak_state state;

    memcpy(state_input, seed, SEEDBYTES);
    state_input[32] = nonce & 0xFF;
    state_input[33] = nonce >> 8;

    shake128_absorb_once(&state, state_input, 34);
    shake128_squeezeblocks(buf, POLY_UNIFORM_NBLOCKS, &state);

    while(ctr < DILITHIUM_N && pos + 3 <= sizeof(buf)) {
        uint32_t t = buf[pos++];
        t |= (uint32_t)buf[pos++] << 8;
        t |= (uint32_t)buf[pos++] << 16;
        t &= 0x7FFFFF; 

        if(t < DILITHIUM_Q) a->coeffs[ctr++] = t;
    }
    return ctr;
}

unsigned int poly_uniform_eta(poly *a, const uint8_t seed[CRHBYTES], uint16_t nonce) {
    unsigned int ctr = 0, pos = 0;
    uint8_t buf[SHAKE256_RATE];
    uint8_t state_input[66];
    keccak_state state;

    memcpy(state_input, seed, CRHBYTES);
    state_input[64] = nonce & 0xFF;
    state_input[65] = nonce >> 8;

    shake256_absorb_once(&state, state_input, 66);
    shake256_squeezeblocks(buf, 1, &state);

    while(ctr < DILITHIUM_N && pos < SHAKE256_RATE) {
        uint8_t t0 = buf[pos] & 0x0F;
        uint8_t t1 = buf[pos++] >> 4;

        if(t0 < 5) a->coeffs[ctr++] = DILITHIUM_ETA - t0;
        if(ctr < DILITHIUM_N && t1 < 5) a->coeffs[ctr++] = DILITHIUM_ETA - t1;
    }
    return ctr;
}

void poly_uniform_gamma1(poly *a, const uint8_t seed[CRHBYTES], uint16_t nonce) {
    uint8_t buf[576]; // 256 * 18 bits / 8 = 576 bytes
    uint8_t state_input[66];
    keccak_state state;

    memcpy(state_input, seed, CRHBYTES);
    state_input[64] = nonce & 0xFF;
    state_input[65] = nonce >> 8;

    shake256_absorb_once(&state, state_input, 66);
    shake256_squeeze(buf, 576, &state);

    /* Bit-unpacking: 18 bits per coefficient */
    for(int i = 0; i < DILITHIUM_N/4; i++) {
        a->coeffs[4*i+0] = (buf[9*i+0]      | (uint32_t)buf[9*i+1] << 8 | (uint32_t)buf[9*i+2] << 16) & 0x3FFFF;
        a->coeffs[4*i+1] = ((buf[9*i+2]>>2) | (uint32_t)buf[9*i+3] << 6 | (uint32_t)buf[9*i+4] << 14) & 0x3FFFF;
        a->coeffs[4*i+2] = ((buf[9*i+4]>>4) | (uint32_t)buf[9*i+5] << 4 | (uint32_t)buf[9*i+6] << 12) & 0x3FFFF;
        a->coeffs[4*i+3] = ((buf[9*i+6]>>6) | (uint32_t)buf[9*i+7] << 2 | (uint32_t)buf[9*i+8] << 10) & 0x3FFFF;
        
        for(int j=0; j<4; j++) a->coeffs[4*i+j] = DILITHIUM_GAMMA1 - a->coeffs[4*i+j];
    }
}