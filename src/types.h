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

#define W 100
#define H 100
#define SCALE 20

#define INIT_BUNNY_COUNT 400
#define INIT_BUSH_COUNT 150
#define INIT_WOLF_COUNT 50

#define GAME_SPEED 0.1f // default 0.1

#endif