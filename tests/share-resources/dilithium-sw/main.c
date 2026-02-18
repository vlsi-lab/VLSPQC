#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "dilithium_params.h"
#include "fips202.h"

// Assuming CRHEEPTO MCU headers are in your include path
#include "core_v_mini_mcu.h"
#include "csr.h"

int main() {
    uint8_t seed_rho[SEEDBYTES];
    uint8_t seed_crh[CRHBYTES];
    poly p;
    uint32_t start, end;

    // Initialize seeds
    for(int i=0; i<SEEDBYTES; i++) seed_rho[i] = i;
    for(int i=0; i<CRHBYTES; i++) seed_crh[i] = i;

    printf("\n--- Dilithium SW Model Verification ---\n");

    // 1. Test poly_uniform
    CSR_READ(CSR_REG_MCYCLE, &start);
    poly_uniform(&p, seed_rho, 0);
    CSR_READ(CSR_REG_MCYCLE, &end);
    printf("poly_uniform: %u cycles\n", (unsigned int)(end - start));
    printf("  Coeffs [0-3]: %d, %d, %d, %d\n", p.coeffs[0], p.coeffs[1], p.coeffs[2], p.coeffs[3]);

    // 2. Test poly_uniform_eta
    CSR_READ(CSR_REG_MCYCLE, &start);
    poly_uniform_eta(&p, seed_crh, 0);
    CSR_READ(CSR_REG_MCYCLE, &end);
    printf("poly_uniform_eta: %u cycles\n", (unsigned int)(end - start));
    printf("  Coeffs [0-3]: %d, %d, %d, %d\n", p.coeffs[0], p.coeffs[1], p.coeffs[2], p.coeffs[3]);

    // 3. Test poly_uniform_gamma1
    CSR_READ(CSR_REG_MCYCLE, &start);
    poly_uniform_gamma1(&p, seed_crh, 0);
    CSR_READ(CSR_REG_MCYCLE, &end);
    printf("poly_uniform_gamma1: %u cycles\n", (unsigned int)(end - start));

    return 0;
}