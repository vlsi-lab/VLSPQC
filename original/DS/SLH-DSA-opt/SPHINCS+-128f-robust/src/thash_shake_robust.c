#include <stdint.h>
#include <string.h>

#include "thash.h"
#include "address.h"
#include "params.h"
#include "utils.h"

#include "fips202.h"

/* Contiguous layout for the thash SHAKE256 input: pub_seed || addr || data.
   The flexible array member 'data' has no compile-time size; the caller
   allocates raw_buf large enough to hold inblocks*SPX_N bytes after the
   fixed fields.  __attribute__((packed)) prevents padding between fields. */
typedef struct __attribute__((packed)) {
    uint8_t pub_seed[SPX_N];
    uint8_t addr[SPX_ADDR_BYTES];
    uint8_t data[];
} thash_input_t;

/**
 * Takes an array of inblocks concatenated arrays of SPX_N bytes.
 */
void thash(unsigned char *out, const unsigned char *in, unsigned int inblocks,
           const spx_ctx *ctx, uint32_t addr[8])
{
    SPX_VLA(uint8_t, raw_buf, SPX_N + SPX_ADDR_BYTES + inblocks*SPX_N);
    thash_input_t *input = (thash_input_t *)raw_buf;
    SPX_VLA(uint8_t, bitmask, inblocks * SPX_N);
    unsigned int i;

    memcpy(input->pub_seed, ctx->pub_seed, SPX_N);
    memcpy(input->addr, addr, SPX_ADDR_BYTES);

    shake256(bitmask, inblocks * SPX_N, (const uint8_t *)input, SPX_N + SPX_ADDR_BYTES);

    for (i = 0; i < inblocks * SPX_N; i++) {
        input->data[i] = in[i] ^ bitmask[i];
    }

    shake256(out, SPX_N, (const uint8_t *)input, SPX_N + SPX_ADDR_BYTES + inblocks*SPX_N);
}
