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

#define W 55
#define H 50
#define SCALE 10