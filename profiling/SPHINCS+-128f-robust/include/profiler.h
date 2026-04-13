#ifndef PQC_PROFILER_H__
#define PQC_PROFILER_H__

#include <stdint.h>
#include <stdio.h>

void pqc_profile_hit(const char *name);
void pqc_profile_reset(void);
void pqc_profile_dump(FILE *out);

#define PQC_PROFILE_FUNCTION() pqc_profile_hit(__func__)

#endif
