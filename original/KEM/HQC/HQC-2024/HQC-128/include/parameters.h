#ifndef HQC_PARAMETERS_H
#define HQC_PARAMETERS_H

/**
 * @file parameters.h
 * @brief Parameters of the HQC_KEM IND-CCA2 scheme
 */

#include "api.h"

#define CEIL_DIVIDE(a, b)  (((a)/(b)) + ((a) % (b) == 0 ? 0 : 1)) /*!< Divide a by b and ceil the result*/
#define BITMASK(a, size) (((uint64_t)1 << (a % size)) - 1) /*!< Create a mask*/

#define PARAM_N                               17669
#define PARAM_N1                              46
#define PARAM_N2                              384
#define PARAM_N1N2                            17664
#define PARAM_OMEGA                           66
#define PARAM_OMEGA_E                         75
#define PARAM_OMEGA_R                         75
#define PARAM_SECURITY                        128
#define PARAM_DFR_EXP                         128

#define SECRET_KEY_BYTES                      CRYPTO_SECRETKEYBYTES
#define PUBLIC_KEY_BYTES                      CRYPTO_PUBLICKEYBYTES
#define SHARED_SECRET_BYTES                   CRYPTO_BYTES
#define CIPHERTEXT_BYTES                      CRYPTO_CIPHERTEXTBYTES

#define UTILS_REJECTION_THRESHOLD             16767881
#define VEC_N_SIZE_BYTES                      CEIL_DIVIDE(PARAM_N, 8)
#define VEC_K_SIZE_BYTES                      PARAM_K
#define VEC_N1_SIZE_BYTES                     PARAM_N1
#define VEC_N1N2_SIZE_BYTES                   CEIL_DIVIDE(PARAM_N1N2, 8)

#define VEC_N_SIZE_64                         CEIL_DIVIDE(PARAM_N, 64)
#define VEC_K_SIZE_64                         CEIL_DIVIDE(PARAM_K, 8)
#define VEC_N1_SIZE_64                        CEIL_DIVIDE(PARAM_N1, 8)
#define VEC_N1N2_SIZE_64                      CEIL_DIVIDE(PARAM_N1N2, 64)

#define PARAM_DELTA                           15
#define PARAM_M                               8
#define PARAM_GF_POLY                         0x11D
#define PARAM_GF_POLY_WT                      5
#define PARAM_GF_POLY_M2                        4
#define PARAM_GF_MUL_ORDER                    255
#define PARAM_K                               16
#define PARAM_G                               31
#define PARAM_FFT                             4
#define RS_POLY_COEFS 89,69,153,116,176,117,111,75,73,233,242,233,65,210,21,139,103,173,67,118,105,210,174,110,74,69,228,82,255,181,1

#define RED_MASK                              BITMASK(PARAM_N, 64)
#define SHAKE256_512_BYTES                    64
#define SEED_BYTES                            40

#endif