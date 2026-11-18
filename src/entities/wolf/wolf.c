#include "wolf.h"
#include "drawing/canvas.h"
#include "utils/utils.h"
#include "wolf_internal.h"
#include <stdlib.h>

Wolf *add_new_Wolf(WolfList *list, Position pos, Genes genes, float energy) {
  int slot = -1;

  for (int i = 0; i < list->wolf_count; i++) {
    if (!list->wolf_pack[i].alive) {
      slot = i;
      break;
    }
  }

  if (slot < 0) {
    if (list->wolf_count >= list->wolf_capacity)
      return NULL;
    slot = list->wolf_count++;
  }

  Wolf *b = &list->wolf_pack[slot];
  *b = (Wolf){
      .alive = 1, .hp = 100, .energy = energy, .genes = genes, .pos = pos};
  return b;
}

static void initialize_Wolf(Wolf *b) {
  b->alive = 1;
  b->energy = 100;
  b->hp = 100;
  b->age = 0;
  b->repro_cooldown = 0;
  b->move_budget = 0;
  b->genes = (Genes){.vision = 1, .speed = 1, .fear = 1, .herd = 1};

  b->pos.x = rand() % W;
  b->pos.y = rand() % H;
}

struct WolfList *create_wolf_pack(int size) {
  int capacity = W * H;

  if (size <= 0 || size > capacity)
    return NULL;

  struct WolfList *list = malloc(sizeof(struct WolfList));
  if (!list)
    return NULL;

  list->wolf_pack = malloc(capacity * sizeof(Wolf));
  if (!list->wolf_pack) {
    free(list);
    return NULL;
  }

  list->wolf_count = size;
  list->wolf_capacity = capacity;

  for (int i = 0; i < size; i++)
    initialize_Wolf(&list->wolf_pack[i]);

  return list;
}

void destroy_wolf_pack(WolfList *list) {
  if (!list)
    return;
  free(list->wolf_pack);
  free(list);
}

static void process_wolf(int bunny_id, WolfList *list) {
  Wolf *w = &list->wolf_pack[bunny_id];

  if (!w->alive)
    return;
}

void update_wolfs(WolfList *list) {
  int n = list->wolf_count;

  int *order = malloc(n * sizeof(int));
  if (!order)
    return;

  for (int i = 0; i < n; i++)
    order[i] = i;

  shuffle(order, n);

  for (int i = 0; i < n; i++)
    process_wolf(order[i], list);

  free(order);
}

void draw_wolfs(const WolfList *list) {
  for (int i = 0; i < list->wolf_count; i++)
    if (list->wolf_pack[i].alive)
      draw_pixel(list->wolf_pack[i].pos.x, list->wolf_pack[i].pos.y, RED);
}

int get_alive_wolves_count(const WolfList *list) {
  if (!list)
    return 0;

  int alive = 0;
  for (int i = 0; i < list->wolf_count; i++) {
    if (list->wolf_pack[i].alive)
      alive++;
  }
  return alive;
}