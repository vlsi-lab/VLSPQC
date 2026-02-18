#include "bfu.h"
#include "falcon.h"
#include <stdio.h>


#define DEBUG_MODULE 0

#if DEBUG_MODULE
    #define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
#endif


/**
 * UNIFIED MULTIPLIER TREE (Hardware Emulation)
 * Models a 32x32 multiplier where carries can be gated.

static int64_t unified_mul_32x32(int32_t a, int32_t b, int carryless) {
    if (!carryless) {
        // Integer Mode: SIGNED multiplication
        // In RTL, the multiplier tree handles sign-extension bits.
        return (int64_t)a * (int64_t)b;
    } else {
        // Carry-less Mode: Bits are treated as polynomial coefficients in GF(2)
        // We use uint32 to avoid sign-extension interference during XOR shifts.
        uint32_t ua = (uint32_t)a;
        uint32_t ub = (uint32_t)b;
        uint64_t res = 0;
        for (int i = 0; i < 32; i++) {
            if ((ua >> i) & 1) {
                res ^= ((uint64_t)ub << i);
            }
        }
        return (int64_t)res;
    }
}
 */
/**
 * UNIFIED MULTIPLIER TREE (Hardware-Centric Model)
 * This models a single array of logic that can perform 
 * either Integer (Dilithium/Kyber/Falcon) or GF(2) (HQC) multiplication.
 */
static int64_t unified_mul_32x32(int32_t a, int32_t b, int carryless) {
    int64_t accumulator = 0;
    uint32_t op_a = (uint32_t)a;

    // In Hardware, this is ONE tree of adders. 
    // The 'carryless' signal gates the carry-propagation wires.
    for (int i = 0; i < 32; i++) {
        // 1. Partial Product Generation
        // If bit i of 'a' is 1, the partial product is 'b' shifted by i.
        int64_t partial_product = 0;
        if ((op_a >> i) & 1) {
            // HARDWARE NOTE: In integer mode, 'b' is sign-extended to 64 bits 
            // before the shift. In carryless mode, it is zero-extended.
            if (carryless) {
                partial_product = (int64_t)((uint64_t)(uint32_t)b << i);
            } else {
                partial_product = ((int64_t)b << i);
            }
        }

        // 2. The Summation Tree (Shared Resource)
        if (carryless) {
            // GF(2) Logic: Summation is XOR. No carries exist.
            accumulator ^= partial_product;
        } else {
            // Integer Logic: Standard 2's complement multiplication.
            // Bit 31 of a signed 32-bit integer has a negative weight (-2^31).
            if (i < 31) {
                accumulator += partial_product;
            } else {
                // MSB weight is negative: subtract instead of add.
                accumulator -= partial_product;
            }
        }
    }
    DEBUG_PRINT("    [MUL] Input A: %d, B: %d, Carryless: %d -> Out: %lld\n", a, b, carryless, (long long)accumulator);
    
    return accumulator;
}

static int32_t barrett_hardware_emulation(int32_t a) {
    return kyber_barrett_reduce(a);
}

/**
 * SHARED MODULAR REDUCTION UNIT
 */
static int32_t shared_multiplication_logic(int32_t a, int32_t b, int mode) {
    // Mode 4 is HQC (carry-less). Others are integer.
    int carryless = (mode == 4);
    int32_t result = 0;
    int64_t prod = unified_mul_32x32(a, b, carryless);

    //DEBUG_PRINT("  [SH_MUL] Mode %d with Ops: %d, %d -> %lld\n", mode, a, b, (long long)prod);


    if (mode == 1) { // Kyber (16-bit Montgomery)
        int32_t p32 = (int32_t)prod;
        int16_t p16 = (int16_t)p32;
        int16_t t = p16 * (int16_t)KEM_QINV;
        result = (int16_t)((p32 - (int32_t)t * (int32_t)KEM_Q) >> 16);
        //DEBUG_PRINT("   [KYBER_MONT] Prod: %d, p16: %d, t: %d -> Res: %d\n", p32, p16, t, result);
        return result;
    } 
    else if (mode == 0) { // Dilithium (32-bit Montgomery)
        // Crucial: 'prod' must be the correct signed 64-bit product
        int32_t t = (int32_t)prod * (int32_t)DSA_QINV;
        result = (int32_t)((prod - (int64_t)t * (int64_t)DSA_Q) >> 32);
        DEBUG_PRINT("   [DILITHIUM_MONT] Prod: %d, t: %d -> Res: %d\n", prod, t, result);
        return result;

    } 
    else if (mode == 2) { // Falcon (16-bit Falcon Reduction)
        uint32_t z = (uint32_t)prod;
        uint32_t w = ((z * (uint32_t)FALCON_Q0I) & 0xFFFF) * (uint32_t)FALCON_Q;
        uint32_t res = (z + w) >> 16;
        res -= FALCON_Q;
        res += FALCON_Q & -(res >> 31);
        return (int32_t)res;
    }
    else { // mode 4: HQC (Return raw product bits)
        // We return the bits. Note that Karatsuba Step 1/2/3 handle 
        // high/low 32-bit splitting separately in the BFU wrapper.
        return (int32_t)0; // Placeholder, logic handled in switch(op)
    }
}

bfu_result_t bfu_unit(uint32_t rs1, uint32_t rs2, int32_t zeta, bfu_op_t op) {
    bfu_result_t res = {0, 0};
    static uint32_t a1_reg = 0, b1_reg = 0;
    static uint32_t reg_A = 0, reg_B = 0;

    DEBUG_PRINT("[BFU] OP: %d, RS1: 0x%08X (%d), RS2: 0x%08X (%d), Zeta: %d\n", op, rs1, (int32_t)rs1, rs2, (int32_t)rs2, zeta);
    switch (op) {
        case OP_BFNTTK: { 
            // 32-bit input rs1 = [ b (16b) | a (16b) ]
            // One instruction = One Butterfly
            //int16_t a = (int16_t)(rs1 & 0xFFFF);
            //int16_t b = (int16_t)(rs1 >> 16);
            int16_t a = (int16_t)rs1;
            int16_t b = (int16_t)rs2;

            // One instantiation of multiplication logic
            int16_t t = (int16_t)shared_multiplication_logic(zeta, b, 1);
            
            int16_t res_a = a + t; // a' = a + zeta*b
            int16_t res_b = a - t; // b' = a - zeta*b

            // Pack both 16-bit results into the 32-bit rd_lo
            res.rd_lo = ((uint32_t)(uint16_t)res_b << 16) | (uint32_t)(uint16_t)res_a;
            res.rd_hi = 0; 
            DEBUG_PRINT("  [BFNTTK] a: %d, b: %d, t: %d -> res_a: %d, res_b: %d\n", a, b, t, res_a, res_b);
            break;
        }

        case OP_BFINTTK: { 
            // 32-bit input rs1 = [ b (16b) | a (16b) ]
            int16_t a = (int16_t)(rs1 & 0xFFFF);
            int16_t b = (int16_t)(rs1 >> 16);

            // One instantiation of Barrett and Multiplier
            int16_t res_a = (int16_t)barrett_hardware_emulation(a + b);
            DEBUG_PRINT("  [BARRETT_INTTK] a: %d, b: %d -> res_a: %d\n", a, b, res_a);

            int16_t res_b = (int16_t)shared_multiplication_logic(zeta, b - a, 1);
            DEBUG_PRINT("  [BFINTTK] zeta: %d, b - a: %d -> res_b: %d\n", zeta, b - a, res_b);

            res.rd_lo = ((uint32_t)(uint16_t)res_b << 16) | (uint32_t)(uint16_t)res_a;
            res.rd_hi = 0;
            break;
        }

        case OP_BFNTTD: { 
            // Dilithium still needs two inputs (rs1, rs2) for A and B
            int32_t t = shared_multiplication_logic(zeta, (int32_t)rs2, 0);
            res.rd_lo = (uint32_t)((int32_t)rs1 + t);
            res.rd_hi = (uint32_t)((int32_t)rs1 - t);
            //DEBUG_PRINT("  [BFNTTD] zeta: %d, rs2: %d, t: %d -> res_a: %d, res_b: %d\n", zeta, rs2, t, res.rd_lo, res.rd_hi);

            break;
        }

        case OP_BFINTTD: { 
            res.rd_lo = (uint32_t)((int32_t)rs1 + (int32_t)rs2);
            res.rd_hi = (uint32_t)shared_multiplication_logic(zeta, (int32_t)rs1 - (int32_t)rs2, 0);
            break;
        }
        
        case OP_MQMULF: { 
            res.rd_lo = (uint32_t)shared_multiplication_logic((int32_t)rs1, (int32_t)rs2, 2);
            break;
        }

        case OP_MQMULK: { // Kyber Standalone (16-bit)
            // Logic: rd = (rs1 * rs2 * R^-1) mod q
            res.rd_lo = (uint32_t)shared_multiplication_logic((int32_t)rs1, (int32_t)rs2, 1);
            break;
        }
        case OP_MQMULD: { // Dilithium Standalone (32-bit)
            // Logic: rd = (rs1 * rs2 * R^-1) mod q
            res.rd_lo = (uint32_t)shared_multiplication_logic((int32_t)rs1, (int32_t)rs2, 0);
            break;
        }

        case OP_KARATS_1: { 
            // Manual call to unified multiplier for HQC (Mode 4)
            uint64_t prod = (uint64_t)unified_mul_32x32((int32_t)rs1, (int32_t)rs2, 1);
            res.rd_lo = (uint32_t)prod;
            res.rd_hi = (uint32_t)(prod >> 32);
            reg_A = res.rd_lo ^ res.rd_hi; 
            reg_B = res.rd_hi;
            DEBUG_PRINT("  [KARATS_1] prod=0x%016llX, reg_A=0x%08X, reg_B=0x%08X\n", 
                        (unsigned long long)prod, reg_A, reg_B);
            break;
        }
        case OP_KARATS_2: { 
            uint64_t prod = (uint64_t)unified_mul_32x32((int32_t)rs1, (int32_t)rs2, 1);
            res.rd_lo = (uint32_t)prod;
            res.rd_hi = (uint32_t)(prod >> 32);
            a1_reg = rs1; b1_reg = rs2;
            uint32_t res1_1 = res.rd_lo ^ reg_A;
            uint32_t res1_2 = res.rd_lo ^ res.rd_hi ^ reg_B;
            DEBUG_PRINT("  [KARATS_2] prod=0x%016llX, res.rd_lo = 0x%016llX, res.rd_hi = 0x%016llX, before: reg_A=0x%08X, reg_B=0x%08X\n", 
                        (unsigned long long)prod, res.rd_lo, res.rd_hi, reg_A, reg_B);
            reg_A = res1_1;
            reg_B = res1_2;
            DEBUG_PRINT("  [KARATS_2] after: reg_A=0x%08X, reg_B=0x%08X, a1_reg=0x%08X, b1_reg=0x%08X\n", 
                        reg_A, reg_B, a1_reg, b1_reg);
            break;
        }
        case OP_KARATS_3: { 
            uint32_t a_xor = rs1 ^ a1_reg;
            uint32_t b_xor = rs2 ^ b1_reg;
            uint64_t prod = (uint64_t)unified_mul_32x32((int32_t)a_xor, (int32_t)b_xor, 1);
            DEBUG_PRINT("  [KARATS_3] a_xor=0x%08X, b_xor=0x%08X, prod=0x%016llX\n", 
                        a_xor, b_xor, (unsigned long long)prod);
            DEBUG_PRINT("  [KARATS_3] reg_A=0x%08X, reg_B=0x%08X\n", reg_A, reg_B);
            res.rd_lo = (uint32_t)prod ^ reg_A;
            res.rd_hi = (uint32_t)(prod >> 32) ^ reg_B;
            DEBUG_PRINT("  [KARATS_3] result: rd_lo=0x%08X, rd_hi=0x%08X\n", res.rd_lo, res.rd_hi);
            break;
        }
        
        case OP_GFMUL8: {
            // Hardware reuse: Pad 8-bit operands to 32-bit
            // Use unified multiplier in carry-less mode (1)
            uint64_t prod = (uint64_t)unified_mul_32x32((int32_t)(rs1 & 0xFF), 
                                                       (int32_t)(rs2 & 0xFF), 1);
            // Result is 16-bit, packed into rd_lo
            res.rd_lo = (uint32_t)(prod & 0xFFFF);
            res.rd_hi = 0;
            break;
        }

        case OP_RED32: {
            int32_t a = (int32_t)rs1;
            int32_t t = (a + (1 << 22)) >> 23;
            res.rd_lo = (uint32_t)(a - t * DSA_Q);
            res.rd_hi = 0;
            DEBUG_PRINT("  [OP_RED32] Input: %d -> Out: %d\n", a, (int32_t)res.rd_lo);
            break;
        }

        case OP_CADDQ: {
            int32_t a = (int32_t)rs1;
            // a >> 31 creates a mask of all 1s if negative, all 0s if positive
            res.rd_lo = (uint32_t)(a + ((a >> 31) & DSA_Q));
            res.rd_hi = 0;
            DEBUG_PRINT("  [OP_CADDQ] Input: %d -> Out: %d\n", a, (int32_t)res.rd_lo);
            break;
        }
    }
    return res;
}

