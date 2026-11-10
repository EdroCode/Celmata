#include "types.h"

typedef struct Bush {
  Position pos;
} Bush;

typedef struct BushList {
  Bush *bushes;
  int bush_count;
  int bush_capacity;
} BushList;

struct BushList *create_bushes(int size);

void update_bushes(BushList *list);

void draw_bushes(BushList *list);