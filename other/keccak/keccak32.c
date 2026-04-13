#include <stdint.h>
#include <string.h>
#include "keccak32.h"

#define NROUNDS 24

void print_debug(const char *s1, const char *s2, uint32_t in1, uint32_t in2)
{
    //printf("Debug %s, %s: %08X - %08X\n", s1, s2, in1, in2);
}


static const uint32_t KeccakF1600RoundConstants32[2 * 24] = {
    0x00000000, 0x00000001,
    0x00000000, 0x00008082,
    0x80000000, 0x0000808a,
    0x80000000, 0x80008000,
    0x00000000, 0x0000808b,
    0x00000000, 0x80000001,
    0x80000000, 0x80008081,
    0x80000000, 0x00008009,
    0x00000000, 0x0000008a,
    0x00000000, 0x00000088,
    0x00000000, 0x80008009,
    0x00000000, 0x8000000a,
    0x00000000, 0x8000808b,
    0x80000000, 0x0000008b,
    0x80000000, 0x00008089,
    0x80000000, 0x00008003,
    0x80000000, 0x00008002,
    0x80000000, 0x00000080,
    0x00000000, 0x0000800a,
    0x80000000, 0x8000000a,
    0x80000000, 0x80008081,
    0x80000000, 0x00008080,
    0x00000000, 0x80000001,
    0x80000000, 0x80008008
};


void rol32_asm(uint32_t a, uint32_t b, unsigned int offset, uint32_t *result1, uint32_t *result2) {
        
    asm volatile (
        ".insn r 0x6b, 0x01, 0x0, %[rd1],  %[rs2], %[rs1], %[i]\n\t"
        ".insn r 0x6b, 0x01, 0x1, %[rd2],  %[rs2], %[rs1], %[i]\n\t"
        : [rd1] "=r" (*result1), [rd2] "=r" (*result2)
        : [rs1] "r" (a), [rs2] "r" (b), [i] "r" (offset)
        : "cc");
    }

void Keccak32(uint32_t *state){

    uint32_t Aba0, Abe0, Abi0, Abo0, Abu0;
    uint32_t Aba1, Abe1, Abi1, Abo1, Abu1;
    uint32_t Aga0, Age0, Agi0, Ago0, Agu0;
    uint32_t Aga1, Age1, Agi1, Ago1, Agu1;
    uint32_t Aka0, Ake0, Aki0, Ako0, Aku0;
    uint32_t Aka1, Ake1, Aki1, Ako1, Aku1;
    uint32_t Ama0, Ame0, Ami0, Amo0, Amu0;
    uint32_t Ama1, Ame1, Ami1, Amo1, Amu1;
    uint32_t Asa0, Ase0, Asi0, Aso0, Asu0;
    uint32_t Asa1, Ase1, Asi1, Aso1, Asu1;
    uint32_t BCa0, BCe0, BCi0, BCo0, BCu0;
    uint32_t BCa1, BCe1, BCi1, BCo1, BCu1;
    uint32_t Da0, De0, Di0, Do0, Du0;
    uint32_t Da1, De1, Di1, Do1, Du1;
    uint32_t Eba0, Ebe0, Ebi0, Ebo0, Ebu0;
    uint32_t Eba1, Ebe1, Ebi1, Ebo1, Ebu1;
    uint32_t Ega0, Ege0, Egi0, Ego0, Egu0;
    uint32_t Ega1, Ege1, Egi1, Ego1, Egu1;
    uint32_t Eka0, Eke0, Eki0, Eko0, Eku0;
    uint32_t Eka1, Eke1, Eki1, Eko1, Eku1;
    uint32_t Ema0, Eme0, Emi0, Emo0, Emu0;
    uint32_t Ema1, Eme1, Emi1, Emo1, Emu1;
    uint32_t Esa0, Ese0, Esi0, Eso0, Esu0;
    uint32_t Esa1, Ese1, Esi1, Eso1, Esu1;

    //copyFromState(A, state)
        Aba0 = state[ 0];
        Aba1 = state[ 1];
        Abe0 = state[ 2];
        Abe1 = state[ 3];
        Abi0 = state[ 4];
        Abi1 = state[ 5];
        Abo0 = state[ 6];
        Abo1 = state[ 7];
        Abu0 = state[ 8];
        Abu1 = state[ 9];
        Aga0 = state[10];
        Aga1 = state[11];
        Age0 = state[12];
        Age1 = state[13];
        Agi0 = state[14];
        Agi1 = state[15];
        Ago0 = state[16];
        Ago1 = state[17];
        Agu0 = state[18];
        Agu1 = state[19];
        Aka0 = state[20];
        Aka1 = state[21];
        Ake0 = state[22];
        Ake1 = state[23];
        Aki0 = state[24];
        Aki1 = state[25];
        Ako0 = state[26];
        Ako1 = state[27];
        Aku0 = state[28];
        Aku1 = state[29];
        Ama0 = state[30];
        Ama1 = state[31];
        Ame0 = state[32];
        Ame1 = state[33];
        Ami0 = state[34];
        Ami1 = state[35];
        Amo0 = state[36];
        Amo1 = state[37];
        Amu0 = state[38];
        Amu1 = state[39];
        Asa0 = state[40];
        Asa1 = state[41];
        Ase0 = state[42];
        Ase1 = state[43];
        Asi0 = state[44];
        Asi1 = state[45];
        Aso0 = state[46];
        Aso1 = state[47];
        Asu0 = state[48];
        Asu1 = state[49];

        uint32_t Da0a, Da0b;
        uint32_t De0a, De0b;
        uint32_t Di0a, Di0b;
        uint32_t Do0a, Do0b;
        uint32_t Du0a, Du0b;
        uint32_t BCa0a, BCa0b;
        uint32_t BCe0a, BCe0b;
        uint32_t BCi0a, BCi0b;
        uint32_t BCo0a, BCo0b;
        uint32_t BCu0a, BCu0b;


        for(int round = 0; round < 24; round += 2 )
        {
            //printf("\n\n\n\nround=%d\n\n\n\n\n\n", round);

            //    prepareTheta
            BCa0 = Aba0^Aga0^Aka0^Ama0^Asa0;
            BCa1 = Aba1^Aga1^Aka1^Ama1^Asa1;
            BCe0 = Abe0^Age0^Ake0^Ame0^Ase0;
            BCe1 = Abe1^Age1^Ake1^Ame1^Ase1;
            BCi0 = Abi0^Agi0^Aki0^Ami0^Asi0;
            BCi1 = Abi1^Agi1^Aki1^Ami1^Asi1;
            BCo0 = Abo0^Ago0^Ako0^Amo0^Aso0;
            BCo1 = Abo1^Ago1^Ako1^Amo1^Aso1;
            BCu0 = Abu0^Agu0^Aku0^Amu0^Asu0;
            BCu1 = Abu1^Agu1^Aku1^Amu1^Asu1;
            print_debug("BCa0", "Bca1", BCa0, BCa1);
            print_debug("BCe0", "Bce1", BCe0, BCe1);
            print_debug("BCi0", "Bci1", BCi0, BCi1);
            print_debug("BCo0", "Bco1", BCo0, BCo1);
            print_debug("BCu0", "Bcu1", BCu0, BCu1);

            //thetaRhoPiChiIota(round  , A, E)
            rol32_asm(BCe1, BCe0, 1, &Da0a, &Da0b);
            Da0 = BCu0^Da0b;
            Da1 = BCu1^Da0a;
            print_debug("Da0", "Da1", Da0, Da1);

            rol32_asm(BCi1, BCi0, 1, &De0a, &De0b);
            De0 = BCa0^De0b;
            De1 = BCa1^De0a;
            print_debug("De0", "De1", De0, De1);

            rol32_asm(BCo1, BCo0, 1, &Di0a, &Di0b);
            Di0 = BCe0^Di0b;
            Di1 = BCe1^Di0a;
            print_debug("Di0", "Di1", Di0, Di1);

            rol32_asm(BCu1, BCu0, 1, &Do0a, &Do0b);
            Do0 = BCi0^Do0b;
            Do1 = BCi1^Do0a;
            print_debug("Do0", "Do1", Do0, Do1);

            rol32_asm(BCa1, BCa0, 1, &Du0a, &Du0b);
            Du0 = BCo0^Du0b;
            Du1 = BCo1^Du0a;
            print_debug("Du0", "Du1", Du0, Du1);

            BCa0 = Aba0 ^ Da0;
            BCa1 = Aba1 ^ Da1;
            print_debug("BCa0", "BCa1", BCa0, BCa1);
            print_debug("Da0", "Da1", Da0, Da1);

            Age0 ^= De0;
            Age1 ^= De1;
            print_debug("Age0", "Age1", Age0, Age1);
            rol32_asm(Age1, Age0, 44, &BCe1, &BCe0);

            Aki1 ^= Di1;
            Aki0 ^= Di0;
            print_debug("Aki0", "Aki1", Aki0, Aki1);
            rol32_asm(Aki1, Aki0, 43, &BCi1, &BCi0);

            Amo1 ^= Do1;
            Amo0 ^= Do0;
            print_debug("Amo0", "Amo1", Amo0, Amo1);
            rol32_asm(Amo1, Amo0, 21, &BCo1, &BCo0);

            Asu0 ^= Du0;
            Asu1 ^= Du1;
            print_debug("Asu0", "Asu1", Asu0, Asu1);
            rol32_asm(Asu1, Asu0, 14, &BCu1, &BCu0);

            //Eba0 =   BCa0 ^((~BCe0)&  BCi0 );
            //Eba1 =   BCa1 ^((~BCe1)&  BCi1 );
            print_debug("rc1", "rc0", KeccakF1600RoundConstants32[round*2+1], KeccakF1600RoundConstants32[round*2+0]);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, a3,  %[rs1a], %[rs2a], %[rs3a]\n\t"
                "xor %[rda], a3, %[keccak1]\n\t"
                ".insn r 0x6b, 0x01, 0x2, a5,  %[rs1b], %[rs2b], %[rs3b]\n\t"
                "xor %[rdb], a5, %[keccak2]\n\t"
                : [rda] "=r" (Eba0), [rdb] "=r" (Eba1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0), [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1), [keccak1] "r" (KeccakF1600RoundConstants32[round*2+1]), [keccak2] "r" (KeccakF1600RoundConstants32[round*2+0])
                : "a3", "a5", "cc");
            print_debug("Eba0", "Eba1", Eba0, Eba1);


            //Ebe0 =   BCe0 ^((~BCi0)&  BCo0 );
            //Ebe1 =   BCe1 ^((~BCi1)&  BCo1 );
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ebe0), [rdb] "=r" (Ebe1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0), [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");
            print_debug("Ebe0", "Ebe1", Ebe0, Ebe1);

            //Ebi0 =   BCi0 ^((~BCo0)&  BCu0 );
            //Ebi1 =   BCi1 ^((~BCo1)&  BCu1 );
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ebi0), [rdb] "=r" (Ebi1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0), [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");
            print_debug("Ebi0", "Ebi1", Ebi0, Ebi1);

            //Ebo0 =   BCo0 ^((~BCu0)&  BCa0 );
            //Ebo1 =   BCo1 ^((~BCu1)&  BCa1 );

            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ebo0), [rdb] "=r" (Ebo1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0), [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");
            print_debug("Ebo0", "Ebo1", Ebo0, Ebo1);

            //Ebu0 =   BCu0 ^((~BCa0)&  BCe0 );
            //Ebu1 =   BCu1 ^((~BCa1)&  BCe1 );
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ebu0), [rdb] "=r" (Ebu1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0), [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");
            print_debug("Ebu0", "Ebu1", Ebu0, Ebu1);
    
            Abo0 ^= Do0;
            Abo1 ^= Do1;
            print_debug("Abo0", "Abo1", Abo0, Abo1);
            rol32_asm(Abo1, Abo0, 28, &BCa1, &BCa0);

            Agu0 ^= Du0;
            Agu1 ^= Du1;
            print_debug("Agu0", "Agu1", Agu0, Agu1);
            rol32_asm(Agu1, Agu0, 20, &BCe1, &BCe0);

            Aka1 ^= Da1;
            Aka0 ^= Da0;
            print_debug("Aka0", "Aka1", Aka0, Aka1);
            rol32_asm(Aka1, Aka0, 3, &BCi1, &BCi0);

            Ame1 ^= De1;
            Ame0 ^= De0;
            print_debug("Ame0", "Ame1", Ame0, Ame1);
            rol32_asm(Ame1, Ame0, 45, &BCo1, &BCo0);

            Asi1 ^= Di1;
            Asi0 ^= Di0;
            print_debug("Asi0", "Asi1", Asi0, Asi1);
            rol32_asm(Asi1, Asi0, 61, &BCu1, &BCu0);

            // Ega0 = BCa0 ^ ((~BCe0) & BCi0);
            // Ega1 = BCa1 ^ ((~BCe1) & BCi1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ega0), [rdb] "=r" (Ega1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
                [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
                : "cc");
            print_debug("Ega0", "Ega1", Ega0, Ega1);

            // Ege0 = BCe0 ^ ((~BCi0) & BCo0);
            // Ege1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ege0), [rdb] "=r" (Ege1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");
            print_debug("Ege0", "Ege1", Ege0, Ege1);

            // Egi0 = BCi0 ^ ((~BCo0) & BCu0);
            // Egi1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Egi0), [rdb] "=r" (Egi1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");
            print_debug("Egi0", "Egi1", Egi0, Egi1);

            // Ego0 = BCo0 ^ ((~BCu0) & BCa0);
            // Ego1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ego0), [rdb] "=r" (Ego1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");
            print_debug("Ego0", "Ego1", Ego0, Ego1);

            // Egu0 = BCu0 ^ ((~BCa0) & BCe0);
            // Egu1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Egu0), [rdb] "=r" (Egu1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");
            print_debug("Egu0", "Egu1", Egu0, Egu1);

            Abe1 ^= De1;
            Abe0 ^= De0;
            print_debug("Abe0", "Abe1", Abe0, Abe1);
            rol32_asm(Abe1, Abe0, 1, &BCa1, &BCa0);

            Agi0 ^= Di0;
            Agi1 ^= Di1;
            print_debug("Agi0", "Agi1", Agi0, Agi1);
            rol32_asm(Agi1, Agi0, 6, &BCe1, &BCe0);

            Ako1 ^= Do1;
            Ako0 ^= Do0;
            print_debug("Ako0", "Ako1", Ako0, Ako1);       
            rol32_asm(Ako1, Ako0, 25, &BCi1, &BCi0);
    
            Amu0 ^= Du0;
            Amu1 ^= Du1;
            print_debug("Amu0", "Amu1", Amu0, Amu1);
            rol32_asm(Amu1, Amu0, 8, &BCo1, &BCo0);

            Asa0 ^= Da0;
            Asa1 ^= Da1;
            print_debug("Asa0", "Asa1", Asa0, Asa1);
            rol32_asm(Asa1, Asa0, 18, &BCu1, &BCu0);

            // Eka0 = BCa0 ^ ((~BCe0) & BCi0);
            // Eka1 = BCa1 ^ ((~BCe1) & BCi1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Eka0), [rdb] "=r" (Eka1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
                [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
                : "cc");
            print_debug("Eka0", "Eka1", Eka0, Eka1);

            // Eke0 = BCe0 ^ ((~BCi0) & BCo0);
            // Eke1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Eke0), [rdb] "=r" (Eke1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");
            print_debug("Eke0", "Eke1", Eke0, Eke1);

            // Eki0 = BCi0 ^ ((~BCo0) & BCu0);
            // Eki1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Eki0), [rdb] "=r" (Eki1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");
            print_debug("Eki0", "Eki1", Eki0, Eki1);

            // Eko0 = BCo0 ^ ((~BCu0) & BCa0);
            // Eko1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Eko0), [rdb] "=r" (Eko1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");
            print_debug("Eko0", "Eko1", Eko0, Eko1);

            // Eku0 = BCu0 ^ ((~BCa0) & BCe0);
            // Eku1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda],  %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb],  %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Eku0), [rdb] "=r" (Eku1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");
            print_debug("Eku0", "Eku1", Eku0, Eku1);
    
            Abu1 ^= Du1;
            Abu0 ^= Du0;
            print_debug("Abu0", "Abu1", Abu0, Abu1);
            rol32_asm(Abu1, Abu0, 27, &BCa1, &BCa0);

            Aga0 ^= Da0;
            Aga1 ^= Da1;
            print_debug("Aga0", "Aga1", Aga0, Aga1);
            rol32_asm(Aga1, Aga0, 36, &BCe1, &BCe0);

            Ake0 ^= De0;
            Ake1 ^= De1;
            print_debug("Ake0", "Ake1", Ake0, Ake1);
            rol32_asm(Ake1, Ake0, 10, &BCi1, &BCi0);

            Ami1 ^= Di1;
            Ami0 ^= Di0;
            print_debug("Ami0", "Ami1", Ami0, Ami1);
            rol32_asm(Ami1, Ami0, 15, &BCo1, &BCo0);

            Aso0 ^= Do0;
            Aso1 ^= Do1;
            print_debug("Aso0", "Aso1", Aso0, Aso1);
            rol32_asm(Aso1, Aso0, 56, &BCu1, &BCu0);

            // Ema0 = BCa0 ^ ((~BCe0) & BCi0);
            // Ema1 = BCa1 ^ ((~BCe1) & BCi1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ema0), [rdb] "=r" (Ema1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
                [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
                : "cc");
            print_debug("Ema0", "Ema1", Ema0, Ema1);   

            // Eme0 = BCe0 ^ ((~BCi0) & BCo0);
            // Eme1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Eme0), [rdb] "=r" (Eme1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");
            print_debug("Eme0", "Eme1", Eme0, Eme1);

            // Emi0 = BCi0 ^ ((~BCo0) & BCu0);
            // Emi1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Emi0), [rdb] "=r" (Emi1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");
            print_debug("Emi0", "Emi1", Emi0, Emi1);

            // Emo0 = BCo0 ^ ((~BCu0) & BCa0);
            // Emo1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Emo0), [rdb] "=r" (Emo1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");
            print_debug("Emo0", "Emo1", Emo0, Emo1);

            // Emu0 = BCu0 ^ ((~BCa0) & BCe0);
            // Emu1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Emu0), [rdb] "=r" (Emu1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");
            print_debug("Emu0", "Emu1", Emu0, Emu1);

    
            Abi0 ^= Di0;
            Abi1 ^= Di1;
            print_debug("Abi0", "Abi1", Abi0, Abi1);
            rol32_asm(Abi1, Abi0, 62, &BCa1, &BCa0);

            Ago1 ^= Do1;
            Ago0 ^= Do0;
            print_debug("Ago0", "Ago1", Ago0, Ago1);
            rol32_asm(Ago1, Ago0, 55, &BCe1, &BCe0);

            Aku1 ^= Du1;
            Aku0 ^= Du0;
            print_debug("Aku0", "Aku1", Aku0, Aku1);
            rol32_asm(Aku1, Aku0, 39, &BCi1, &BCi0);

            Ama1 ^= Da1;
            Ama0 ^= Da0;
            print_debug("Ama0", "Ama1", Ama0, Ama1);
            rol32_asm(Ama1, Ama0, 41, &BCo1, &BCo0);

            Ase0 ^= De0;
            Ase1 ^= De1;
            print_debug("Ase0", "Ase1", Ase0, Ase1);
            rol32_asm(Ase1, Ase0, 2, &BCu1, &BCu0);


            // Esa0 = BCa0 ^ ((~BCe0) & BCi0);
            // Esa1 = BCa1 ^ ((~BCe1) & BCi1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Esa0), [rdb] "=r" (Esa1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
                [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
                : "cc");
            print_debug("Esa0", "Esa1", Esa0, Esa1);

            // Ese0 = BCe0 ^ ((~BCi0) & BCo0);
            // Ese1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ese0), [rdb] "=r" (Ese1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");
            print_debug("Ese0", "Ese1", Ese0, Ese1);

            // Esi0 = BCi0 ^ ((~BCo0) & BCu0);
            // Esi1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Esi0), [rdb] "=r" (Esi1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");
            print_debug("Esi0", "Esi1", Esi0, Esi1);

            // Eso0 = BCo0 ^ ((~BCu0) & BCa0);
            // Eso1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Eso0), [rdb] "=r" (Eso1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");
            print_debug("Eso0", "Eso1", Eso0, Eso1);

            // Esu0 = BCu0 ^ ((~BCa0) & BCe0);
            // Esu1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Esu0), [rdb] "=r" (Esu1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");
            print_debug("Esu0", "Esu1", Esu0, Esu1);

            //    prepareTheta
            BCa0 = Eba0^Ega0^Eka0^Ema0^Esa0;
            BCa1 = Eba1^Ega1^Eka1^Ema1^Esa1;
            print_debug("BCa0", "BCa1", BCa0, BCa1);
            BCe0 = Ebe0^Ege0^Eke0^Eme0^Ese0;
            BCe1 = Ebe1^Ege1^Eke1^Eme1^Ese1;
            print_debug("BCe0", "BCe1", BCe0, BCe1);    
            BCi0 = Ebi0^Egi0^Eki0^Emi0^Esi0;
            BCi1 = Ebi1^Egi1^Eki1^Emi1^Esi1;
            print_debug("BCi0", "BCi1", BCi0, BCi1);    
            BCo0 = Ebo0^Ego0^Eko0^Emo0^Eso0;
            BCo1 = Ebo1^Ego1^Eko1^Emo1^Eso1;
            print_debug("BCo0", "BCo1", BCo0, BCo1);
            BCu0 = Ebu0^Egu0^Eku0^Emu0^Esu0;
            BCu1 = Ebu1^Egu1^Eku1^Emu1^Esu1;
            print_debug("BCu0", "BCu1", BCu0, BCu1);

            //thetaRhoPiChiIota(round+1, E, A)
            rol32_asm(BCe1, BCe0, 1, &BCe0a, &BCe0b);
            Da0 = BCe0b ^ BCu0;
            Da1 = BCe0a ^ BCu1;
            print_debug("Da0", "Da1", Da0, Da1);

            rol32_asm(BCi1, BCi0, 1, &BCi0a, &BCi0b);
            De0 = BCi0b ^ BCa0;
            De1 = BCi0a ^ BCa1;
            print_debug("De0", "De1", De0, De1);

            rol32_asm(BCo1, BCo0, 1, &BCo0a, &BCo0b);
            Di0 = BCo0b ^ BCe0;
            Di1 = BCo0a ^ BCe1;
            print_debug("Di0", "Di1", Di0, Di1);

            rol32_asm(BCu1, BCu0, 1, &BCu0a, &BCu0b);
            Do0 = BCu0b ^ BCi0;
            Do1 = BCu0a ^ BCi1;
            print_debug("Do0", "Do1", Do0, Do1);

            rol32_asm(BCa1, BCa0, 1, &BCa0a, &BCa0b);
            Du0 = BCa0b ^ BCo0;
            Du1 = BCa0a ^ BCo1;
            print_debug("Du0", "Du1", Du0, Du1);

            //Eba0 ^= Da0;
            //Eba1 ^= Da1;
            //print_debug("Eba0", "Eba1", Eba0, Eba1);
            BCa0 = Eba0 ^ Da0;
            BCa1 = Eba1 ^ Da1;
            print_debug("BCa0", "BCa1", BCa0, BCa1);

            Ege0 ^= De0;
            Ege1 ^= De1;
            print_debug("Ege0", "Ege1", Ege0, Ege1);
            rol32_asm(Ege1, Ege0, 44, &BCe1, &BCe0);

            Eki1 ^= Di1;
            Eki0 ^= Di0;
            print_debug("Eki0", "Eki1", Eki0, Eki1);
            rol32_asm(Eki1, Eki0, 43, &BCi1, &BCi0);

            Emo1 ^= Do1;
            Emo0 ^= Do0;
            print_debug("Emo0", "Emo1", Emo0, Emo1);
            rol32_asm(Emo1, Emo0, 21, &BCo1, &BCo0);

            Esu0 ^= Du0;
            Esu1 ^= Du1;
            print_debug("Esu0", "Esu1", Esu0, Esu1);
            rol32_asm(Esu1, Esu0, 14, &BCu1, &BCu0);

            print_debug("Aba0", "Aba1", Aba0, Aba1);
            print_debug("rc2", "rc3", KeccakF1600RoundConstants32[round*2+2], KeccakF1600RoundConstants32[round*2+3]);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, a3,  %[rs1a], %[rs2a], %[rs3a]\n\t"
                "xor %[rda], a3, %[keccak1]\n\t"
                ".insn r 0x6b, 0x01, 0x2, a5,  %[rs1b], %[rs2b], %[rs3b]\n\t"
                "xor %[rdb], a5, %[keccak2]\n\t"
                : [rda] "=r" (Aba0), [rdb] "=r" (Aba1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0), [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1), [keccak1] "r" (KeccakF1600RoundConstants32[round*2+3]), [keccak2] "r" (KeccakF1600RoundConstants32[round*2+2])
                : "a3", "a5", "cc");

            print_debug("Aba0", "Aba1", Aba0, Aba1);
            // Abe0 = BCe0 ^ ((~BCi0) & BCo0);
            // Abe1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Abe0), [rdb] "=r" (Abe1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");
            print_debug("Abe0", "Abe1", Abe0, Abe1);

            // Abi0 = BCi0 ^ ((~BCo0) & BCu0);
            // Abi1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Abi0), [rdb] "=r" (Abi1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");
            print_debug("Abi0", "Abi1", Abi0, Abi1);

            // Abo0 = BCo0 ^ ((~BCu0) & BCa0);
            // Abo1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Abo0), [rdb] "=r" (Abo1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");
            print_debug("Abo0", "Abo1", Abo0, Abo1);

            // Abu0 = BCu0 ^ ((~BCa0) & BCe0);
            // Abu1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Abu0), [rdb] "=r" (Abu1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");
            print_debug("Abu0", "Abu1", Abu0, Abu1);

    
            Ebo0 ^= Do0;
            Ebo1 ^= Do1;
            print_debug("Ebo0", "Ebo1", Ebo0, Ebo1);
            rol32_asm(Ebo1, Ebo0, 28, &BCa1, &BCa0);

            Egu0 ^= Du0;
            Egu1 ^= Du1;
            print_debug("Egu0", "Egu1", Egu0, Egu1);
            rol32_asm(Egu1, Egu0, 20, &BCe1, &BCe0);

            Eka1 ^= Da1;
            Eka0 ^= Da0;
            print_debug("Eka0", "Eka1", Eka0, Eka1);
            rol32_asm(Eka1, Eka0, 3, &BCi1, &BCi0);

            Eme1 ^= De1;
            Eme0 ^= De0;
            print_debug("Eme0", "Eme1", Eme0, Eme1);
            rol32_asm(Eme1, Eme0, 45, &BCo1, &BCo0);

            Esi1 ^= Di1;
            Esi0 ^= Di0;
            print_debug("Esi0", "Esi1", Esi0, Esi1);
            rol32_asm(Esi1, Esi0, 61, &BCu1, &BCu0);

            // Aga0 = BCa0 ^ ((~BCe0) & BCi0);
            // Aga1 = BCa1 ^ ((~BCe1) & BCi1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Aga0), [rdb] "=r" (Aga1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
                [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
                : "cc");
            print_debug("Aga0", "Aga1", Aga0, Aga1);

            // Age0 = BCe0 ^ ((~BCi0) & BCo0);
            // Age1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Age0), [rdb] "=r" (Age1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");
            print_debug("Age0", "Age1", Age0, Age1);

            // Agi0 = BCi0 ^ ((~BCo0) & BCu0);
            // Agi1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Agi0), [rdb] "=r" (Agi1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");
            print_debug("Agi0", "Agi1", Agi0, Agi1);

            // Ago0 = BCo0 ^ ((~BCu0) & BCa0);
            // Ago1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ago0), [rdb] "=r" (Ago1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");
            print_debug("Ago0", "Ago1", Ago0, Ago1);

            // Agu0 = BCu0 ^ ((~BCa0) & BCe0);
            // Agu1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Agu0), [rdb] "=r" (Agu1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");
            print_debug("Agu0", "Agu1", Agu0, Agu1);

            Ebe1 ^= De1;
            Ebe0 ^= De0;
            print_debug("Ebe0", "Ebe1", Ebe0, Ebe1);
            rol32_asm(Ebe1, Ebe0, 1, &BCa1, &BCa0);

            Egi0 ^= Di0;
            Egi1 ^= Di1;
            print_debug("Egi0", "Egi1", Egi0, Egi1);
            rol32_asm(Egi1, Egi0, 6, &BCe1, &BCe0);    

            Eko1 ^= Do1;
            Eko0 ^= Do0;
            print_debug("Eko0", "Eko1", Eko0, Eko1);
            rol32_asm(Eko1, Eko0, 25, &BCi1, &BCi0);

            Emu0 ^= Du0;
            Emu1 ^= Du1;
            print_debug("Emu0", "Emu1", Emu0, Emu1);
            rol32_asm(Emu1, Emu0, 8, &BCo1, &BCo0);

            Esa0 ^= Da0;
            Esa1 ^= Da1;
            print_debug("Esa0", "Esa1", Esa0, Esa1);
            rol32_asm(Esa1, Esa0, 18, &BCu1, &BCu0);

            // Aka0 = BCa0 ^ ((~BCe0) & BCi0);
            // Aka1 = BCa1 ^ ((~BCe1) & BCi1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Aka0), [rdb] "=r" (Aka1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
                [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
                : "cc");
            print_debug("Aka0", "Aka1", Aka0, Aka1);

            // Ake0 = BCe0 ^ ((~BCi0) & BCo0);
            // Ake1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ake0), [rdb] "=r" (Ake1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");
            print_debug("Ake0", "Ake1", Ake0, Ake1);

            // Aki0 = BCi0 ^ ((~BCo0) & BCu0);
            // Aki1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Aki0), [rdb] "=r" (Aki1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");
            print_debug("Aki0", "Aki1", Aki0, Aki1);

            // Ako0 = BCo0 ^ ((~BCu0) & BCa0);
            // Ako1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ako0), [rdb] "=r" (Ako1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");
            print_debug("Ako0", "Ako1", Ako0, Ako1);

            // Aku0 = BCu0 ^ ((~BCa0) & BCe0);
            // Aku1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Aku0), [rdb] "=r" (Aku1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");
            print_debug("Aku0", "Aku1", Aku0, Aku1);


            Ebu1 ^= Du1;
            Ebu0 ^= Du0;
            print_debug("Ebu0", "Ebu1", Ebu0, Ebu1);
            rol32_asm(Ebu1, Ebu0, 27, &BCa1, &BCa0);  

            Ega0 ^= Da0;
            Ega1 ^= Da1;
            print_debug("Ega0", "Ega1", Ega0, Ega1);
            rol32_asm(Ega1, Ega0, 36, &BCe1, &BCe0);    

            Eke0 ^= De0;
            Eke1 ^= De1;
            print_debug("Eke0", "Eke1", Eke0, Eke1);
            rol32_asm(Eke1, Eke0, 10, &BCi1, &BCi0);

            Emi1 ^= Di1;
            Emi0 ^= Di0;
            print_debug("Emi0", "Emi1", Emi0, Emi1);
            rol32_asm(Emi1, Emi0, 15, &BCo1, &BCo0);

            Eso0 ^= Do0;
            Eso1 ^= Do1;
            print_debug("Eso0", "Eso1", Eso0, Eso1);
            rol32_asm(Eso1, Eso0, 56, &BCu1, &BCu0);
        
            // Ama0 = BCa0 ^ ((~BCe0) & BCi0);
            // Ama1 = BCa1 ^ ((~BCe1) & BCi1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ama0), [rdb] "=r" (Ama1)
                : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
                [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
                : "cc");
            print_debug("Ama0", "Ama1", Ama0, Ama1);

            // Ame0 = BCe0 ^ ((~BCi0) & BCo0);
            // Ame1 = BCe1 ^ ((~BCi1) & BCo1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ame0), [rdb] "=r" (Ame1)
                : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
                [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
                : "cc");
            print_debug("Ame0", "Ame1", Ame0, Ame1);

            // Ami0 = BCi0 ^ ((~BCo0) & BCu0);
            // Ami1 = BCi1 ^ ((~BCo1) & BCu1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Ami0), [rdb] "=r" (Ami1)
                : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
                [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
                : "cc");
            print_debug("Ami0", "Ami1", Ami0, Ami1);

            // Amo0 = BCo0 ^ ((~BCu0) & BCa0);
            // Amo1 = BCo1 ^ ((~BCu1) & BCa1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Amo0), [rdb] "=r" (Amo1)
                : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
                [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
                : "cc");
            print_debug("Amo0", "Amo1", Amo0, Amo1);

            // Amu0 = BCu0 ^ ((~BCa0) & BCe0);
            // Amu1 = BCu1 ^ ((~BCa1) & BCe1);
            asm volatile (
                ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
                ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
                : [rda] "=r" (Amu0), [rdb] "=r" (Amu1)
                : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
                [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
                : "cc");
            print_debug("Amu0", "Amu1", Amu0, Amu1);


            Ebi0 ^= Di0;
            Ebi1 ^= Di1;
            print_debug("Ebi0", "Ebi1", Ebi0, Ebi1);
            rol32_asm(Ebi1, Ebi0, 62, &BCa1, &BCa0);

            Ego1 ^= Do1;
            Ego0 ^= Do0;
            print_debug("Ego0", "Ego1", Ego0, Ego1);
            rol32_asm(Ego1, Ego0, 55, &BCe1, &BCe0);

            Eku1 ^= Du1;
            Eku0 ^= Du0;
            print_debug("Eku0", "Eku1", Eku0, Eku1);
            rol32_asm(Eku1, Eku0, 39, &BCi1, &BCi0);

            Ema1 ^= Da1;
            Ema0 ^= Da0;
            print_debug("Ema0", "Ema1", Ema0, Ema1);
            rol32_asm(Ema1, Ema0, 41, &BCo1, &BCo0);

            Ese0 ^= De0;
            Ese1 ^= De1;
            print_debug("Ese0", "Ese1", Ese0, Ese1);
            rol32_asm(Ese1, Ese0, 2, &BCu1, &BCu0);

        // Asa0 = BCa0 ^ ((~BCe0) & BCi0);
        // Asa1 = BCa1 ^ ((~BCe1) & BCi1);
        asm volatile (
            ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
            ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
            : [rda] "=r" (Asa0), [rdb] "=r" (Asa1)
            : [rs1a] "r" (BCa0), [rs2a] "r" (BCe0), [rs3a] "r" (BCi0),
            [rs1b] "r" (BCa1), [rs2b] "r" (BCe1), [rs3b] "r" (BCi1)
            : "cc");
        print_debug("Asa0", "Asa1", Asa0, Asa1);

        // Ase0 = BCe0 ^ ((~BCi0) & BCo0);
        // Ase1 = BCe1 ^ ((~BCi1) & BCo1);
        asm volatile (
            ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
            ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
            : [rda] "=r" (Ase0), [rdb] "=r" (Ase1)
            : [rs1a] "r" (BCe0), [rs2a] "r" (BCi0), [rs3a] "r" (BCo0),
            [rs1b] "r" (BCe1), [rs2b] "r" (BCi1), [rs3b] "r" (BCo1)
            : "cc");
        print_debug("Ase0", "Ase1", Ase0, Ase1);

        // Asi0 = BCi0 ^ ((~BCo0) & BCu0);
        // Asi1 = BCi1 ^ ((~BCo1) & BCu1);
        asm volatile (
            ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
            ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
            : [rda] "=r" (Asi0), [rdb] "=r" (Asi1)
            : [rs1a] "r" (BCi0), [rs2a] "r" (BCo0), [rs3a] "r" (BCu0),
            [rs1b] "r" (BCi1), [rs2b] "r" (BCo1), [rs3b] "r" (BCu1)
            : "cc");
        print_debug("Asi0", "Asi1", Asi0, Asi1);

        // Aso0 = BCo0 ^ ((~BCu0) & BCa0);
        // Aso1 = BCo1 ^ ((~BCu1) & BCa1);
        asm volatile (
            ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
            ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
            : [rda] "=r" (Aso0), [rdb] "=r" (Aso1)
            : [rs1a] "r" (BCo0), [rs2a] "r" (BCu0), [rs3a] "r" (BCa0),
            [rs1b] "r" (BCo1), [rs2b] "r" (BCu1), [rs3b] "r" (BCa1)
            : "cc");
        print_debug("Aso0", "Aso1", Aso0, Aso1);

        // Asu0 = BCu0 ^ ((~BCa0) & BCe0);
        // Asu1 = BCu1 ^ ((~BCa1) & BCe1);
        asm volatile (
            ".insn r 0x6b, 0x01, 0x2, %[rda], %[rs1a], %[rs2a], %[rs3a]\n\t"
            ".insn r 0x6b, 0x01, 0x2, %[rdb], %[rs1b], %[rs2b], %[rs3b]\n\t"
            : [rda] "=r" (Asu0), [rdb] "=r" (Asu1)
            : [rs1a] "r" (BCu0), [rs2a] "r" (BCa0), [rs3a] "r" (BCe0),
            [rs1b] "r" (BCu1), [rs2b] "r" (BCa1), [rs3b] "r" (BCe1)
            : "cc");
        print_debug("Asu0", "Asu1", Asu0, Asu1);
            
        }

        //copyToState(state, A)
        state[ 0] = Aba0;
        state[ 1] = Aba1;
        state[ 2] = Abe0;
        state[ 3] = Abe1;
        state[ 4] = Abi0;
        state[ 5] = Abi1;
        state[ 6] = Abo0;
        state[ 7] = Abo1;
        state[ 8] = Abu0;
        state[ 9] = Abu1;
        state[10] = Aga0;
        state[11] = Aga1;
        state[12] = Age0;
        state[13] = Age1;
        state[14] = Agi0;
        state[15] = Agi1;
        state[16] = Ago0;
        state[17] = Ago1;
        state[18] = Agu0;
        state[19] = Agu1;
        state[20] = Aka0;
        state[21] = Aka1;
        state[22] = Ake0;
        state[23] = Ake1;
        state[24] = Aki0;
        state[25] = Aki1;
        state[26] = Ako0;
        state[27] = Ako1;
        state[28] = Aku0;
        state[29] = Aku1;
        state[30] = Ama0;
        state[31] = Ama1;
        state[32] = Ame0;
        state[33] = Ame1;
        state[34] = Ami0;
        state[35] = Ami1;
        state[36] = Amo0;
        state[37] = Amo1;
        state[38] = Amu0;
        state[39] = Amu1;
        state[40] = Asa0;
        state[41] = Asa1;
        state[42] = Ase0;
        state[43] = Ase1;
        state[44] = Asi0;
        state[45] = Asi1;
        state[46] = Aso0;
        state[47] = Aso1;
        state[48] = Asu0;
        state[49] = Asu1;

}