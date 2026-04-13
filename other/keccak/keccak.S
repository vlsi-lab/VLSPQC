# Keccak-1600 RISC-V 32-bit Optimized Assembly
# Targeting Custom Instructions via Opcode 0x6b

.section .text
.global Keccak32_S
.align 4

# --- CUSTOM INSTRUCTION MACROS ---

# ROL_HI: rd = High Word of (rs1||rs2) rotated by shamt
.macro ROL_HI rd, rs_low, rs_high, shamt
    .insn r4 0x6b, 0x01, 0x0, \rd, \rs_low, \rs_high, \shamt
.endm

# ROL_LO: rd = Low Word of (rs1||rs2) rotated by shamt
.macro ROL_LO rd, rs_low, rs_high, shamt
    .insn r4 0x6b, 0x01, 0x1, \rd, \rs_low, \rs_high, \shamt
.endm

# CHI: rd = rs1 ^ ((~rs2) & rs3)
.macro CHI_STEP rd, rs1, rs2, rs3
    .insn r4 0x6b, 0x01, 0x2, \rd, \rs1, \rs2, \rs3
.endm

.macro DEBUG_PRINT_DA reg_hi, reg_lo
    # Allocate 80 bytes (16-byte aligned)
    addi sp, sp, -80
    sw   ra, 76(sp)
    sw   s0, 72(sp)   # Save s0 just in case C uses it as a frame pointer
    sw   a0, 68(sp)   # Save your state pointer!
    sw   t0, 64(sp)
    sw   t1, 60(sp)
    sw   t2, 56(sp)
    sw   t3, 52(sp)
    sw   t4, 48(sp)
    sw   t5, 44(sp)
    sw   t6, 40(sp)
    # a1-a7 are scratch registers for C
    sw   a1, 36(sp)
    sw   a2, 32(sp)
    sw   a3, 28(sp)
    sw   a4, 24(sp)
    sw   a5, 20(sp)
    sw   a6, 16(sp)
    sw   a7, 12(sp)

    # Move values to print into a0, a1
    mv   a0, \reg_hi
    mv   a1, \reg_lo

    jal  print_da

    # Restore everything exactly
    lw   a7, 12(sp)
    lw   a6, 16(sp)
    lw   a5, 20(sp)
    lw   a4, 24(sp)
    lw   a3, 28(sp)
    lw   a2, 32(sp)
    lw   a1, 36(sp)
    lw   t6, 40(sp)
    lw   t5, 44(sp)
    lw   t4, 48(sp)
    lw   t3, 52(sp)
    lw   t2, 56(sp)
    lw   t1, 60(sp)
    lw   t0, 64(sp)
    lw   a0, 68(sp)   # Restore state pointer
    lw   s0, 72(sp)   # Restore s0
    lw   ra, 76(sp)
    addi sp, sp, 80
.endm



# --- THE FUNCTION -----------------------------------------------------------------
Keccak32_S:
    # a0: pointer to state[50]
    
    # 1. Prologue: Save all Callee-saved registers
    addi    sp, sp, -320        # Allocate stack space for saved regs + local state
    sw      ra,  320(sp)
    sw      s0,  312(sp)
    sw      s1,  308(sp)
    sw      s2,  304(sp)
    sw      s3,  300(sp)
    sw      s4,  296(sp)
    sw      s5,  292(sp)
    sw      s6,  288(sp)
    sw      s7,  284(sp)
    sw      s8,  280(sp)
    sw      s9,  276(sp)
    sw      s10, 272(sp)
    sw      s11, 268(sp)

    # 2. Main Loop Setup
    # We will use the caller's state pointer a0 throughout
    la      s0, KeccakF1600RoundConstants32   # Load address of constants
    li      s11, 0                            # Round index (0 to 23)

round_loop:
    # --- STEP 1: THETA (Calculate Column Parity) ---
    # s1=BCa0, s2=BCe0, s3=BCi0, s4=BCo0, s5=BCu0 (Low)
    # s6=BCa1, s7=BCe1, s8=BCi1, s9=BCo1, s10=BCu1 (High)
    
    # --- STEP 1: THETA (Calculate Column Parity) --------------------------------------------
    # --- Column a ---
    lw      s1, 0(a0)       # Aba0
    lw      s6, 4(a0)       # Aba1
    lw      t2, 40(a0)      # Aga0
    lw      t3, 44(a0)      # Aga1
    xor     s1, s1, t2
    xor     s6, s6, t3
    lw      t2, 80(a0)      # Aka0
    lw      t3, 84(a0)      # Aka1
    xor     s1, s1, t2
    xor     s6, s6, t3
    lw      t2, 120(a0)     # Ama0
    lw      t3, 124(a0)     # Ama1
    xor     s1, s1, t2
    xor     s6, s6, t3
    lw      t2, 160(a0)     # Asa0
    lw      t3, 164(a0)     # Asa1
    xor     s1, s1, t2
    xor     s6, s6, t3
    //*DEBUG_PRINT_DA s1, s6

    # --- Column e ---
    lw      s2, 8(a0)       # Abe0
    lw      s7, 12(a0)      # Abe1
    lw      t2, 48(a0)      # Age0
    lw      t3, 52(a0)      # Age1
    xor     s2, s2, t2
    xor     s7, s7, t3
    lw      t2, 88(a0)      # Ake0
    lw      t3, 92(a0)      # Ake1
    xor     s2, s2, t2
    xor     s7, s7, t3
    lw      t2, 128(a0)     # Ame0
    lw      t3, 132(a0)     # Ame1
    xor     s2, s2, t2
    xor     s7, s7, t3
    lw      t2, 168(a0)     # Ase0
    lw      t3, 172(a0)     # Ase1
    xor     s2, s2, t2
    xor     s7, s7, t3
    //*DEBUG_PRINT_DA s2, s7

    # --- Column i ---
    lw      s3, 16(a0)      # Abi0
    lw      s8, 20(a0)      # Abi1
    lw      t2, 56(a0)      # Agi0
    lw      t3, 60(a0)      # Agi1
    xor     s3, s3, t2
    xor     s8, s8, t3
    lw      t2, 96(a0)      # Aki0
    lw      t3, 100(a0)     # Aki1
    xor     s3, s3, t2
    xor     s8, s8, t3
    lw      t2, 136(a0)     # Ami0
    lw      t3, 140(a0)     # Ami1
    xor     s3, s3, t2
    xor     s8, s8, t3
    lw      t2, 176(a0)     # Asi0
    lw      t3, 180(a0)     # Asi1
    xor     s3, s3, t2
    xor     s8, s8, t3
    //*DEBUG_PRINT_DA s3, s8

    # --- Column o ---
    lw      s4, 24(a0)      # Abo0
    lw      s9, 28(a0)      # Abo1
    lw      t2, 64(a0)      # Ago0
    lw      t3, 68(a0)      # Ago1
    xor     s4, s4, t2
    xor     s9, s9, t3
    lw      t2, 104(a0)     # Ako0
    lw      t3, 108(a0)     # Ako1
    xor     s4, s4, t2
    xor     s9, s9, t3
    lw      t2, 144(a0)     # Amo0
    lw      t3, 148(a0)     # Amo1
    xor     s4, s4, t2
    xor     s9, s9, t3
    lw      t2, 184(a0)     # Aso0
    lw      t3, 188(a0)     # Aso1
    xor     s4, s4, t2
    xor     s9, s9, t3
    //*DEBUG_PRINT_DA s4, s9

    # --- Column u ---
    lw      s5, 32(a0)      # Abu0
    lw      s10, 36(a0)     # Abu1
    lw      t2, 72(a0)      # Agu0
    lw      t3, 76(a0)      # Agu1
    xor     s5, s5, t2
    xor     s10, s10, t3
    lw      t2, 112(a0)     # Aku0
    lw      t3, 116(a0)     # Aku1
    xor     s5, s5, t2
    xor     s10, s10, t3
    lw      t2, 152(a0)     # Amu0
    lw      t3, 156(a0)     # Amu1
    xor     s5, s5, t2
    xor     s10, s10, t3
    lw      t2, 192(a0)     # Asu0
    lw      t3, 196(a0)     # Asu1
    xor     s5, s5, t2
    xor     s10, s10, t3
    //*DEBUG_PRINT_DA s5, s10


# --- STEP 2: PREPARE D ---------------------------------------------------------
    # Constant for rotation
    li      t6, 1

    # Da = BCu ^ ROL(BCe, 1)
    # BCu is s5/s10, BCe is s2/s7
    ROL_HI  t2, s2, s7, t6
    ROL_LO  t3, s2, s7, t6
    xor     t2, t2, s10     # Da1
    xor     t3, t3, s5      # Da0
    sw      t2, 8(sp)
    sw      t3, 4(sp)
    //*DEBUG_PRINT_DA t3, t2
    
    # De = BCa ^ ROL(BCi, 1)
    # BCa is s1/s6, BCi is s3/s8
    ROL_HI  t2, s3, s8, t6
    ROL_LO  t3, s3, s8, t6
    xor     t2, t2, s6      # De1
    xor     t3, t3, s1      # De0
    sw      t2, 16(sp)
    sw      t3, 12(sp)
    //*DEBUG_PRINT_DA t3, t2

    # Di = BCe ^ ROL(BCo, 1)
    # BCe is s2/s7, BCo is s4/s9
    ROL_HI  t2, s4, s9, t6
    ROL_LO  t3, s4, s9, t6
    xor     t2, t2, s7      # Di1
    xor     t3, t3, s2      # Di0
    sw      t2, 24(sp)
    sw      t3, 20(sp)
    //*DEBUG_PRINT_DA t3, t2

    # Do = BCi ^ ROL(BCu, 1)
    # BCi is s3/s8, BCu is s5/s10
    ROL_HI  t2, s5, s10, t6
    ROL_LO  t3, s5, s10, t6
    xor     t2, t2, s8      # Do1
    xor     t3, t3, s3      # Do0
    sw      t2, 32(sp)
    sw      t3, 28(sp)
    //*DEBUG_PRINT_DA t3, t2

    # Du = BCo ^ ROL(BCa, 1)
    # BCo is s4/s9, BCa is s1/s6
    ROL_HI  t2, s1, s6, t6
    ROL_LO  t3, s1, s6, t6
    xor     t2, t2, s9      # Du1
    xor     t3, t3, s4      # Du0
    sw      t2, 40(sp)
    sw      t3, 36(sp)
    //*DEBUG_PRINT_DA t3, t2
    

# --- STEP 3: RHO/PI/CHI --------------------------------------------------------
    
    # Example for Aba (Lane 0)
    lw      t2, 0(a0)       # Load Aba0
    lw      t3, 4(a0)       # Load Aba1
    lw      t4, 4(sp)       # Load Da0
    lw      t5, 8(sp)       # Load Da1
    xor     t2, t2, t4      # BCa0 = Aba0 ^ Da0
    xor     t3, t3, t5      # BCa1 = Aba1 ^ Da1
    # Store new BCa
    sw      t2, 44(sp)
    sw      t3, 48(sp)
    //*DEBUG_PRINT_DA t2, t3

    lw      t2, 48(a0)       # Load Age0
    lw      t3, 52(a0)       # Load Age1
    lw      t4, 12(sp)       # Load De0
    lw      t5, 16(sp)       # Load De1
    xor     t2, t2, t4       # Age0 = Age0 ^ De0
    xor     t3, t3, t5       # Age1 = Age1 ^ De1
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 48(a0)      # Store back Age0
    sw      t3, 52(a0)      # Store back Age1


    # 1. Prepare BCe (from Age)
    li      t1, 44           # Shift amount
    ROL_HI  s7, t2, t3, t1   # s7 = BCe1
    ROL_LO  s2, t2, t3, t1   # s2 = BCe0

    # 2. Prepare BCi (from Aki)
    # User logic: Aki ^= Di;
    lw      t2, 96(a0)       # Load Aki0
    lw      t3, 100(a0)       # Load Aki1
    lw      t4, 20(sp)       # Load Di0
    lw      t5, 24(sp)       # Load Di1
    xor     t2, t2, t4       # t2 = Aki0 ^ Di0
    xor     t3, t3, t5       # t3 = Aki1 ^ Di1
    sw      t2, 96(a0)       # Store back Aki0
    sw      t3, 100(a0)       # Store back Aki1
    //*DEBUG_PRINT_DA t2, t3
    li      t1, 43           # Shift amount
    ROL_HI  s8, t2, t3, t1   # s8 = BCi1
    ROL_LO  s3, t2, t3, t1   # s3 = BCi0


    # 3. Prepare BCo (from Amo)
    # User logic: Amo ^= Do; BCo = ROL(Amo, 21)
    lw      t2, 144(a0)      # Load Amo0
    lw      t3, 148(a0)      # Load Amo1
    lw      t4, 28(sp)       # Load Do0
    lw      t5, 32(sp)       # Load Do1
    xor     t2, t2, t4       # t2 = Amo0 ^ Do0
    xor     t3, t3, t5       # t3 = Amo1 ^ Do1
    sw      t2, 144(a0)      # Store back Amo0
    sw      t3, 148(a0)      # Store back Amo1
    //*DEBUG_PRINT_DA t2, t3
    li      t1, 21           # Shift amount
    ROL_HI  s9, t2, t3, t1   # s9 = BCo1
    ROL_LO  s4, t2, t3, t1   # s4 = BCo0

    # 4. Prepare BCu (from Asu)
    # User logic: Asu ^= Du; BCu = ROL(Asu, 14)
    lw      t2, 192(a0)      # Load Asu0
    lw      t3, 196(a0)      # Load Asu1
    lw      t4, 36(sp)       # Load Du0
    lw      t5, 40(sp)       # Load Du1
    xor     t2, t2, t4       # t2 = Asu0 ^ Du0
    xor     t3, t3, t5       # t3 = Asu1 ^ Du1
    sw      t2, 192(a0)      # Store back Asu0
    sw      t3, 196(a0)      # Store back Asu1
    //*DEBUG_PRINT_DA t2, t3
    li      t1, 14           # Shift amount
    ROL_HI  s10, t2, t3, t1  # s10 = BCu1
    ROL_LO  s5, t2, t3, t1   # s5  = BCu0

# --- STEP 4: CHI & IOTA --------------------------------------------------------
    # Load BCa (saved earlier in your code at 44/48)
    lw      t0, 44(sp)       # t0 = BCa0
    lw      t1, 48(sp)       # t1 = BCa1

    # Eba0 = (BCa0 ^ ((~BCe0) & BCi0)) ^ RC_low
    # Eba1 = (BCa1 ^ ((~BCe1) & BCi1)) ^ RC_high
    # Note: Using your s11 (round index) and s0 (RC pointer)
    slli    t4, s11, 3       # round * 8
    add     t4, t4, s0
    lw      t5, 0(t4)        # RC_low (index*2)
    lw      t6, 4(t4)        # RC_high (index*2 + 1)
    //*DEBUG_PRINT_DA t5, t6 

    CHI_STEP a3, t0, s2, s3  # a3 = BCa0 ^ ((~BCe0) & BCi0)
    xor      s1, a3, t6      # s1 = Eba0 (Result stored in s1)
    CHI_STEP a4, t1, s7, s8  # a4 = BCa1 ^ ((~BCe1) & BCi1)
    xor      s6, a4, t5      # s6 = Eba1 (Result stored in s6)
    //*DEBUG_PRINT_DA s1, s6    # Print Eba0/1
    sw    s1, 228(sp)        # Store Eba0
    sw    s6, 232(sp)        # Store Eba1

    # Ebe0 = BCe0 ^ ((~BCi0) & BCo0)
    # Ebe1 = BCe1 ^ ((~BCi1) & BCo1)
    CHI_STEP t2, s2, s3, s4  # t2 = Ebe0
    CHI_STEP t3, s7, s8, s9  # t3 = Ebe1
    //*DEBUG_PRINT_DA t2, t3
    sw     t2, 236(sp)        # Store Ebe0
    sw     t3, 240(sp)        # Store Ebe1

    # Ebi0 = BCi0 ^ ((~BCo0) & BCu0)
    # Ebi1 = BCi1 ^ ((~BCo1) & BCu1)
    CHI_STEP t2, s3, s4, s5  # t2 = Ebi0
    CHI_STEP t3, s8, s9, s10 # t3 = Ebi1
    //*DEBUG_PRINT_DA t2, t3
    sw     t2, 244(sp)        # Store Ebi0
    sw     t3, 248(sp)        # Store Ebi1

    # Ebo0 = BCo0 ^ ((~BCu0) & BCa0)
    # Ebo1 = BCo1 ^ ((~BCu1) & BCa1)
    CHI_STEP t2, s4, s5, t0  # t2 = Ebo0
    CHI_STEP t3, s9, s10, t1 # t3 = Ebo1
    //*DEBUG_PRINT_DA t2, t3
    sw     t2, 252(sp)        # Store Ebo0
    sw     t3, 256(sp)        # Store Ebo1

    # Ebu0 = BCu0 ^ ((~BCa0) & BCe0)
    # Ebu1 = BCu1 ^ ((~BCa1) & BCe1)
    CHI_STEP t2, s5, t0, s2  # t2 = Ebu0
    CHI_STEP t3, s10, t1, s7 # t3 = Ebu1
    //*DEBUG_PRINT_DA t2, t3
    sw     t2, 260(sp)        # Store Ebu0
    sw     t3, 264(sp)        # Store Ebu1

# --- STEP 5: RHO / PI / CHI (Row 1 - G Row) --------------------------------------------------------

    lw      t2, 24(a0)       # Load Abo0
    lw      t3, 28(a0)       # Load Abo1
    lw      t4, 28(sp)       # Load Do0
    lw      t5, 32(sp)       # Load Do1
    xor     t2, t2, t4       # Abo0 = Abo0 ^ Do0
    xor     t3, t3, t5       # Abo1 = Abo1 ^ Do1
    sw      t2, 24(a0)      # Store back Abo0
    sw      t3, 28(a0)      # Store back Abo1
    //*DEBUG_PRINT_DA t2, t3
    li      t1, 28           # Shift amount
    ROL_HI  s6, t2, t3, t1   # s1 = BCa1
    ROL_LO  s1, t2, t3, t1   # s6 = BCa0 

    # 2. Prepare BCe (from Agu)
    # C-Logic: Agu ^= Du; BCe = ROL(Agu, 20)
    lw      t2, 72(a0)       # Agu0 (Row 1 Col 4: 40 + 32)
    lw      t3, 76(a0)       # Agu1
    lw      t4, 36(sp)       # Du0
    lw      t5, 40(sp)       # Du1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 72(a0)       # Store back Agu0
    sw      t3, 76(a0)       # Store back Agu1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 20           # Shift amount (Using t4 because t1 is BCa1)
    ROL_HI  s7, t2, t3, t4   # s7 = BCe1
    ROL_LO  s2, t2, t3, t4   # s2 = BCe0

    # 3. Prepare BCi (from Aka)
    # C-Logic: Aka ^= Da; BCi = ROL(Aka, 3)
    lw      t2, 80(a0)       # Aka0 (Row 2 Col 0: 80 + 0)
    lw      t3, 84(a0)       # Aka1
    lw      t4, 4(sp)        # Da0
    lw      t5, 8(sp)        # Da1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 80(a0)       # Store back Aka0
    sw      t3, 84(a0)       # Store back Aka1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 3            # Shift amount
    ROL_HI  s8, t2, t3, t4   # s8 = BCi1
    ROL_LO  s3, t2, t3, t4   # s3 = BCi0

    # 4. Prepare BCo (from Ame)
    # C-Logic: Ame ^= De; BCo = ROL(Ame, 45)
    lw      t2, 128(a0)      # Ame0 (Row 3 Col 1: 120 + 8)
    lw      t3, 132(a0)      # Ame1
    lw      t4, 12(sp)       # De0
    lw      t5, 16(sp)       # De1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 128(a0)      # Store back Ame0
    sw      t3, 132(a0)      # Store back Ame1  
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 45           # Shift amount
    ROL_HI  s9, t2, t3, t4   # s9 = BCo1
    ROL_LO  s4, t2, t3, t4   # s4 = BCo0

    # 5. Prepare BCu (from Asi)
    # C-Logic: Asi ^= Di; BCu = ROL(Asi, 61)
    lw      t2, 176(a0)      # Asi0 (Row 4 Col 2: 160 + 16)
    lw      t3, 180(a0)      # Asi1
    lw      t4, 20(sp)       # Di0
    lw      t5, 24(sp)       # Di1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 176(a0)      # Store back Asi0
    sw      t3, 180(a0)      # Store back Asi1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 61           # Shift amount
    ROL_HI  s10, t2, t3, t4  # s10 = BCu1
    ROL_LO  s5, t2, t3, t4   # s5 = BCu0

    # --- ROW 1 CHI (Ega, Ege, Egi, Ego, Egu) ---

    # Ega0 = BCa0 ^ ((~BCe0) & BCi0)
    # Ega1 = BCa1 ^ ((~BCe1) & BCi1)
    CHI_STEP t2, s1, s2, s3  # t2 = Ega0
    CHI_STEP t3, s6, s7, s8  # t3 = Ega1
    sw     t2, 188(sp)        # Store Ega0
    sw     t3, 192(sp)        # Store Ega1
    //*DEBUG_PRINT_DA t2, t3

    # Ege0 = BCe0 ^ ((~BCi0) & BCo0)
    # Ege1 = BCe1 ^ ((~BCi1) & BCo1)
    CHI_STEP t2, s2, s3, s4  # t2 = Ege0
    CHI_STEP t3, s7, s8, s9  # t3 = Ege1
    //*DEBUG_PRINT_DA t2, t3
    sw    t2, 196(sp)        # Store Ege0
    sw    t3, 200(sp)        # Store Ege1

    # Egi0 = BCi0 ^ ((~BCo0) & BCu0)
    # Egi1 = BCi1 ^ ((~BCo1) & BCu1)
    CHI_STEP t2, s3, s4, s5  # t2 = Egi0
    CHI_STEP t3, s8, s9, s10 # t3 = Egi1
    //*DEBUG_PRINT_DA t2, t3
    sw    t2, 204(sp)        # Store Egi0
    sw    t3, 208(sp)        # Store Egi1

    # Ego0 = BCo0 ^ ((~BCu0) & BCa0)
    # Ego1 = BCo1 ^ ((~BCu1) & BCa1)
    CHI_STEP t2, s4, s5, s1  # t2 = Ego0
    CHI_STEP t3, s9, s10, s6 # t3 = Ego1
    //*DEBUG_PRINT_DA t2, t3
    sw    t2, 212(sp)        # Store Ego0
    sw    t3, 216(sp)        # Store Ego1

    # Egu0 = BCu0 ^ ((~BCa0) & BCe0)
    # Egu1 = BCu1 ^ ((~BCa1) & BCe1)
    CHI_STEP t2, s5, s1, s2  # t2 = Egu0
    CHI_STEP t3, s10, s6, s7 # t3 = Egu1
    //*DEBUG_PRINT_DA t2, t3
    sw    t2, 220(sp)        # Store Egu0
    sw    t3, 224(sp)        # Store Egu1

# --- STEP 6: RHO / PI / CHI (Row 2 Mapping) --------------------------------------------------------

    # 1. Prepare BCa (from Abe)
    # C-Logic: Abe ^= De; BCa = ROL(Abe, 1)
    lw      t2, 8(a0)        # Abe0
    lw      t3, 12(a0)       # Abe1
    lw      t4, 12(sp)       # De0
    lw      t5, 16(sp)       # De1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw     t2, 8(a0)        # Store back Abe0
    sw     t3, 12(a0)       # Store back Abe1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 1            # Shift amount
    ROL_HI  t1, t2, t3, t4   # t1 = BCa1
    ROL_LO  t0, t2, t3, t4   # t0 = BCa0

    # 2. Prepare BCe (from Agi)
    # C-Logic: Agi ^= Di; BCe = ROL(Agi, 6)
    lw      t2, 56(a0)       # Agi0 (Row 1 Col 2: 40 + 16)
    lw      t3, 60(a0)       # Agi1
    lw      t4, 20(sp)       # Di0
    lw      t5, 24(sp)       # Di1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 56(a0)       # Store back Agi0
    sw      t3, 60(a0)       # Store back Agi1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 6            # Shift amount
    ROL_HI  s7, t2, t3, t4   # s7 = BCe1
    ROL_LO  s2, t2, t3, t4   # s2 = BCe0

    # 3. Prepare BCi (from Ako)
    # C-Logic: Ako ^= Do; BCi = ROL(Ako, 25)
    lw      t2, 104(a0)      # Ako0 (Row 2 Col 3: 80 + 24)
    lw      t3, 108(a0)      # Ako1
    lw      t4, 28(sp)       # Do0
    lw      t5, 32(sp)       # Do1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 104(a0)      # Store back Ako0
    sw      t3, 108(a0)      # Store back Ako1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 25           # Shift amount
    ROL_HI  s8, t2, t3, t4   # s8 = BCi1
    ROL_LO  s3, t2, t3, t4   # s3 = BCi0

    # 4. Prepare BCo (from Amu)
    # C-Logic: Amu ^= Du; BCo = ROL(Amu, 8)
    lw      t2, 152(a0)      # Amu0 (Row 3 Col 4: 120 + 32)
    lw      t3, 156(a0)      # Amu1
    lw      t4, 36(sp)       # Du0
    lw      t5, 40(sp)       # Du1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 152(a0)      # Store back Amu0
    sw      t3, 156(a0)      # Store back Amu1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 8            # Shift amount
    ROL_HI  s9, t2, t3, t4   # s9 = BCo1
    ROL_LO  s4, t2, t3, t4   # s4 = BCo0

    # 5. Prepare BCu (from Asa)
    # C-Logic: Asa ^= Da; BCu = ROL(Asa, 18)
    lw      t2, 160(a0)      # Asa0 
    lw      t3, 164(a0)      # Asa1
    lw      t4, 4(sp)        # Da0
    lw      t5, 8(sp)        # Da1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 160(a0)      # Store back Asa0
    sw      t3, 164(a0)      # Store back Asa1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 18           # Shift amount
    ROL_HI  s10, t2, t3, t4  # s10 = BCu1
    ROL_LO  s5, t2, t3, t4   # s5 = BCu0

    # --- ROW 2 CHI (Eka, Eke, Eki, Eko, Eku) ---
    # Inputs: BCa(t0, t1), BCe(s2, s7), BCi(s3, s8), BCo(s4, s9), BCu(s5, s10)
    
    # Eka
    CHI_STEP t2, t0, s2, s3
    CHI_STEP t3, t1, s7, s8
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 60(sp)
    sw      t3, 64(sp)

    # Eke
    CHI_STEP t2, s2, s3, s4
    CHI_STEP t3, s7, s8, s9
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 68(sp)
    sw      t3, 72(sp)

    # Eki
    CHI_STEP t2, s3, s4, s5
    CHI_STEP t3, s8, s9, s10
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 76(sp)
    sw      t3, 80(sp)

    # Eko
    CHI_STEP t2, s4, s5, t0
    CHI_STEP t3, s9, s10, t1
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 84(sp)
    sw      t3, 88(sp)

    # Eku
    CHI_STEP t2, s5, t0, s2
    CHI_STEP t3, s10, t1, s7
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 92(sp)
    sw      t3, 96(sp)

# --- STEP 7: RHO / PI / CHI (Row 3 - M Row) --------------------------------------------------------

    # 1. BCa (from Abu) Rho 27
    lw      t2, 32(a0)       # Abu0
    lw      t3, 36(a0)       # Abu1
    lw      t4, 36(sp)       # Du0
    lw      t5, 40(sp)       # Du1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw    t2, 32(a0)       # Store back Abu0
    sw    t3, 36(a0)       # Store back Abu1
    //*DEBUG_PRINT_DA t2, t3 
    li      t4, 27
    ROL_HI  t1, t2, t3, t4
    ROL_LO  t0, t2, t3, t4

    # 2. BCe (from Aga) Rho 36
    lw      t2, 40(a0)       # Aga0
    lw      t3, 44(a0)       # Aga1
    lw      t4, 4(sp)        # Da0
    lw      t5, 8(sp)        # Da1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw    t2, 40(a0)       # Store back Aga0
    sw    t3, 44(a0)       # Store back Aga1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 36
    ROL_HI  s7, t2, t3, t4
    ROL_LO  s2, t2, t3, t4

    # 3. BCi (from Ake) Rho 10
    lw      t2, 88(a0)       # Ake0
    lw      t3, 92(a0)       # Ake1
    lw      t4, 12(sp)       # De0
    lw      t5, 16(sp)       # De1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw   t2, 88(a0)       # Store back Ake0
    sw   t3, 92(a0)       # Store back Ake1
    //*DEBUG_PRINT_DA t2, t3  
    li      t4, 10
    ROL_HI  s8, t2, t3, t4
    ROL_LO  s3, t2, t3, t4

    # 4. BCo (from Ami) Rho 15
    lw      t2, 136(a0)      # Ami0
    lw      t3, 140(a0)      # Ami1
    lw      t4, 20(sp)       # Di0
    lw      t5, 24(sp)       # Di1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw  t2, 136(a0)      # Store back Ami0
    sw  t3, 140(a0)      # Store back Ami1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 15
    ROL_HI  s9, t2, t3, t4
    ROL_LO  s4, t2, t3, t4

    # 5. BCu (from Aso) Rho 56
    lw      t2, 184(a0)      # Aso0
    lw      t3, 188(a0)      # Aso1
    lw      t4, 28(sp)       # Do0
    lw      t5, 32(sp)       # Do1
    xor     t2, t2, t4
    xor     t3, t3, t5
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 184(a0)      # Store back Aso0
    sw      t3, 188(a0)      # Store back Aso1
    li      t4, 56
    ROL_HI  s10, t2, t3, t4
    ROL_LO  s5, t2, t3, t4

    # --- ROW 3 CHI (Ema, Eme, Emi, Emo, Emu) ---
    CHI_STEP t2, t0, s2, s3
    CHI_STEP t3, t1, s7, s8
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 108(sp)
    sw      t3, 112(sp)

    CHI_STEP t2, s2, s3, s4
    CHI_STEP t3, s7, s8, s9
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 116(sp)
    sw      t3, 120(sp)
    
    CHI_STEP t2, s3, s4, s5
    CHI_STEP t3, s8, s9, s10
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 124(sp)
    sw      t3, 128(sp)
    
    CHI_STEP t2, s4, s5, t0
    CHI_STEP t3, s9, s10, t1
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 132(sp)
    sw      t3, 136(sp)

    CHI_STEP t2, s5, t0, s2
    CHI_STEP t3, s10, t1, s7
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 140(sp)
    sw      t3, 144(sp)

# --- STEP 8: RHO / PI / CHI (Row 4 - S Row) -----------------------------------------------------------

    # 1. BCa (from Abi) Rho 62
    lw      t2, 16(a0)       # Abi0
    lw      t3, 20(a0)       # Abi1
    lw      t4, 20(sp)       # Di0
    lw      t5, 24(sp)       # Di1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw    t2, 16(a0)       # Store back Abi0
    sw    t3, 20(a0)       # Store back Abi1    
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 62
    ROL_HI  s6, t2, t3, t4
    ROL_LO  s1, t2, t3, t4

    # 2. BCe (from Ago) Rho 55
    lw      t2, 64(a0)       # Ago0
    lw      t3, 68(a0)       # Ago1
    lw      t4, 28(sp)       # Do0
    lw      t5, 32(sp)       # Do1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw   t2, 64(a0)       # Store back Ago0
    sw   t3, 68(a0)       # Store back Ago1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 55
    ROL_HI  s7, t2, t3, t4
    ROL_LO  s2, t2, t3, t4

    # 3. BCi (from Aku) Rho 39
    lw      t2, 112(a0)      # Aku0
    lw      t3, 116(a0)      # Aku1
    lw      t4, 36(sp)       # Du0
    lw      t5, 40(sp)       # Du1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw   t2, 112(a0)      # Store back Aku0
    sw   t3, 116(a0)      # Store back Aku1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 39
    ROL_HI  s8, t2, t3, t4
    ROL_LO  s3, t2, t3, t4

    # 4. BCo (from Ama) Rho 41
    lw      t2, 120(a0)      # Ama0
    lw      t3, 124(a0)      # Ama1
    lw      t4, 4(sp)        # Da0
    lw      t5, 8(sp)        # Da1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw    t2, 120(a0)      # Store back Ama0
    sw    t3, 124(a0)      # Store back Ama1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 41
    ROL_HI  s9, t2, t3, t4
    ROL_LO  s4, t2, t3, t4

    # 5. BCu (from Ase) Rho 2
    lw      t2, 168(a0)      # Ase0
    lw      t3, 172(a0)      # Ase1
    lw      t4, 12(sp)       # De0
    lw      t5, 16(sp)       # De1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 168(a0)      # Store back Ase0
    sw      t3, 172(a0)      # Store back Ase1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 2
    ROL_HI  s10, t2, t3, t4
    ROL_LO  s5, t2, t3, t4

    # --- ROW 4 CHI ---
    CHI_STEP t2, s1, s2, s3   # Esa0
    CHI_STEP t3, s6, s7, s8   # Esa1
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 148(sp)
    sw      t3, 152(sp)
    
    CHI_STEP t2, s2, s3, s4   # Ese0
    CHI_STEP t3, s7, s8, s9   # Ese1
    //*DEBUG_PRINT_DA t2, t3    
    sw      t2, 156(sp)
    sw      t3, 160(sp)
    
    CHI_STEP t2, s3, s4, s5   # Esi0
    CHI_STEP t3, s8, s9, s10  # Esi1
    sw      t2, 164(sp)
    sw      t3, 168(sp)
    //*DEBUG_PRINT_DA t2, t3

    CHI_STEP t2, s4, s5, s1   # Eso0
    CHI_STEP t3, s9, s10, s6  # Eso1
    sw      t2, 172(sp)
    sw      t3, 176(sp)
    //*DEBUG_PRINT_DA t2, t3

    CHI_STEP t2, s5, s1, s2   # Esu0
    CHI_STEP t3, s10, s6, s7  # Esu1
    sw      t2, 180(sp)
    sw      t3, 184(sp)
    //*DEBUG_PRINT_DA t2, t3


#------------------------------------------------------------------------------
# --- PREPARE THETA (Calculate Column Parity from E state) --------------------------------------
#------------------------------------------------------------------------------

    # --- Column a (BCa0 = s1, BCa1 = s6) ---
    lw      t2, 228(sp)      # Eba0
    lw      t3, 232(sp)      # Eba1
    lw      t4, 148(sp)      # Esa0
    lw      t5, 152(sp)      # Esa1
    xor     s1, t2, t4       # BCa0 ^= Esa0 ^ Eba0
    xor     s6, t3, t5       # BCa1 ^= Esa1 ^ Eba1
    lw      t2, 108(sp)      # Ema0
    lw      t3, 112(sp)      # Ema1
    xor     s1, s1, t2       # BCa0 ^= Ema0
    xor     s6, s6, t3       # BCa1 ^= Ema1
    lw      t2, 60(sp)       # Eka0
    lw      t3, 64(sp)       # Eka1
    xor     s1, s1, t2       # BCa0 ^= Eka0
    xor     s6, s6, t3       # BCa1 ^= Eka1
    lw      t2, 188(sp)      # Ega0
    lw      t3, 192(sp)      # Ega1
    xor     s1, s1, t2       # BCa0 ^= Ega0
    xor     s6, s6, t3       # BCa1 ^= Ega1
    //*DEBUG_PRINT_DA s1, s6    # Print BCa

    # --- Column e (BCe0 = s2, BCe1 = s7) ---
    lw      s2, 236(sp)      # Ebe0
    lw      s7, 240(sp)      # Ebe1
    lw      t2, 196(sp)      # Ege0
    lw      t3, 200(sp)      # Ege1
    xor     s2, s2, t2
    xor     s7, s7, t3
    lw      t2, 68(sp)       # Eke0
    lw      t3, 72(sp)       # Eke1
    xor     s2, s2, t2
    xor     s7, s7, t3
    lw      t2, 116(sp)      # Eme0
    lw      t3, 120(sp)      # Eme1
    xor     s2, s2, t2
    xor     s7, s7, t3
    lw      t2, 156(sp)      # Ese0
    lw      t3, 160(sp)      # Ese1
    xor     s2, s2, t2
    xor     s7, s7, t3
    //*DEBUG_PRINT_DA s2, s7

    # --- Column i (BCi0 = s3, BCi1 = s8) ---
    lw      s3, 244(sp)      # Ebi0
    lw      s8, 248(sp)        # Ebi1
    lw      t2, 204(sp)      # Egi0
    lw      t3, 208(sp)      # Egi1
    xor     s3, s3, t2
    xor     s8, s8, t3
    lw      t2, 76(sp)       # Eki0
    lw      t3, 80(sp)       # Eki1
    xor     s3, s3, t2
    xor     s8, s8, t3
    lw      t2, 124(sp)      # Emi0
    lw      t3, 128(sp)      # Emi1
    xor     s3, s3, t2
    xor     s8, s8, t3
    lw      t2, 164(sp)      # Esi0
    lw      t3, 168(sp)      # Esi1
    xor     s3, s3, t2
    xor     s8, s8, t3
    //*DEBUG_PRINT_DA s3, s8

    # --- Column o (BCo0 = s4, BCo1 = s9) ---
    lw      s4, 252(sp)      # Ebo0
    lw      s9, 256(sp)      # Ebo1
    lw      t2, 212(sp)      # Ego0
    lw      t3, 216(sp)      # Ego1
    xor     s4, s4, t2
    xor     s9, s9, t3
    lw      t2, 84(sp)       # Eko0
    lw      t3, 88(sp)       # Eko1
    xor     s4, s4, t2
    xor     s9, s9, t3
    lw      t2, 132(sp)      # Emo0
    lw      t3, 136(sp)      # Emo1
    xor     s4, s4, t2
    xor     s9, s9, t3
    lw      t2, 172(sp)      # Eso0
    lw      t3, 176(sp)      # Eso1
    xor     s4, s4, t2
    xor     s9, s9, t3
    //*DEBUG_PRINT_DA s4, s9

    # --- Column u (BCu0 = s5, BCu1 = s10) ---
    lw      s5,  260(sp)     # Ebu0 
    lw      s10, 264(sp)     # Ebu1
    lw      t2, 220(sp)      # Egu0
    lw      t3, 224(sp)      # Egu1
    xor     s5, s5, t2
    xor     s10, s10, t3
    lw      t2, 92(sp)       # Eku0
    lw      t3, 96(sp)       # Eku1
    xor     s5, s5, t2
    xor     s10, s10, t3
    lw      t2, 140(sp)      # Emu0
    lw      t3, 144(sp)      # Emu1
    xor     s5, s5, t2
    xor     s10, s10, t3
    lw      t2, 180(sp)      # Esu0
    lw      t3, 184(sp)      # Esu1
    xor     s5, s5, t2
    xor     s10, s10, t3
    //*DEBUG_PRINT_DA s5, s10

/*****************************************************/
# --- STEP 2: PREPARE D ---------------------------------------------------------
    # Constant for rotation
    li      t6, 1

    # Da = BCu ^ ROL(BCe, 1)
    # BCu is s5/s10, BCe is s2/s7
    ROL_HI  t2, s2, s7, t6
    ROL_LO  t3, s2, s7, t6
    xor     t2, t2, s10     # Da1
    xor     t3, t3, s5      # Da0
    sw      t2, 8(sp)
    sw      t3, 4(sp)
    //*DEBUG_PRINT_DA t3, t2
    
    # De = BCa ^ ROL(BCi, 1)
    # BCa is s1/s6, BCi is s3/s8
    ROL_HI  t2, s3, s8, t6
    ROL_LO  t3, s3, s8, t6
    xor     t2, t2, s6      # De1
    xor     t3, t3, s1      # De0
    sw      t2, 16(sp)
    sw      t3, 12(sp)
    //*DEBUG_PRINT_DA t3, t2

    # Di = BCe ^ ROL(BCo, 1)
    # BCe is s2/s7, BCo is s4/s9
    ROL_HI  t2, s4, s9, t6
    ROL_LO  t3, s4, s9, t6
    xor     t2, t2, s7      # Di1
    xor     t3, t3, s2      # Di0
    sw      t2, 24(sp)
    sw      t3, 20(sp)
    //*DEBUG_PRINT_DA t3, t2

    # Do = BCi ^ ROL(BCu, 1)
    # BCi is s3/s8, BCu is s5/s10
    ROL_HI  t2, s5, s10, t6
    ROL_LO  t3, s5, s10, t6
    xor     t2, t2, s8      # Do1
    xor     t3, t3, s3      # Do0
    sw      t2, 32(sp)
    sw      t3, 28(sp)
    //*DEBUG_PRINT_DA t3, t2

    # Du = BCo ^ ROL(BCa, 1)
    # BCo is s4/s9, BCa is s1/s6
    ROL_HI  t2, s1, s6, t6
    ROL_LO  t3, s1, s6, t6
    xor     t2, t2, s9      # Du1
    xor     t3, t3, s4      # Du0
    sw      t2, 40(sp)
    sw      t3, 36(sp)
    //*DEBUG_PRINT_DA t3, t2

# --- STEP 3: RHO/PI/CHI --------------------------------------------------------
    
    # Example for Aba (Lane 0)
    lw      t2, 228(sp)     # Load Eba0
    lw      t3, 232(sp)     # Load Eba1
    lw      t4, 4(sp)       # Load Da0
    lw      t5, 8(sp)       # Load Da1
    xor     t2, t2, t4      # BCa0 = Eba0 ^ Da0
    xor     t3, t3, t5      # BCa1 = Eba1 ^ Da1
    sw      t2, 44(sp)      # Store new BCa0
    sw      t3, 48(sp)      # Store new BCa1
    //*DEBUG_PRINT_DA t2, t3

    lw      t2, 196(sp)       # Load Ege0
    lw      t3, 200(sp)       # Load Ege1
    lw      t4, 12(sp)       # Load De0
    lw      t5, 16(sp)       # Load De1
    xor     t2, t2, t4       # Ege0 = Ege0 ^ De0
    xor     t3, t3, t5       # Ege1 = Ege1 ^ De1
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 196(sp)      # Store back Ege0
    sw      t3, 200(sp)      # Store back Ege1
    # 1. Prepare BCe (from Ege)
    li      t6, 44           # Shift amount
    ROL_HI  s7, t2, t3, t6   # s7 = BCe1
    ROL_LO  s2, t2, t3, t6   # s2 = BCe0

    # 2. Prepare BCi (from Eki)
    # User logic: Eki ^= Di;
    lw      t2, 76(sp)       # Load eki0
    lw      t3, 80(sp)       # Load eki1
    lw      t4, 20(sp)       # Load Di0
    lw      t5, 24(sp)       # Load Di1
    xor     t2, t2, t4       # t2 = eki0 ^ Di0
    xor     t3, t3, t5       # t3 = eki1 ^ Di1
    sw      t2, 76(sp)       # Store back eki0
    sw      t3, 80(sp)       # Store back eki1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 43           # Shift amount
    ROL_HI  s8, t2, t3, t6   # s8 = BCi1
    ROL_LO  s3, t2, t3, t6   # s3 = BCi0

    # 3. Prepare BCo (from Emo)
    # User logic: Emo ^= Do; BCo = ROL(Emo, 21)
    lw      t2, 132(sp)      # Load Emo0
    lw      t3, 136(sp)      # Load Emo1
    lw      t4, 28(sp)       # Load Do0
    lw      t5, 32(sp)       # Load Do1
    xor     t2, t2, t4       # t2 = Emo0 ^ Do0
    xor     t3, t3, t5       # t3 = Emo1 ^ Do1
    sw      t2, 132(sp)      # Store back Emo0
    sw      t3, 136(sp)      # Store back Emo1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 21           # Shift amount
    ROL_HI  s9, t2, t3, t6   # s9 = BCo1
    ROL_LO  s4, t2, t3, t6   # s4 = BCo0

    # 4. Prepare BCu (from Esu)
    # User logic: Esu ^= Du; BCu = ROL(Esu, 14)
    lw      t2, 180(sp)      # Load Esu0
    lw      t3, 184(sp)      # Load Esu1
    lw      t4, 36(sp)       # Load Du0
    lw      t5, 40(sp)       # Load Du1
    xor     t2, t2, t4       # t2 = Esu0 ^ Du0
    xor     t3, t3, t5       # t3 = Esu1 ^ Du1
    sw      t2, 180(sp)      # Store back Esu0
    sw      t3, 184(sp)      # Store back Esu1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 14           # Shift amount
    ROL_HI  s10, t2, t3, t6  # s10 = BCu1
    ROL_LO  s5, t2, t3, t6   # s5  = BCu0

# --- STEP 4: CHI & IOTA --------------------------------------------------------
    # Load BCa (saved earlier in your code at 44/48)
    lw      t0, 44(sp)       # t0 = BCa0
    lw      t1, 48(sp)       # t1 = BCa1

    # Eba0 = (BCa0 ^ ((~BCe0) & BCi0)) ^ RC_low
    # Eba1 = (BCa1 ^ ((~BCe1) & BCi1)) ^ RC_high
    # Note: Using your s11 (round index) and s0 (RC pointer)
    slli    t4, s11, 3       # round * 8
    add     t4, t4, s0
    lw      t5, 8(t4)        # RC_low (index*2 + 2)
    lw      t6, 12(t4)        # RC_high (index*2 + 3)
    //*DEBUG_PRINT_DA t5, t6     # Print RC_low/high

    CHI_STEP a3, t0, s2, s3  # a3 = BCa0 ^ ((~BCe0) & BCi0)
    xor      s1, a3, t6      # s1 = aba0 (Result stored in s1)
    CHI_STEP a4, t1, s7, s8  # a4 = BCa1 ^ ((~BCe1) & BCi1)
    xor      s6, a4, t5      # s6 = aba1 (Result stored in s6)
    //*DEBUG_PRINT_DA s1, s6    # Print aba0/1
    sw    s1, 0(a0)        # Store Aba0
    sw    s6, 4(a0)        # Store Aba1

    # Abe0 = BCe0 ^ ((~BCi0) & BCo0)
    # Abe1 = BCe1 ^ ((~BCi1) & BCo1)
    CHI_STEP t2, s2, s3, s4  # t2 = Abe0
    CHI_STEP t3, s7, s8, s9  # t3 = Abe1
    //*DEBUG_PRINT_DA t2, t3
    sw     t2, 8(a0)         # Store Abe0
    sw     t3, 12(a0)        # Store Abe1

    # Abi0 = BCi0 ^ ((~BCo0) & BCu0)
    # Abi1 = BCi1 ^ ((~BCo1) & BCu1)
    CHI_STEP t2, s3, s4, s5  # t2 = Abi0
    CHI_STEP t3, s8, s9, s10 # t3 = Abi1
    //*DEBUG_PRINT_DA t2, t3
    sw     t2, 16(a0)        # Store Abi0
    sw     t3, 20(a0)        # Store Abi1

    # Abo0 = BCo0 ^ ((~BCu0) & BCa0)
    # Abo1 = BCo1 ^ ((~BCu1) & BCa1)
    CHI_STEP t2, s4, s5, t0  # t2 = Abo0
    CHI_STEP t3, s9, s10, t1 # t3 = Abo1
    //*DEBUG_PRINT_DA t2, t3
    sw     t2, 24(a0)        # Store Abo0
    sw     t3, 28(a0)        # Store Abo1

    # Abu0 = BCu0 ^ ((~BCa0) & BCe0)
    # Abu1 = BCu1 ^ ((~BCa1) & BCe1)
    CHI_STEP t2, s5, t0, s2  # t2 = Abu0
    CHI_STEP t3, s10, t1, s7 # t3 = Abu1
    //*DEBUG_PRINT_DA t2, t3
    sw     t2, 32(a0)        # Store Abu0
    sw     t3, 36(a0)        # Store Abu1

# --- STEP 5: RHO / PI / CHI (Row 1 - G Row) --------------------------------------------------------

    lw      t2, 252(sp)      # Load Ebo0
    lw      t3, 256(sp)      # Load Ebo1
    lw      t4, 28(sp)       # Load Do0
    lw      t5, 32(sp)       # Load Do1
    xor     t2, t2, t4       # Ebo0 = Ebo0 ^ Do0
    xor     t3, t3, t5       # Ebo1 = Ebo1 ^ Do1
    sw      t2, 252(sp)      # Store back Ebo0
    sw      t3, 256(sp)      # Store back Ebo1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 28           # Shift amount
    ROL_HI  s6, t2, t3, t6   # s6 = BCa1
    ROL_LO  s1, t2, t3, t6   # s1 = BCa0

    # 2. Prepare BCe (from Egu)
    # C-Logic: Egu ^= Du; BCe = ROL(Egu, 20)
    lw      t2, 220(sp)      # Egu0 (Row 1 Col 4: 40 + 32)
    lw      t3, 224(sp)      # Egu1
    lw      t4, 36(sp)       # Du0
    lw      t5, 40(sp)       # Du1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 220(sp)       # Store back Egu0
    sw      t3, 224(sp)       # Store back Egu1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 20           # Shift amount (Using t4 because t1 is BCa1)
    ROL_HI  s7, t2, t3, t6   # s7 = BCe1
    ROL_LO  s2, t2, t3, t6   # s2 = BCe0

    # 3. Prepare BCi (from Eka)
    # C-Logic: Eka ^= Da; BCi = ROL(Eka, 3)
    lw      t2, 60(sp)       # Eka0 
    lw      t3, 64(sp)       # Eka1
    lw      t4, 4(sp)        # Da0
    lw      t5, 8(sp)        # Da1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 60(sp)       # Store back Eka0
    sw      t3, 64(sp)       # Store back Eka1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 3            # Shift amount
    ROL_HI  s8, t2, t3, t4   # s8 = BCi1
    ROL_LO  s3, t2, t3, t4   # s3 = BCi0

    # 4. Prepare BCo (from Eme)
    # C-Logic: Eme ^= De; BCo = ROL(Eme, 45)
    lw      t2, 116(sp)      # Eme0 (Row 3 Col 1: 120 + 8)
    lw      t3, 120(sp)      # Eme1
    lw      t4, 12(sp)       # De0
    lw      t5, 16(sp)       # De1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 116(sp)      # Store back Eme0
    sw      t3, 120(sp)      # Store back Eme1  
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 45           # Shift amount
    ROL_HI  s9, t2, t3, t4   # s9 = BCo1
    ROL_LO  s4, t2, t3, t4   # s4 = BCo0

    # 5. Prepare BCu (from Esi)
    # C-Logic: Esi ^= Di; BCu = ROL(Esi, 61)
    lw      t2, 164(sp)      # Esi0 (Row 4 Col 2: 160 + 16)
    lw      t3, 168(sp)      # Esi1
    lw      t4, 20(sp)       # Di0
    lw      t5, 24(sp)       # Di1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 164(sp)      # Store back Esi0
    sw      t3, 168(sp)      # Store back Esi1
    //*DEBUG_PRINT_DA t2, t3
    li      t4, 61           # Shift amount
    ROL_HI  s10, t2, t3, t4  # s10 = BCu1
    ROL_LO  s5, t2, t3, t4   # s5 = BCu0

   # --- ROW 1 CHI (Aga, Age, Agi, Ago, Agu) ---

    # Aga0 = BCa0 ^ ((~BCe0) & BCi0)
    # Aga1 = BCa1 ^ ((~BCe1) & BCi1)
    CHI_STEP t2, s1, s2, s3  # t2 = Aga0
    CHI_STEP t3, s6, s7, s8  # t3 = Aga1
    sw     t2, 40(a0)        # Store Aga0
    sw     t3, 44(a0)        # Store Aga1
    //*DEBUG_PRINT_DA t2, t3

    # Age0 = BCe0 ^ ((~BCi0) & BCo0)
    # Age1 = BCe1 ^ ((~BCi1) & BCo1)
    CHI_STEP t2, s2, s3, s4  # t2 = Age0
    CHI_STEP t3, s7, s8, s9  # t3 = Age1
    //*DEBUG_PRINT_DA t2, t3
    sw    t2, 48(a0)        # Store Age0
    sw    t3, 52(a0)        # Store Age1

    # Agi0 = BCi0 ^ ((~BCo0) & BCu0)
    # Agi1 = BCi1 ^ ((~BCo1) & BCu1)
    CHI_STEP t2, s3, s4, s5  # t2 = Agi0
    CHI_STEP t3, s8, s9, s10 # t3 = Agi1
    //*DEBUG_PRINT_DA t2, t3
    sw    t2, 56(a0)        # Store Agi0
    sw    t3, 60(a0)        # Store Agi1

    # Ago0 = BCo0 ^ ((~BCu0) & BCa0)
    # Ago1 = BCo1 ^ ((~BCu1) & BCa1)
    CHI_STEP t2, s4, s5, s1  # t2 = Ago0
    CHI_STEP t3, s9, s10, s6 # t3 = Ago1
    //*DEBUG_PRINT_DA t2, t3
    sw    t2, 64(a0)        # Store Ago0
    sw    t3, 68(a0)        # Store Ago1

    # Agu0 = BCu0 ^ ((~BCa0) & BCe0)
    # Agu1 = BCu1 ^ ((~BCa1) & BCe1)
    CHI_STEP t2, s5, s1, s2  # t2 = Agu0
    CHI_STEP t3, s10, s6, s7 # t3 = Agu1
    //*DEBUG_PRINT_DA t2, t3
    sw    t2, 72(a0)        # Store Agu0
    sw    t3, 76(a0)        # Store Agu1

# --- STEP 6: RHO / PI / CHI (Row 2 Mapping) --------------------------------------------------------

    # 1. Prepare BCa (from Ebe)
    # C-Logic: Ebe ^= De; BCa = ROL(Ebe, 1)
    lw      t2, 236(sp)        # Ebe0
    lw      t3, 240(sp)       # Ebe1
    lw      t4, 12(sp)       # De0
    lw      t5, 16(sp)       # De1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw     t2, 236(sp)        # Store back Ebe0
    sw     t3, 240(sp)       # Store back Ebe1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 1            # Shift amount
    ROL_HI  t1, t2, t3, t6   # t1 = BCa1
    ROL_LO  t0, t2, t3, t6   # t0 = BCa0

    # 2. Prepare BCe (from Egi)
    # C-Logic: Egi ^= Di; BCe = ROL(Egi, 6)
    lw      t2, 204(sp)       # Egi0 (Row 2 Col 1: 80 + 24)
    lw      t3, 208(sp)       # Egi1
    lw      t4, 20(sp)       # Di0
    lw      t5, 24(sp)       # Di1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 204(sp)       # Store back Egi0
    sw      t3, 208(sp)       # Store back Egi1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 6            # Shift amount
    ROL_HI  s7, t2, t3, t6   # s7 = BCe1
    ROL_LO  s2, t2, t3, t6   # s2 = BCe0

    # 3. Prepare BCi (from Eko)
    # C-Logic: Eko ^= Do; BCi = ROL(Eko, 25)
    lw      t2, 84(sp)      # Eko0 (Row 2 Col 3: 80 + 24)
    lw      t3, 88(sp)      # Eko1
    lw      t4, 28(sp)       # Do0
    lw      t5, 32(sp)       # Do1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 84(sp)      # Store back Eko0
    sw      t3, 88(sp)      # Store back Eko1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 25           # Shift amount
    ROL_HI  s8, t2, t3, t6   # s8 = BCi1
    ROL_LO  s3, t2, t3, t6   # s3 = BCi0

    # 4. Prepare BCo (from Emu)
    # C-Logic: Emu ^= Du; BCo = ROL(Emu, 8)
    lw      t2, 140(sp)      # Emu0 (Row 3 Col 4: 120 + 32)
    lw      t3, 144(sp)      # Emu1
    lw      t4, 36(sp)       # Du0
    lw      t5, 40(sp)       # Du1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 140(sp)      # Store back Emu0
    sw      t3, 144(sp)      # Store back Emu1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 8            # Shift amount
    ROL_HI  s9, t2, t3, t6   # s9 = BCo1
    ROL_LO  s4, t2, t3, t6   # s4 = BCo0

    # 5. Prepare BCu (from Esa)
    # C-Logic: Esa ^= Da; BCu = ROL(Esa, 18)
    lw      t2, 148(sp)      # Esa0 
    lw      t3, 152(sp)      # Esa1
    lw      t4, 4(sp)        # Da0
    lw      t5, 8(sp)        # Da1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 148(sp)      # Store back Esa0
    sw      t3, 152(sp)      # Store back Esa1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 18           # Shift amount
    ROL_HI  s10, t2, t3, t6  # s10 = BCu1
    ROL_LO  s5, t2, t3, t6   # s5 = BCu0

 # --- ROW 2 CHI (aKa, aKe, Aki, Ako, Aku) ---
    # Inputs: BCa(t0, t1), BCe(s2, s7), BCi(s3, s8), BCo(s4, s9), BCu(s5, s10)
    
    # Aka
    CHI_STEP t2, t0, s2, s3
    CHI_STEP t3, t1, s7, s8
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 80(a0)
    sw      t3, 84(a0)

    # Ake
    CHI_STEP t2, s2, s3, s4
    CHI_STEP t3, s7, s8, s9
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 88(a0)
    sw      t3, 92(a0)

    # Aki
    CHI_STEP t2, s3, s4, s5
    CHI_STEP t3, s8, s9, s10
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 96(a0)
    sw      t3, 100(a0)

    # Ako
    CHI_STEP t2, s4, s5, t0
    CHI_STEP t3, s9, s10, t1
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 104(a0)
    sw      t3, 108(a0)

    # Aku
    CHI_STEP t2, s5, t0, s2
    CHI_STEP t3, s10, t1, s7
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 112(a0)
    sw      t3, 116(a0)

# --- STEP 7: RHO / PI / CHI (Row 3 - M Row) --------------------------------------------------------

    # 1. BCa (from Ebu) Rho 27
    lw      t2, 260(sp)       # Ebu0
    lw      t3, 264(sp)       # Ebu1
    lw      t4, 36(sp)       # Du0
    lw      t5, 40(sp)       # Du1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw    t2, 260(sp)       # Store back Ebu0
    sw    t3, 264(sp)       # Store back Ebu1
    //*DEBUG_PRINT_DA t2, t3 
    li      t6, 27
    ROL_HI  t1, t2, t3, t6
    ROL_LO  t0, t2, t3, t6

    # 2. BCe (from Ega) Rho 36
    lw      t2, 188(sp)       # Ega0
    lw      t3, 192(sp)       # Ega1
    lw      t4, 4(sp)        # Da0
    lw      t5, 8(sp)        # Da1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw    t2, 188(sp)       # Store back Ega0
    sw    t3, 192(sp)       # Store back Ega1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 36
    ROL_HI  s7, t2, t3, t6
    ROL_LO  s2, t2, t3, t6

    # 3. BCi (from Eke) Rho 10
    lw      t2, 68(sp)       # Eke0
    lw      t3, 72(sp)       # Eke1
    lw      t4, 12(sp)       # De0
    lw      t5, 16(sp)       # De1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw   t2, 68(sp)       # Store back Eke0
    sw   t3, 72(sp)       # Store back Eke1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 10
    ROL_HI  s8, t2, t3, t6
    ROL_LO  s3, t2, t3, t6

    # 4. BCo (from emi) Rho 15
    lw      t2, 124(sp)      # Emi0
    lw      t3, 128(sp)      # Emi1
    lw      t4, 20(sp)       # Di0
    lw      t5, 24(sp)       # Di1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw  t2, 124(sp)      # Store back Emi0
    sw  t3, 128(sp)      # Store back Emi1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 15
    ROL_HI  s9, t2, t3, t6
    ROL_LO  s4, t2, t3, t6

    # 5. BCu (from Eso) Rho 56
    lw      t2, 172(sp)      # Eso0
    lw      t3, 176(sp)      # Eso1
    lw      t4, 28(sp)       # Do0
    lw      t5, 32(sp)       # Do1
    xor     t2, t2, t4
    xor     t3, t3, t5
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 184(sp)      # Store back Eso0
    sw      t3, 188(sp)      # Store back Eso1
    li      t6, 56
    ROL_HI  s10, t2, t3, t6
    ROL_LO  s5, t2, t3, t6

    # --- ROW 3 CHI (Ama, Ame, Ami, Amo, Amu) ---
    CHI_STEP t2, t0, s2, s3
    CHI_STEP t3, t1, s7, s8
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 120(a0)
    sw      t3, 124(a0)

    CHI_STEP t2, s2, s3, s4
    CHI_STEP t3, s7, s8, s9
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 128(a0)
    sw      t3, 132(a0)
    
    CHI_STEP t2, s3, s4, s5
    CHI_STEP t3, s8, s9, s10
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 136(a0)
    sw      t3, 140(a0)

    CHI_STEP t2, s4, s5, t0
    CHI_STEP t3, s9, s10, t1
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 144(a0)
    sw      t3, 148(a0)

    CHI_STEP t2, s5, t0, s2
    CHI_STEP t3, s10, t1, s7
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 152(a0)
    sw      t3, 156(a0)

# --- STEP 8: RHO / PI / CHI (Row 4 - S Row) -----------------------------------------------------------

    # 1. BCa (from Ebi) Rho 62
    lw      t2, 244(sp)       # Ebi0
    lw      t3, 248(sp)       # Ebi1
    lw      t4, 20(sp)       # Di0
    lw      t5, 24(sp)       # Di1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw    t2, 244(sp)       # Store back Ebi0
    sw    t3, 248(sp)       # Store back Ebi1    
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 62
    ROL_HI  s6, t2, t3, t6
    ROL_LO  s1, t2, t3, t6

    # 2. BCe (from Ego) Rho 55
    lw      t2, 212(sp)       # Ego0
    lw      t3, 216(sp)       # Ego1
    lw      t4, 28(sp)       # Do0
    lw      t5, 32(sp)       # Do1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw    t2, 212(sp)       # Store back Ego0
    sw    t3, 216(sp)       # Store back Ego1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 55
    ROL_HI  s7, t2, t3, t6
    ROL_LO  s2, t2, t3, t6    

    # 3. BCi (from Eku) Rho 39
    lw      t2, 92(sp)      # Eku0
    lw      t3, 96(sp)      # Eku1
    lw      t4, 36(sp)       # Du0
    lw      t5, 40(sp)       # Du1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw   t2, 92(sp)      # Store back Eku0
    sw   t3, 96(sp)      # Store back Eku1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 39
    ROL_HI  s8, t2, t3, t6
    ROL_LO  s3, t2, t3, t6

    # 4. BCo (from Ema) Rho 41
    lw      t2, 108(sp)      # Ema0
    lw      t3, 112(sp)      # Ema1
    lw      t4, 4(sp)        # Da0
    lw      t5, 8(sp)        # Da1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw    t2, 108(sp)      # Store back Ema0
    sw    t3, 112(sp)      # Store back Ema1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 41
    ROL_HI  s9, t2, t3, t6
    ROL_LO  s4, t2, t3, t6

    # 5. BCu (from Ese) Rho 2
    lw      t2, 156(sp)      # Ese0
    lw      t3, 160(sp)      # Ese1
    lw      t4, 12(sp)       # De0
    lw      t5, 16(sp)       # De1
    xor     t2, t2, t4
    xor     t3, t3, t5
    sw      t2, 156(sp)      # Store back Ese0
    sw      t3, 160(sp)      # Store back Ese1
    //*DEBUG_PRINT_DA t2, t3
    li      t6, 2
    ROL_HI  s10, t2, t3, t6
    ROL_LO  s5, t2, t3, t6

    # --- ROW 4 CHI ---------------------------
    CHI_STEP t2, s1, s2, s3   # Asa0
    CHI_STEP t3, s6, s7, s8   # Asa1
    //*DEBUG_PRINT_DA t2, t3
    sw      t2, 160(a0)
    sw      t3, 164(a0)
    
    CHI_STEP t2, s2, s3, s4   # Ase0
    CHI_STEP t3, s7, s8, s9   # Ase1
    //*DEBUG_PRINT_DA t2, t3    
    sw      t2, 168(a0)
    sw      t3, 172(a0)
    
    CHI_STEP t2, s3, s4, s5   # Asi0
    CHI_STEP t3, s8, s9, s10  # Asi1
    sw      t2, 176(a0)
    sw      t3, 180(a0)
    //*DEBUG_PRINT_DA t2, t3

    CHI_STEP t2, s4, s5, s1   # Aso0
    CHI_STEP t3, s9, s10, s6  # Aso1
    sw      t2, 184(a0)
    sw      t3, 188(a0)
    //*DEBUG_PRINT_DA t2, t3

    CHI_STEP t2, s5, s1, s2   # Asu0
    CHI_STEP t3, s10, s6, s7  # Asu1
    sw      t2, 192(a0)
    sw      t3, 196(a0)
    //*DEBUG_PRINT_DA t2, t3


    # --- LOOP INCREMENT ---
    addi    s11, s11, 2
    li      t2, 24
    blt     s11, t2, round_loop

    # 3. Epilogue: Restore registers and Return
    lw      ra, 320(sp)
    lw      s0, 312(sp)
    lw      s1, 308(sp)
    lw      s2, 304(sp)
    lw      s3, 300(sp)
    lw      s4, 296(sp)
    lw      s5, 292(sp)
    lw      s6, 288(sp)
    lw      s7, 284(sp)
    lw      s8, 280(sp)
    lw      s9, 276(sp)
    lw      s10, 272(sp)
    lw      s11, 268(sp)

    addi    sp, sp, 320
    ret

.section .rodata
KeccakF1600RoundConstants32:
    .word 0x00000000, 0x00000001
    .word 0x00000000, 0x00008082
    .word 0x80000000, 0x0000808a
    .word 0x80000000, 0x80008000
    .word 0x00000000, 0x0000808b
    .word 0x00000000, 0x80000001
    .word 0x80000000, 0x80008081
    .word 0x80000000, 0x00008009
    .word 0x00000000, 0x0000008a
    .word 0x00000000, 0x00000088
    .word 0x00000000, 0x80008009
    .word 0x00000000, 0x8000000a
    .word 0x00000000, 0x8000808b
    .word 0x80000000, 0x0000008b
    .word 0x80000000, 0x00008089
    .word 0x80000000, 0x00008003
    .word 0x80000000, 0x00008002
    .word 0x80000000, 0x00000080
    .word 0x00000000, 0x0000800a
    .word 0x80000000, 0x8000000a
    .word 0x80000000, 0x80008081
    .word 0x80000000, 0x00008080
    .word 0x00000000, 0x80000001
    .word 0x80000000, 0x80008008          
  