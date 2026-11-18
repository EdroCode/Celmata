#ifndef TYPES_H
#define TYPES_H

typedef struct Position {
  int x, y;
} Position;

// Inheritable traits, shared by bunny and wolf so mutation code is reusable
typedef struct Genes {
  float vision; // float, not int: round only when scanning the grid
  float speed;
  float fear;
  float herd;
} Genes;

#define W 60
#define H 60
#define SCALE 12

#define INIT_BUNNY_COUNT 200
#define INIT_BUSH_COUNT 40
#define INIT_WOLF_COUNT 1

#define GENERATION_TYPE 1 // 1 - Random 2 - Cluster

#define GAME_SPEED 0.1f // default 0.1

#endif
