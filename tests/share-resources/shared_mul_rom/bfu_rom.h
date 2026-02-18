#ifndef BFU_ROM_H
#define BFU_ROM_H

#include <stdint.h>

typedef enum {
    OP_BFNTTK, OP_BFINTTK,
    OP_BFNTTD, OP_BFINTTD
} bfu_op_t;

typedef struct {
    uint32_t rd_lo; 
    uint32_t rd_hi;
} bfu_result_t;

// Note: tw_idx is the index within the specific algorithm's table
bfu_result_t bfu_unit_rom(uint32_t rs1, uint32_t rs2, uint32_t tw_idx, bfu_op_t op);

#endif