
This schematic represents the Unified Polynomial Arithmetic Unit. It maps your software bfu_unit and unified_mul_32x32 into a hardware datapath.
The core philosophy is the Unified Multiplier Tree, which uses a "Gated Carry" control signal to toggle between integer and binary (GF(2)) arithmetic.


rs1 [32]            rs2 [32]            zetas [32]
          │                   │                   │
  ┌───────▼───────────────────┼───────────────────┘
  │   Input Selection Mux     │ (Instruction Dependent)
  └───────┬───────────┬───────┘
          │ op_a      │ op_b
          ▼           ▼
┌───────────────────────────────────────────────┐
│        UNIFIED 32x32 MULTIPLIER TREE          │◄─── Mode Select
│  (Wallace/Dadda Tree with Carry-Gating Logic) │     (Int vs GF2)
└──────────────────────┬────────────────────────┘
                       │ prod [64]
      _________________▼________________________
     │        MUTUALLY EXCLUSIVE PATHS          │ (MUXed)
     │   ____________________________________   │
     │  │                                    │  │
     ▼  ▼                                    ▼  ▼
┌────────────┐  ┌────────────┐  ┌────────────┐  ┌────────────┐
│   ML-KEM   │  │   ML-DSA   │  │   FALCON   │  │    HQC     │
│ Montgomery │  │ Montgomery │  │ Reduction  │  │   Bypass   │
│  (16-bit)  │  │  (32-bit)  │  │  (16-bit)  │  │ (Raw Bits) │
└─────┬──────┘  └─────┬──────┘  └─────┬──────┘  └─────┬──────┘
      │               │               │               │
      └───────────────┴───────┬───────┴───────────────┘
                              │
               ┌──────────────┴──────────────┐
               │      Destination Switch     │
               └──────┬───────────────┬──────┘
                      │               │
      (Kyber/DSA/Falcon Results)      │ (HQC Results Only)
                      ▼               ▼
┌────────────────────────────┐  ┌────────────────────────────┐
│      BUTTERFLY ALU         │  │   KARATSUBA STATE REGS     │
│                            │  │                            │
│  [±] Add/Sub Unit          │  │  [R] a1_reg, b1_reg        │
│  [B] Barrett Reduction     │  │  [R] reg_A, reg_B          │
└──────────┬───────────┬─────┘  └─────┬──────────────┬───────┘
           │           └──────────────┤              │
           ▼                          ▼              ▼
      ┌──────────┐               ┌───────────────────────────┐
      │  rd_lo   │               │   rd_hi (or res2 in SV)   │
      └──────────┘               └───────────────────────────┘

Instruction-Specific Signal Paths

Below is how the control unit configures the datapath for your different functions:
1. ML-KEM / ML-DSA (Forward NTT)
- Multiplier Mode: Carry_Disable = 0 (Integer).
- Path: Multiplier computes zeta * rs2 (or half-registers for Kyber).
- Reduction: Selected Montgomery block (16 or 32-bit).
- Butterfly ALU: Performs rs1 + mul_res and rs1 - mul_res.
- Output: rd_lo = Sum, rd_hi = Difference.

2. Falcon (mq_montymul)
- Multiplier Mode: Carry_Disable = 0 (Integer).
- Path: Multiplier computes rs1 * rs2.
- Reduction: Falcon logic (z + w >> 16 with conditional subtract).
- Butterfly ALU: Bypassed.
- Output: rd_lo = mul_res, rd_hi = 0.

3. HQC Karatsuba (karats_1/2/3)
- Multiplier Mode: Carry_Disable = 1 (GF(2) Mode).
- Path: Multiplier computes Carry-less product.
- Reduction: Bypassed (Raw 64-bit product).
- Butterfly ALU (Stateful):
    - Step 1: Store prod[31:0] ^ prod[63:32] into reg_A.
    - Step 2: Store rs1, rs2 into a1_reg, b1_reg.
    - Step 3: Multiplier input is (rs1 ^ a1_reg) * (rs2 ^ b1_reg).
- Output: Reconstructed 64-bit segments across instructions.

4. HQC GFMUL8
- Multiplier Mode: Carry_Disable = 1 (GF(2) Mode).
- Path: Multiplier computes (rs1 & 0xFF) * (rs2 & 0xFF).
- Reduction: Bypassed.
- Output: rd_lo = prod[15:0].


Key Architectural Elements in the Schematic:
- Unified Multiplier Tree:
    - The heart of the module. Unlike standard designs that use separate multipliers, this uses one tree where the Carry-Chain - is gated by the instruction opcode.
    - Integer Mode: Carries propagate (used by Kyber, Dilithium, Falcon).
    - GF(2) Mode: Carries are forced to zero, turning the unit into a 32x32 Carry-less Multiplier (used by HQC).

- Shared Reduction Datapath:
    - The 64-bit product is routed through a bank of modular reduction logic.
    - Falcon/Kyber Path: Optimized for 16-bit shift-and-add reduction.
    - Dilithium Path: Optimized for 32-bit Montgomery reduction.
    - HQC Path: Bypasses reduction to keep the raw polynomial product for Karatsuba recombination.

- Stateful Karatsuba Registers:
    - As seen in your SystemVerilog (karats.sv), the BFU includes internal Flip-Flops (a1_reg, reg_A, reg_B).
    - This allows the HQC Karatsuba to be performed over 3 clock cycles/instructions without needing to write intermediate - 128-bit results back to the main CPU register file.

- Butterfly Unit (BFU) Logic:
    - The Add/Sub unit surrounding the multiplier handles the NTT/INTT butterflies.
    - In HQC mode, these same adders act as XOR gates for the Karatsuba final recombination step.
    - This design maximizes Hardware Reuse, allowing a single ALU to support four different Post-Quantum Cryptography - algorithms with minimal area overhead.