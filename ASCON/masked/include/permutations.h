#ifndef PERMUTATIONS_H_
#define PERMUTATIONS_H_

#include "ascon.h"
#include "api.h"
#include "round.h"
#include "word.h"


static inline void P1(state_t* s, int nr) { PROUNDS(s, nr, 1); }
static inline void P2(state_t* s, int nr) { PROUNDS(s, nr, 2); }
static inline void P3(state_t* s, int nr) { PROUNDS(s, nr, 3); }
static inline void P4(state_t* s, int nr) { PROUNDS(s, nr, 4); }

static inline void P(state_t* s, int nr, int ns) {
  if (ns == 1) P1(s, nr);
  if (ns == 2) P2(s, nr);
  if (ns == 3) P3(s, nr);
  if (ns == 4) P4(s, nr);
}

#endif /* PERMUTATIONS_H_ */
