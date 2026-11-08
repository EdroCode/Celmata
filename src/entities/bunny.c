#include "src/bunny.h"
#include "src/canvas.h"
#include "src/utils/shuffle.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * BUNNY RULES v3
 *
 * STATE:    energy, age, repro_cooldown, move_budget, alive
 * GENES:    vision, speed, fear, herd   (inherited, mutated)
 * CONSTANTS (starting values):
 *   MAX_ENERGY = 100     NEWBORN_ENERGY = 25    MAX_AGE = 150
 *   BASE_COST = 0.5      MATURITY = 20          EAT_GAIN = 10
 *   BASE_P = 0.3         REPRO_COST = 30        REPRO_COOLDOWN = 10
 *   IDEAL_DENSITY = 0.4  NOISE = 0.2
 *
 * 0. ORDER
 *    - Shuffle the update order every tick (no one gets a permanent advantage)
 * DONE
 *    - Skip bunnies with alive == false
 *
 * 1. PERCEPTION  (radius = roundf(genes.vision), scanned once per bunny)
 *    - wolves in radius        -> distance to each
 *    - bushes with food > 0    -> distance to each
 *    - other bunnies           -> distance to each
 *    - local_density = bunnies_in_radius / cells_in_radius
 *    - Anything outside the radius does not exist for this bunny
 *
 * 2. EAT  (grazing takes the whole turn)
 *    - If a bush with food is adjacent AND no wolf is within radius:
 *        amount = min(EAT_GAIN, bush.food, MAX_ENERGY - energy)
 *        energy += amount; bush.food -= amount
 *        skip movement this tick
 *    - If a wolf is visible, the bunny doesn't graze, it moves (step 3)
 *
 * 3. MOVE  (utility scoring, no hard if/else priorities)
 *    move_budget += genes.speed
 *    while move_budget >= 1:
 *        hunger = 1 - energy / MAX_ENERGY             // 0 full, 1 starving
 *        for each FREE neighbor cell c:
 *            score(c) =  1.0         * hunger * bush_pull(c)    // seek food
 *                     -  genes.fear  * wolf_threat(c)           // flee
 *                     +  genes.herd  * (IDEAL_DENSITY - density(c)) *
 * bunny_pull(c)
 *                     +  NOISE * random(0..1)
 *        move to the highest score; move_budget -= 1; moved = true
 *    where pull/threat = sum over visible things of 1 / (distance + 1)
 *    - The herd term flips sign: bunnies group up in open space and
 *      spread out when it is crowded
 *    - A hungry bunny is driven by food, a full one by fear and herd
 *
 * 4. REPRODUCTION
 *    requires: age >= MATURITY, energy > 70% of MAX_ENERGY,
 *              repro_cooldown == 0, an adult bunny adjacent,
 *              a free cell adjacent
 *    chance  = BASE_P * (1 - local_density)       // crowding lowers births
 *    on success:
 *        energy -= REPRO_COST
 *        child.energy = NEWBORN_ENERGY            // births lose energy overall
 *        child.genes  = mutate(parent.genes)
 *        child spawns in a random free adjacent cell
 *        repro_cooldown = REPRO_COOLDOWN
 *
 * 5. METABOLISM  (end of tick, every trait has a price)
 *    energy -= BASE_COST
 *            + 0.15 * genes.vision
 *            + (moved ? 0.2 * genes.speed^2 : 0)  // sprinting is quadratic
 *    age++; repro_cooldown-- (min 0)
 *    dies if energy <= 0 or age > MAX_AGE
 *    (optional) on death: adjacent bushes gain fertility -> bunnies feed the
 * grass
 *
 * 6. WOLF ENCOUNTER  (resolved on the WOLF's turn, not here)
 *    escape = clamp(0.1 + 0.2 * genes.speed + (adjacent_to_bush ? 0.3 : 0), 0,
 * 0.9) attack_success = wolf.hunger * (1 - escape)  // wolf.hunger = 1 -
 * wolf.energy / MAX success -> bunny.alive = false, wolf.energy += bunny_value
 */

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

struct BunnyList {
  Bunny *bunnies;
  int bunny_count;
};

static int check_cell(const BunnyList *list, const Position *p) {
  for (int i = 0; i < list->bunny_count; i++) {
    const Bunny *b = &list->bunnies[i];

    if (b->alive && b->pos.x == p->x && b->pos.y == p->y)
      return 1;
  }
  return 0;
}

static int check_surroundings(const BunnyList *list, const Position *p,
                              int self, int vision, int *out) {
  int count = 0;

  for (int i = 0; i < list->bunny_count; i++) {
    const Bunny *b = &list->bunnies[i];

    if (i == self || !b->alive)
      continue;

    int dx = abs(p->x - b->pos.x);
    int dy = abs(p->y - b->pos.y);

    if (dx * dx + dy * dy <= vision * vision)
      out[count++] = i;
  }
  return count;
}

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

static void initialize_bunny(Bunny *b) {
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

BunnyList *create_bunnies(int size) {
  if (size <= 0)
    return NULL;

  BunnyList *list = malloc(sizeof(BunnyList));
  if (!list)
    return NULL;

  list->bunnies = malloc(size * sizeof(Bunny));
  if (!list->bunnies) {
    free(list);
    return NULL;
  }

  list->bunny_count = size;
  for (int i = 0; i < size; i++)
    initialize_bunny(&list->bunnies[i]);

  return list;
}

void destroy_bunnies(BunnyList *list) {
  if (!list)
    return;
  free(list->bunnies);
  free(list);
}

static void metabolism(Bunny *b) {
  b->energy -=
      0.5f + 0.15f * b->genes.vision + 0.2f * b->genes.speed * b->genes.speed;
  b->age++;
  if (b->energy <= 0 || b->age > 150)
    b->alive = 0;
}

static void process_bunny(Bunny *b, const BunnyList *list) {
  if (!b->alive)
    return;

  for (int attempt = 0; attempt < 4; attempt++) {
    int dx = rand() % 3 - 1;
    int dy = rand() % 3 - 1;
    if (dx == 0 && dy == 0)
      continue;

    Position target = {(b->pos.x + dx + W) % W, (b->pos.y + dy + H) % H};

    if (!check_cell(list, &target)) {
      b->pos = target;
      break;
    }
  }

  metabolism(b);
}

void update_bunnies(BunnyList *list) {
  int n = list->bunny_count;

  int *order = malloc(n * sizeof(int));
  if (!order)
    return;

  for (int i = 0; i < n; i++)
    order[i] = i;

  shuffle(order, n);

  for (int i = 0; i < n; i++)
    process_bunny(&list->bunnies[order[i]], list);

  free(order);
}

void draw_bunnies(const BunnyList *list) {
  for (int i = 0; i < list->bunny_count; i++)
    if (list->bunnies[i].alive)
      draw_pixel(list->bunnies[i].pos.x, list->bunnies[i].pos.y, WHITE);
}