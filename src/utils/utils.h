#include "types.h"
#include <stddef.h>

void shuffle(int *a, int n);

// Writes the indices of pts[] within `radius` of p to out[].
// Returns how many were written (at most max_out).
int find_within(const Position *pts, int n, const Position *p, int radius,
                int *out, int max_out);