#include "bunny_internal.h"
#include "drawing/canvas.h"
#include "utils/utils.h"
#include <stdlib.h>

int check_cell(BunnyList *list, const Position *p) {
  for (int i = 0; i < list->bunny_count; i++) {
    const Bunny *b = &list->bunnies[i];

    if (b->alive && b->pos.x == p->x && b->pos.y == p->y)
      return 1;
  }
  return 0;
}

int check_surroundings(BunnyList *list, const Position *p, int self, int vision,
                       int *out) {
  int count = 0;

  for (int i = 0; i < list->bunny_count; i++) {
    const Bunny *b = &list->bunnies[i];

    if (i == self || !b->alive)
      continue;

    int dx = abs(p->x - b->pos.x);
    int dy = abs(p->y - b->pos.y);

    if (dx * dx + dy * dy <= vision * vision)
      out[count++] = i;
  }
  return count;
}

int check_bush_surroundings(void *bs_list, Position *p, int self, int vision,
                            int *out) {

  Position ps[8]; // temp, see 8 below
  int ix[8];
  int n = 0;
  int psIx[8];

  bush_available(bs_list, ps, ix, 8);

  n = find_within(ps, 8, p, vision, psIx, 8);

  return n;
}

int bunny_check_empty_surroundings(BunnyList *list, const Position *p,
                                   int radius, Position *out, int max_out) {
  int count = 0;

  for (int dy = -radius; dy <= radius; dy++) {
    for (int dx = -radius; dx <= radius; dx++) {
      Position cell = {p->x + dx, p->y + dy};

      if (dx == 0 && dy == 0)
        continue;
      if (cell.x < 0 || cell.x >= W || cell.y < 0 || cell.y >= H)
        continue;
      if (check_cell(list, &cell))
        continue;
      if (count < max_out)
        out[count++] = cell;
    }
  }
  return count;
}
