#include "types.h"

typedef struct Wolf {
  // dynamic state
  int age;
  int hp;
  int alive;
  int repro_cooldown;
  float move_budget;
  float energy;

  // inheritable
  Genes genes;

  // position
  Position pos;
} Wolf;

struct WolfList {
  Wolf *wolf_pack;
  int wolf_count;
  int wolf_capacity;
};