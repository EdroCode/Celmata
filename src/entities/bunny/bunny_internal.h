#include "bunny.h"
#include "types.h"

typedef struct Bunny {
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
} Bunny;

// The real body of the type declared in bunny.h (no typedef here)
struct BunnyList {
  Bunny *bunnies;
  int bunny_count;
  int bunny_capacity;
};

int check_cell(BunnyList *list, const Position *p);

int check_surroundings(BunnyList *list, const Position *p, int self, int vision,
                       int *out);

int bunny_check_empty_surroundings(BunnyList *list, const Position *p,
                                   int radius, Position *out, int max_out);