#include "bush.h"
#include "drawing/canvas.h"
#include <stdlib.h>

static void initialize_bush(Bush *bs) {

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

void process_bush(Bush *bs) { return; }

void update_bushes(BushList *list) {
  int n = list->bush_count;

  for (int i = 0; i < n; i++)
    process_bush(&list->bushes[i]);
}

void draw_bushes(BushList *list) {
  for (int i = 0; i < list->bush_count; i++)
    draw_pixel(list->bushes[i].pos.x, list->bushes[i].pos.y, GREEN);
}