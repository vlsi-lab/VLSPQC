#ifndef FALCON_H
#define FALCON_H

#include <stdint.h>

#define FALCON_Q     12289
#define FALCON_QINV  53247 // Standard Montgomery inverse: Q * QINV mod 2^16 = 1
#define FALCON_Q0I   12287 // -Q^-1 mod 2^16 (used in your specific falcon snippet)

uint32_t mq_montymul_sw(uint32_t x, uint32_t y);

extern const uint32_t falcon_source1[100];
extern const uint32_t falcon_source0[100];
extern const uint32_t falcon_golden[100];

#endif