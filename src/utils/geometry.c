#include "types.h"
#include <stdlib.h>

int find_within(const Position *pts, int n, const Position *p, int radius,
                int *out, int max_out) {
  int count = 0;

  for (int i = 0; i < n && count < max_out; i++) {
    int dx = abs(p->x - pts[i].x);
    int dy = abs(p->y - pts[i].y);

    if (dx * dx + dy * dy <= radius * radius)
      out[count++] = i;
  }
  return count;
}