#ifndef ASM_UTILS_H_
#define ASM_UTILS_H_

#include <stdint.h>
//#include "word.h"
#include "shares.h"

#define RND(rnd) randombytes((unsigned char*)&rnd, 4)

static inline uint32_t ROR32(uint32_t x, int n) {
  return x >> n | x << (-n & 31);
}

static inline uint32_t ROT(uint32_t i){
	return (((i) * (ASCON_ROR_SHARES)) & 31);
}

 static inline uint64_t ROR64(uint64_t x, int n) {
  return x >> n | x << (-n & 63);
}
// Funzione per caricare un valore da memoria con offset
static inline void LDR(uint32_t* rd, uint32_t* ptr, int32_t offset) {
    *rd = *(ptr + offset);
}

// Funzione per memorizzare un valore in memoria con offset
static inline void STR(uint32_t rd, uint32_t *ptr, int32_t offset) {
	//chess_separator();
    *(ptr + offset) = rd;
}

// Funzione per azzerare una variabile (inutile in C, ma mantenuta per compatibilità)
static inline void CLEAR() {
    uint32_t r, i = 0;
	r=i;
}

// Funzione per assegnare un valore immediato a una variabile
static inline void MOVI(uint32_t *rd, uint32_t imm) {
    *rd = imm;
}

// Funzione per eseguire un XOR seguito da una rotazione a destra
static inline void EOR_ROR(uint32_t *rd, uint32_t rn, uint32_t rm, uint32_t imm) {
    //*rd = rn ^ ((rm >> imm) | (rm << (-imm & 31)));
	*rd = rn ^ ROR32(rm,imm);

}

// Funzione per eseguire un AND seguito da un XOR con rotazione a destra
static inline void EOR_AND_ROR(uint32_t *ce, uint32_t ae, uint32_t be, uint32_t imm, uint32_t *tmp) {
	uint32_t rot_imm;
	rot_imm = ROT(imm);
    //*tmp = ae & ((be >> rot_imm) | (be << (-rot_imm & 31)));
	*tmp = ae & ROR32(be, rot_imm);
    *ce ^= *tmp;
}

// Funzione per eseguire un BIC seguito da un XOR con rotazione a destra
static inline void EOR_BIC_ROR(uint32_t *ce, uint32_t ae, uint32_t be, uint32_t imm, uint32_t *tmp) {
	uint32_t rot_imm;
	rot_imm = ROT(imm);
    //*tmp = ae & ~((be >> rot_imm) | (be << (-rot_imm & 31)));
	*tmp = ae & ~(ROR32(be,rot_imm));
    *ce ^= *tmp;
}

// Funzione per eseguire un ORR seguito da un XOR con rotazione a destra
static inline void EOR_ORR_ROR(uint32_t *ce, uint32_t ae, uint32_t be, uint32_t imm, uint32_t *tmp) {
	uint32_t rot_imm;
	rot_imm = ROT(imm);
    //*tmp = ae | ((be >> rot_imm) | (be << (-rot_imm & 31)));
	*tmp = ae | ROR32(be,rot_imm);
    *ce ^= *tmp;
	}


#endif  // ASM_UTILS_H_
