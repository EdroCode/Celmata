#include "types.h"

/** Bush entity with position and food amount */
typedef struct Bush {
  Position pos;   /**< Position of the bush */
  int food;       /**< Current food units */
} Bush;

/** Dynamic array of bushes */
typedef struct BushList {
  Bush *bushes;           /**< Pointer to bush entries */
  int bush_count;        /**< Number of active bushes */
  int bush_capacity;     /**< Allocated capacity */
} BushList;

/** Create a BushList with `size` initial bushes */
struct BushList *create_bushes(int size);

/** Update all bushes (e.g., decay food) */
void update_bushes(BushList *list);

/** Render all bushes onto the canvas */
void draw_bushes(BushList *list);

/** Retrieve up to `max_out` available bushes; fills `pos` and `ids`. Returns count */
int bush_available(BushList *list, Position *pos, int *ids, int max_out);
