# PQC Algorithms Profiling Analysis

This document presents a comprehensive analysis of function call profiling for six Post-Quantum Cryptography (PQC) algorithms. Each table shows the most called functions across KeyGen, Encaps/Sign, and Decaps/Verify operations.

---

## 1. ML-KEM-512


| Function | KeyGen Calls | Encaps Calls | Decaps Calls | **Total Calls** |
|----------|-------------|--------------|--------------|-----------------|
| `montgomery_reduce` | 6,656 | 9,088 | 13,312 | **29,056** |
| `fqmul` | 6,144 | 9,088 | 13,312 | **28,544** |
| `barrett_reduce` | 2,048 | 4,736 | 6,656 | **13,440** |
| `basemul` | 512 | 768 | 1,024 | **2,304** |
| `cmov_int16` | - | 256 | 256 | **512** |
| `store64` | 332 | 298 | 294 | **924** |
| `load24_littleendian` | 256 | 128 | 128 | **512** |
| `load64` | 85 | 85 | - | **170** |
| `load32_littleendian` | - | 96 | 96 | **192** |
| `KeccakF1600_StatePermute` | 27 | 26 | 26 | **79** |
| `poly_reduce` | 8 | 8 | 12 | **28** |
| `poly_add` | 4 | 7 | 8 | **19** |
| `poly_basemul_montgomery` | 4 | 6 | 8 | **18** |
| `ntt` | 4 | 2 | 4 | **10** |
| `poly_ntt` | 4 | 2 | 4 | **10** |
| `keccak_absorb_once` | 10 | 11 | 10 | **31** |
| `keccak_squeezeblocks` | 8 | 9 | 9 | **26** |
| `invntt` | - | 3 | 4 | **7** |
| `poly_invntt_tomont` | - | 3 | 4 | **7** |
| `polyvec_basemul_acc_montgomery` | 2 | 3 | 4 | **9** |
| `rej_uniform` | 4 | 4 | 4 | **12** |
| `gen_matrix` | 1 | 1 | 1 | **3** |
| `polyvec_ntt` | 2 | 1 | 2 | **5** |

---

## 2. ML-DSA-2
 

| Function | KeyGen Calls | Sign Calls | Verify Calls | **Total Calls** |
|----------|-------------|------------|--------------|-----------------|
| `montgomery_reduce` | 13,312 | 65,536 | 19,456 | **98,304** |
| `reduce32` | 1,024 | 6,144 | 1,024 | **8,192** |
| `store64` | 1,901 | 2,394 | 1,697 | **5,992** |
| `caddq` | 1,024 | 2,048 | 1,024 | **4,096** |
| `decompose` | - | 2,048 | 1,024 | **3,072** |
| `power2round` | 1,024 | - | - | **1,024** |
| `make_hint` | - | 1,024 | - | **1,024** |
| `use_hint` | - | - | 1,024 | **1,024** |
| `KeccakF1600_StatePermute` | 104 | 138 | 99 | **341** |
| `load64` | 153 | - | 153 | **306** |
| `keccak_absorb` | 48 | 60 | 38 | **146** |
| `poly_pointwise_montgomery` | 16 | 48 | 20 | **84** |
| `poly_add` | 16 | 36 | 12 | **64** |
| `shake128_absorb` | 32 | 32 | 32 | **96** |
| `keccak_squeezeblocks` | 31 | 26 | 18 | **75** |
| `keccak_finalize` | 24 | 30 | 19 | **73** |
| `keccak_init` | 24 | 30 | 19 | **73** |
| `rej_uniform` | 16 | 16 | 16 | **48** |
| `ntt` | 4 | 22 | 9 | **35** |
| `poly_ntt` | 4 | 22 | 9 | **35** |
| `invntt_tomont` | 4 | 24 | 4 | **32** |
| `poly_invntt_tomont` | 4 | 24 | 4 | **32** |
| `poly_reduce` | 4 | 24 | 4 | **32** |
| `polyvecl_pointwise_acc_montgomery` | 4 | 8 | 4 | **16** |
| `poly_uniform` | 16 | 16 | 16 | **48** |
| `poly_caddq` | 4 | 8 | 4 | **16** |
| `shake256_absorb` | 16 | 28 | 6 | **50** |

---

## 3. Falcon-512


| Function | KeyGen Calls | Sign Calls | Verify Calls | **Total Calls** |
|----------|-------------|------------|--------------|-----------------|
| `modp_montymul` | 975,905 | - | - | **975,905** |
| `FPR` | 433,929 | 297,190 | - | **731,119** |
| `modp_add` | 683,552 | - | - | **683,552** |
| `FPR_NORM64` | 227,531 | 160,500 | - | **388,031** |
| `modp_sub` | 367,276 | - | - | **367,276** |
| `fpr_add` | 179,099 | 148,474 | - | **327,573** |
| `fpr_mul` | 205,631 | 133,874 | - | **339,505** |
| `fpr_scaled` | 48,432 | 12,026 | - | **60,458** |
| `zint_mod_small_unsigned` | 49,832 | - | - | **49,832** |
| `mq_montymul` | 17,664 | 20,992 | 8,448 | **47,104** |
| `modp_set` | 30,014 | - | - | **30,014** |
| `zint_mod_small_signed` | 28,222 | - | - | **28,222** |
| `mq_sub` | 6,912 | 9,216 | 7,424 | **23,552** |
| `mq_add` | 6,912 | 9,216 | 6,912 | **23,040** |
| `zint_add_mul_small` | 21,610 | - | - | **21,610** |
| `zint_norm_zero` | 21,052 | - | - | **21,052** |
| `zint_sub` | 21,052 | - | - | **21,052** |
| `zint_add_scaled_mul_small` | 18,104 | - | - | **18,104** |
| `mq_montysqr` | 5,632 | 5,632 | - | **11,264** |
| `i_shake256_extract` | 8,216 | 557 | 556 | **9,329** |
| `get_rng_u64` | 8,216 | - | - | **8,216** |
| `modp_R` | 6,740 | - | - | **6,740** |
| `fpr_div` | 767 | 2,304 | - | **3,071** |
| `poly_split_fft` | - | 2,044 | - | **2,044** |
| `mkgauss` | 2,054 | - | - | **2,054** |
| `modp_norm` | 2,048 | - | - | **2,048** |
| `modp_NTT2_ext` | 1,957 | - | - | **1,957** |
| `BerExp` | - | 1,790 | - | **1,790** |
| `gaussian0_sampler` | - | 1,790 | - | **1,790** |
| `mq_conv_small` | 1,024 | 1,536 | - | **2,560** |
| `sampler` | - | 1,024 | - | **1,024** |
| `ffSampling_fft_dyntree` | - | 1,023 | - | **1,023** |
| `poly_merge_fft` | - | 1,022 | - | **1,022** |
| `process_block` | 484 | 10 | 9 | **503** |
| `FFT` | 597 | 9 | - | **606** |
| `iFFT` | 293 | 2 | - | **295** |
| `poly_mul_fft` | 576 | 517 | - | **1,093** |
| `poly_add` | 287 | 516 | - | **803** |
| `poly_LDL_fft` | - | 511 | - | **511** |
| `poly_sub` | - | 511 | - | **511** |
| `fpr_sqrt` | - | 512 | - | **512** |
| `mq_div_12289` | 512 | 512 | - | **1,024** |

---

## 4. HQC-1


| Function | KeyGen Calls | Encaps Calls | Decaps Calls | **Total Calls** |
|----------|-------------|--------------|--------------|-----------------|
| `compare_u32` | - | 8,325 | 8,325 | **16,650** |
| `gf_reduce` | - | 496 | 5,093 | **5,589** |
| `gf_carryless_mul` | - | 496 | 4,502 | **4,998** |
| `gf_mul` | - | 496 | 4,502 | **4,998** |
| `karatsuba_mul` | 364 | 728 | 1,092 | **2,184** |
| `schoolbook_mul` | 243 | 486 | 729 | **1,458** |
| `gf_square` | - | - | 591 | **591** |
| `barrett_reduce` | 133 | - | 66 | **199** |
| `KeccakF1600_StatePermute` | 25 | 42 | 77 | **144** |
| `gf_inverse` | - | - | 92 | **92** |
| `store64` | 9 | 26 | 43 | **78** |
| `encode` | - | 46 | 46 | **92** |
| `expand_and_sum` | - | - | 46 | **46** |
| `find_peaks` | - | - | 46 | **46** |
| `hadamard` | - | - | 46 | **46** |
| `keccak_inc_absorb` | 8 | 10 | 18 | **36** |
| `keccak_inc_squeeze` | 11 | 10 | 10 | **31** |
| `shake256_inc_squeeze` | 11 | 10 | 10 | **31** |
| `fft_rec` | - | - | 14 | **14** |
| `xof_get_bytes` | 6 | 4 | 5 | **15** |
| `keccak_inc_finalize` | 4 | 4 | 6 | **14** |
| `keccak_inc_init` | 4 | 4 | 6 | **14** |
| `compute_subset_sums` | - | - | 8 | **8** |
| `sha3_256_inc_absorb` | - | 2 | 8 | **10** |
| `sha3_512_inc_absorb` | 2 | 4 | 4 | **10** |
| `radix` | - | - | 7 | **7** |
| `vect_add` | 1 | 3 | 4 | **8** |
| `vect_sample_fixed_weight2` | - | 3 | 3 | **6** |
| `vect_write_support_to_vector` | 2 | 3 | 4 | **9** |
| `vect_mul` | 1 | 2 | 3 | **6** |
| `vect_compare` | - | - | 3 | **3** |
| `reduce` | 1 | 2 | 3 | **6** |
| `xof_init` | 3 | 2 | 3 | **8** |
| `shake256_inc_absorb` | 6 | 4 | 6 | **16** |
| `shake256_inc_finalize` | 3 | 2 | 3 | **8** |
| `shake256_inc_init` | 3 | 2 | 3 | **8** |

---

## 5. SPHINCS+-128f-robust

| Function | KeyGen Calls | Sign Calls | Verify Calls | **Total Calls** |
|----------|-------------|------------|--------------|-----------------|
| `load64` | 148,614 | 3,448,569 | 210,834 | **3,808,017** |
| `store64` | 148,614 | 3,448,552 | 210,817 | **3,807,983** |
| `keccak_squeezeblocks` | 17,420 | 404,298 | 24,620 | **446,338** |
| `shake256_squeezeblocks` | 17,420 | 404,298 | 24,620 | **446,338** |
| `keccak_absorb` | 8,710 | 202,149 | 12,310 | **223,169** |
| `shake256` | 8,710 | 202,149 | 12,310 | **223,169** |
| `shake256_absorb` | 8,710 | 202,149 | 12,310 | **223,169** |
| `KeccakF1600_StatePermute` | 8,774 | 203,566 | 12,494 | **224,834** |
| `SPX_set_hash_addr` | 4,480 | 98,560 | 5,835 | **108,875** |
| `SPX_thash` | 4,215 | 96,922 | 6,155 | **107,292** |
| `SPX_set_type` | 562 | 16,657 | 5 | **17,224** |
| `SPX_prf_addr` | 280 | 8,305 | - | **8,585** |
| `SPX_set_chain_addr` | 280 | 6,160 | 770 | **7,210** |
| `SPX_u32_to_bytes` | 23 | 4,753 | 320 | **5,096** |
| `SPX_set_tree_index` | 7 | 4,378 | 297 | **4,682** |
| `SPX_set_tree_height` | 7 | 2,266 | 297 | **2,570** |
| `fors_gen_sk` | - | 2,145 | - | **2,145** |
| `fors_gen_leafx1` | - | 2,112 | - | **2,112** |
| `fors_sk_to_leaf` | - | 2,112 | 33 | **2,145** |
| `gen_chain` | - | - | 770 | **770** |
| `SPX_set_keypair_addr` | 16 | 375 | 23 | **414** |
| `SPX_wots_gen_leafx1` | 8 | 176 | - | **184** |
| `SPX_ull_to_bytes` | 1 | 45 | 45 | **91** |
| `SPX_copy_subtree_addr` | 2 | 66 | 22 | **90** |
| `SPX_treehashx1` | 1 | 55 | - | **56** |
| `SPX_compute_root` | - | - | 55 | **55** |
| `base_w` | 2 | 44 | 44 | **90** |
| `SPX_chain_lengths` | 1 | 22 | 22 | **45** |
| `wots_checksum` | 1 | 22 | 22 | **45** |
| `SPX_merkle_sign` | 1 | 22 | - | **23** |
| `SPX_set_layer_addr` | 2 | 22 | 22 | **46** |
| `SPX_set_tree_addr` | - | 23 | 23 | **46** |
| `SPX_wots_pk_from_sig` | - | - | 22 | **22** |
| `SPX_copy_keypair_addr` | - | 3 | 24 | **27** |

---

## 6. SPHINCS+-128f-simple

| Function | KeyGen Calls | Sign Calls | Verify Calls | **Total Calls** |
|----------|-------------|------------|--------------|-----------------|
| `load64` | 76,959 | 1,800,895 | 105,689 | **1,983,543** |
| `store64` | 76,415 | 1,788,859 | 104,125 | **1,969,399** |
| `keccak_squeezeblocks` | 8,990 | 210,454 | 12,250 | **231,694** |
| `shake256_squeezeblocks` | 8,990 | 210,454 | 12,250 | **231,694** |
| `keccak_absorb` | 4,495 | 105,227 | 6,125 | **115,847** |
| `shake256` | 4,495 | 105,227 | 6,125 | **115,847** |
| `shake256_absorb` | 4,495 | 105,227 | 6,125 | **115,847** |
| `KeccakF1600_StatePermute` | 4,527 | 105,937 | 6,218 | **116,682** |
| `SPX_set_hash_addr` | 4,480 | 98,560 | 5,805 | **108,845** |
| `SPX_set_type` | 562 | 16,657 | 5 | **17,224** |
| `SPX_prf_addr` | 280 | 8,305 | - | **8,585** |
| `SPX_set_chain_addr` | 280 | 6,160 | 770 | **7,210** |
| `SPX_u32_to_bytes` | 23 | 4,753 | 320 | **5,096** |
| `SPX_set_tree_index` | 7 | 4,378 | 297 | **4,682** |
| `SPX_set_tree_height` | 7 | 2,266 | 297 | **2,570** |
| `fors_gen_sk` | - | 2,145 | - | **2,145** |
| `fors_gen_leafx1` | - | 2,112 | - | **2,112** |
| `fors_sk_to_leaf` | - | 2,112 | 33 | **2,145** |
| `gen_chain` | - | - | 770 | **770** |
| `SPX_set_keypair_addr` | 16 | 375 | 23 | **414** |
| `SPX_wots_gen_leafx1` | 8 | 176 | - | **184** |
| `SPX_ull_to_bytes` | 1 | 45 | 45 | **91** |
| `SPX_copy_subtree_addr` | 2 | 66 | 22 | **90** |
| `base_w` | 2 | 44 | 44 | **90** |
| `SPX_treehashx1` | 1 | 55 | - | **56** |
| `SPX_compute_root` | - | - | 55 | **55** |
| `SPX_chain_lengths` | 1 | 22 | 22 | **45** |
| `wots_checksum` | 1 | 22 | 22 | **45** |
| `SPX_merkle_sign` | 1 | 22 | - | **23** |
| `SPX_set_layer_addr` | 2 | 22 | 22 | **46** |
| `SPX_set_tree_addr` | - | 23 | 23 | **46** |
| `SPX_wots_pk_from_sig` | - | - | 22 | **22** |
| `SPX_copy_keypair_addr` | - | 3 | 24 | **27** |

---

## Summary: Key Bottlenecks Comparison

| Algorithm | Total Keccak Calls | Top Bottleneck (by calls) | Other Key Operations |
|-----------|-------------------|---------------------------|----------------------|
| **ML-KEM-512** | 79 | `montgomery_reduce` (29,056) | `barrett_reduce`, `basemul`, `ntt/invntt` |
| **ML-DSA-2** | 341 | `montgomery_reduce` (98,304) | `reduce32`, `decompose`, `ntt/invntt` |
| **Falcon-512** | 503 | `modp_montymul` (975,905) | `FPR` (731K), `FPR_NORM64` (388K), FFT ops |
| **HQC-1** | 144 | `compare_u32` (16,650) | `gf_reduce`, `karatsuba_mul`, GF arithmetic |
| **SPHINCS+-128f-robust** | 224,834 | `load64/store64` (3.8M each) | `shake256`, `thash`, `gen_chain` |
| **SPHINCS+-128f-simple** | 116,682 | `load64/store64` (1.9M each) | `shake256`, hash address ops |

---

**Note:** 
- All counts represent function call frequency, not cycle counts.
- Percentages from the original profiling data are relative to Total Protocol Cost (TPC) = KeyGen + Encaps/Sign + Decaps/Verify.
- Operations are not mutually exclusive to highlight orchestration overheads.
- For lattice-based schemes (ML-KEM, ML-DSA, Falcon), Montgomery multiplication dominates.
- For hash-based schemes (SPHINCS+), Keccak/SHA3 operations are the primary bottleneck.
- For code-based schemes (HQC), Galois field arithmetic and comparison operations dominate.
