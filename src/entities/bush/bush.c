#include "bush.h"
#include "drawing/canvas.h"
#include "utils/utils.h"
#include <stdio.h>
#include <stdlib.h>

/** Check if a bush exists at position \p */
int bush_in_p(BushList *bs_list, Position p) {
  for (int i = 0; i < bs_list->bush_count; i++) {
    if (bs_list->bushes[i].pos.x == p.x && bs_list->bushes[i].pos.y == p.y)
      return 1;
  }
  return 0;
}

/** Initialise a single bush with random food amount */
static void initialize_bush(Bush *bs, Position *p) {
  bs->food = min_int(100, rand() % 61 + 40); // TODO
  bs->pos.x = p->x;
  bs->pos.y = p->y;
}

/** Generate clustered bushes.
 * @param bs_list Target list
 * @param initial_seeds Number of seed bushes
 * @param radius Radius for neighbour search
 * @param growing_scale Number of growth iterations
 * @param chance Probability (0-100) to place a new bush
 */
static void cluster_gen(BushList *bs_list, int initial_seeds, int radius,
                        int growing_scale, int chance) {
  int count = 0;
  int max_cap = bs_list->bush_capacity;
  bs_list->bush_count = 0;

  for (int i = 0; i < initial_seeds && count < max_cap; i++) {
    Position p = {rand() % W, rand() % H};
    if (!bush_in_p(bs_list, p)) {
      initialize_bush(&bs_list->bushes[count++], &p);
      bs_list->bush_count = count;
    }
  }

  Position *pos = malloc(sizeof(Position) * W * H);
  if (!pos)
    return;
  get_canvas_positions(pos);

  for (int j = 0; j < growing_scale && count < max_cap; j++) {
    int current_bushes = bs_list->bush_count;
    for (int k = 0; k < current_bushes && count < max_cap; k++) {
      int out[32];
      int found = find_within(pos, W * H, &bs_list->bushes[k].pos, radius, out, 32);
      for (int i = 0; i < found && count < max_cap; i++) {
        Position target = pos[out[i]];
        if (!bush_in_p(bs_list, target) && (rand() % 100 < chance)) {
          initialize_bush(&bs_list->bushes[count++], &target);
          bs_list->bush_count = count;
        }
      }
    }
  }

  free(pos);
}

/** Generate random independent bushes */
static void random_gen(BushList *bs_list) {
  for (int i = 0; i < bs_list->bush_count; i++) {
    Position p = {rand() % W, rand() % H};
    while (bush_in_p(bs_list, p)) {
      p.x = rand() % W;
      p.y = rand() % H;
    }
    initialize_bush(&bs_list->bushes[i], &p);
  }
}

/** Dispatch bush generation based on type */
static void bush_generator(BushList *bs_list, int type) {
  switch (type) {
  case 1:
    random_gen(bs_list);
    break;
  case 2:
    cluster_gen(bs_list, bs_list->bush_count, 2, 3, 40);
    break;
  default:
    printf("Invalid bush generation type selected");
  }
}

/** Create a BushList with given initial size */
BushList *create_bushes(int size) {
  int capacity = W * H;
  if (size <= 0 || size > capacity)
    return NULL;

  struct BushList *list = malloc(sizeof(struct BushList));
  if (!list)
    return NULL;

  list->bushes = malloc(capacity * sizeof(Bush));
  if (!list->bushes) {
    free(list);
    return NULL;
  }

  list->bush_count = size;
  list->bush_capacity = capacity;

  bush_generator(list, GENERATION_TYPE);
  return list;
}

/** Free all resources of a BushList */
void destroy_bushes(BushList *list) {
  if (!list)
    return;
  free(list->bushes);
  free(list);
}

/** Placeholder for destroying a single bush (not implemented) */
static void destroy_bush(BushList *list, int ix) {
  // TODO: implement removal and memory compaction
  (void)list; (void)ix;
}

/** Process a bush (e.g., food depletion) */
static void process_bush(BushList *bs, int ix) {
  if (bs->bushes[ix].food <= 0) {
    destroy_bush(bs, ix);
  }
}

/** Update all bushes each tick */
void update_bushes(BushList *list) {
  int n = list->bush_count;
  for (int i = 0; i < n; i++)
    process_bush(list, i);
}

/** Draw all bushes with positive food */
void draw_bushes(BushList *list) {
  for (int i = 0; i < list->bush_count && list->bushes[i].food > 0; i++)
    draw_pixel(list->bushes[i].pos.x, list->bushes[i].pos.y, GREEN);
}

/** Get available bushes up to `max_out`.
 * Returns number of entries written to `pos` and `ids`.
 */
int bush_available(BushList *list, Position *pos, int *ids, int max_out) {
  int count = 0;
  for (int i = 0; i < list->bush_count && i < max_out; i++) {
    if (list->bushes[i].food > 0) {
      pos[count] = list->bushes[i].pos;
      ids[count] = i;
      count++;
    }
  }
  return count;
}
