#ifndef BFU_H
#define BFU_H

#include <stdint.h>
#include "kyber.h"
#include "dilithium.h"
#include "falcon.h"
#include "hqc.h"

typedef enum {
    OP_BFNTTK,   // Kyber Forward
    OP_BFINTTK,  // Kyber Inverse
    OP_BFNTTD,   // Dilithium Forward
    OP_BFINTTD,  // Dilithium Inverse
    OP_MQMULF,   // Falcon Montgomery Multiplication
    OP_MQMULK,   // Kyber Standalone Mul (Mode 1)
    OP_MQMULD,     // Dilithium Standalone Mul (Mode 0)
    OP_KARATS_1,
    OP_KARATS_2,
    OP_KARATS_3,
    OP_GFMUL8,  // HQC GF(2^64) Multiplication
    OP_RED32,
    OP_CADDQ
} bfu_op_t;

// Emulate a 32-bit register pair (rd_lo, rd_hi)
typedef struct {
    uint32_t rd_lo; 
    uint32_t rd_hi;
} bfu_result_t;

bfu_result_t bfu_unit(uint32_t rs1, uint32_t rs2, int32_t zeta, bfu_op_t op);

#endif



