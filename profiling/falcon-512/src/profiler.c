#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "profiler.h"

#define PQC_PROFILE_MAX_COUNTERS 1024

typedef struct {
    const char *name;
    uint64_t calls;
} pqc_profile_entry;

static pqc_profile_entry pqc_profile_entries[PQC_PROFILE_MAX_COUNTERS];
static size_t pqc_profile_entry_count;

void
pqc_profile_hit(const char *name)
{
    size_t u;

    for (u = 0; u < pqc_profile_entry_count; u++) {
        if (strcmp(pqc_profile_entries[u].name, name) == 0) {
            pqc_profile_entries[u].calls++;
            return;
        }
    }
    if (pqc_profile_entry_count >= PQC_PROFILE_MAX_COUNTERS) {
        return;
    }
    pqc_profile_entries[pqc_profile_entry_count].name = name;
    pqc_profile_entries[pqc_profile_entry_count].calls = 1;
    pqc_profile_entry_count++;
}

void
pqc_profile_reset(void)
{
    size_t u;

    for (u = 0; u < pqc_profile_entry_count; u++) {
        pqc_profile_entries[u].calls = 0;
    }
}

void
pqc_profile_dump(FILE *out)
{
    size_t order[PQC_PROFILE_MAX_COUNTERS];
    size_t count, u, v;

    if (out == NULL) {
        return;
    }
    count = 0;
    for (u = 0; u < pqc_profile_entry_count; u++) {
        if (pqc_profile_entries[u].calls == 0) {
            continue;
        }
        order[count++] = u;
    }
    for (u = 0; u < count; u++) {
        size_t best = u;
        for (v = u + 1; v < count; v++) {
            uint64_t calls_best = pqc_profile_entries[order[best]].calls;
            uint64_t calls_cur = pqc_profile_entries[order[v]].calls;
            int name_cmp = strcmp(
                pqc_profile_entries[order[v]].name,
                pqc_profile_entries[order[best]].name);
            if (calls_cur > calls_best || (calls_cur == calls_best && name_cmp < 0)) {
                best = v;
            }
        }
        if (best != u) {
            size_t tmp = order[u];
            order[u] = order[best];
            order[best] = tmp;
        }
    }
    if (count == 0) {
        fprintf(out, "  no function calls recorded\n");
        return;
    }
    for (u = 0; u < count; u++) {
        fprintf(out, "  %s: %" PRIu64 "\n",
            pqc_profile_entries[order[u]].name,
            pqc_profile_entries[order[u]].calls);
    }
}
