#include "bush.h"
#include "drawing/canvas.h"
#include <stdlib.h>

static void initialize_bush(Bush *bs) {
  bs->food = 100;
  bs->pos.x = rand() % W;
  bs->pos.y = rand() % H;
}

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

  for (int i = 0; i < size; i++)
    initialize_bush(&list->bushes[i]);

  return list;
}

void destroy_bushes(BushList *list) {
  if (!list)
    return;
  free(list->bushes);
  free(list);
}

static void destroy_bush(BushList *list, int ix) {
  // TODO
  return;
}

static void process_bush(BushList *bs, int ix) {

  if (bs->bushes[ix].food <= 0) {
    destroy_bush(bs, ix);
  }
}

void update_bushes(BushList *list) {
  int n = list->bush_count;

  for (int i = 0; i < n; i++)
    process_bush(list, i);
}

void draw_bushes(BushList *list) {
  for (int i = 0; i < list->bush_count; i++)
    draw_pixel(list->bushes[i].pos.x, list->bushes[i].pos.y, GREEN);
}

// Gets all the bushes in the map
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
