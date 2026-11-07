#include "src/canvas.h"
#include "src/utils/shuffle.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

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
 *    - Shuffle the bunny array every tick (no one gets a permanent advantage)
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
 *    dies if energy <= 0 or age > MAX_AGE#include "src/utils/shuffle.h"
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
  float vision; // float, not int: see note below
  float speed;
  float fear;
  float herd;
} Genes;

typedef struct Bunny {
  // dynamic state
  float energy;
  int age;
  int repro_cooldown;
  float move_budget;
  int alive;

  // inheritable
  Genes genes;

  // position
  Position pos;
} Bunny;

void print_summary(void *b_list, int N) {
  Bunny *bunnies = (Bunny *)b_list;
  int alive = 0;
  float energy = 0;

  for (int i = 0; i < N; i++) {
    if (!bunnies[i].alive)
      continue;
    alive++;
    energy += bunnies[i].energy;
  }
  printf("alive=%d avg_energy=%.1f\n", alive, alive ? energy / alive : 0.0f);
}

void print_bunnies(void *b_list, int N) {
  Bunny *bunnies = (Bunny *)b_list;

  for (int i = 0; i < N; i++) {
    Bunny *b = &bunnies[i];

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
  b->age = 0;
  b->repro_cooldown = 0;
  b->move_budget = 0;
  b->genes = (Genes){.vision = 1, .speed = 1, .fear = 1, .herd = 1};

  b->pos.x = rand() % W;
  b->pos.y = rand() % H;
}

void initialize_bunnies(void *b_list, int size) {
  if (b_list == NULL)
    return;

  Bunny *bunnies = (Bunny *)b_list;

  for (int i = 0; i < size; i++) {
    initialize_bunny(&bunnies[i]);
  }
}

void *create_bunnies(int size) {
  if (size <= 0)
    return NULL;

  Bunny *bunnies = malloc(size * sizeof(Bunny));

  if (!bunnies)
    return NULL;

  initialize_bunnies(bunnies, size);

  return bunnies;
}

static void metabolism(Bunny *b) {
  b->energy -=
      0.5f + 0.15f * b->genes.vision + 0.2f * b->genes.speed * b->genes.speed;
  b->age++;
  if (b->energy <= 0 || b->age > 150)
    b->alive = 0;
}

void destroy_bunnies(void *bunnies) { free(bunnies); }

static void process_bunny(Bunny *b) {
  if (!b->alive)
    return;

  int nx = b->pos.x;
  int ny = b->pos.y;

  switch (rand() % 4) {
  case 0:
    ny++;
    break;
  case 1:
    nx++;
    break;
  case 2:
    ny--;
    break;
  case 3:
    nx--;
    break;
  }

  b->pos.x = (nx + W) % W;
  b->pos.y = (ny + H) % H;

  metabolism(b);
}

void update_bunnies(void *b_list, int size) {
  Bunny *bunnies = (Bunny *)b_list;

  int *order = malloc(size * sizeof(int));
  for (int i = 0; i < size; i++)
    order[i] = i;

  shuffle(order, size);

  for (int i = 0; i < size; i++)
    process_bunny(&bunnies[order[i]]);

  free(order);
}

void draw_bunnies(void *b_list, int size) {
  Bunny *bunnies = b_list;
  for (int i = 0; i < size; i++)
    if (bunnies[i].alive)
      draw_pixel(bunnies[i].pos.x, bunnies[i].pos.y, WHITE);
}