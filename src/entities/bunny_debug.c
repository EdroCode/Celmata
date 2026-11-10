#include "bunny_internal.h"
#include <stdio.h>

void print_summary(const BunnyList *list) {
  int alive = 0;
  float energy = 0;

  for (int i = 0; i < list->bunny_count; i++) {
    if (!list->bunnies[i].alive)
      continue;
    alive++;
    energy += list->bunnies[i].energy;
  }
  printf("alive=%d avg_energy=%.1f\n", alive, alive ? energy / alive : 0.0f);
}
void print_bunnies(const BunnyList *list) {
  for (int i = 0; i < list->bunny_count; i++) {
    const Bunny *b = &list->bunnies[i];

    printf("#%-3d %s pos=(%3d,%3d) energy=%6.1f age=%3d "
           "vision=%.2f speed=%.2f fear=%.2f herd=%.2f\n",
           i, b->alive ? "alive" : "dead ", b->pos.x, b->pos.y, b->energy,
           b->age, b->genes.vision, b->genes.speed, b->genes.fear,
           b->genes.herd);
  }
}

void print_bunny_count(BunnyList *b_list) {
  printf("Bunny Count: %d \n", b_list->bunny_count);
}