#include "bunny.h"

typedef struct {
  int x, y;
} Position;

// Inheritable traits, shared by bunny and wolf so mutation code is reusable
typedef struct Genes {
  float vision; // float, not int: round only when scanning the grid
  float speed;
  float fear;
  float herd;
} Genes;

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

int check_cell(struct BunnyList *list, const Position *p);

int check_surroundings(struct BunnyList *list, const Position *p, int self,
                       int vision, int *out);

int bunny_check_empty_surroundings(const BunnyList *list, const Position *p,
                                   int radius, Position *out, int max_out);