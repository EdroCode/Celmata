#include "bunny_internal.h"
#include "drawing/canvas.h"
#include "utils/utils.h"
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
Bunny *add_new_bunny(BunnyList *list, Position pos, Genes genes, float energy) {
  int slot = -1;

  for (int i = 0; i < list->bunny_count; i++) {
    if (!list->bunnies[i].alive) {
      slot = i;
      break;
    }
  }

  if (slot < 0) {
    if (list->bunny_count >= list->bunny_capacity)
      return NULL;
    slot = list->bunny_count++;
  }

  Bunny *b = &list->bunnies[slot];
  *b = (Bunny){
      .alive = 1, .hp = 100, .energy = energy, .genes = genes, .pos = pos};
  return b;
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

struct BunnyList *create_bunnies(int size) {
  int capacity = W * H;

  if (size <= 0 || size > capacity)
    return NULL;

  struct BunnyList *list = malloc(sizeof(struct BunnyList));
  if (!list)
    return NULL;

  list->bunnies = malloc(capacity * sizeof(Bunny));
  if (!list->bunnies) {
    free(list);
    return NULL;
  }

  list->bunny_count = size;
  list->bunny_capacity = capacity;

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

static void reproduce(BunnyList *b_list, Bunny *b, Position p) {
  add_new_bunny(b_list, p, b->genes, 100);
}

static void process_bunny(int bunny_id, BunnyList *list) {
  Bunny *b = &list->bunnies[bunny_id];

  if (!b->alive)
    return;

  for (int attempt = 0; attempt < 4; attempt++) {
    int dx = rand() % 3 - 1;
    int dy = rand() % 3 - 1;
    if (dx == 0 && dy == 0)
      continue;

    Position target = {(b->pos.x + dx + W) % W, (b->pos.y + dy + H) % H};

    // Reproduce
    int b_surround[25600]; // REview this
    int n = check_surroundings(list, &b->pos, bunny_id, b->genes.vision,
                               b_surround);

    if (n >= 2 && (float)rand() / RAND_MAX < 0.10f) {

      Position valid_pos[8];

      int pn = bunny_check_empty_surroundings(list, &b->pos, b->genes.vision,
                                              valid_pos, 8);

      if (pn > 0) {
        int ix = rand() % pn;
        reproduce(list, b, valid_pos[ix]);
      }
    }

    if (!check_cell(list, &target)) {
      b->pos = target;
      break;
    }
  }

  if (check_bush_surroundings(list, &b->pos, bunny_id, b->genes.vision, NULL)) {
    b->energy += 10; // TODO
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
    process_bunny(order[i], list);

  free(order);
}

void draw_bunnies(const BunnyList *list) {
  for (int i = 0; i < list->bunny_count; i++)
    if (list->bunnies[i].alive)
      draw_pixel(list->bunnies[i].pos.x, list->bunnies[i].pos.y, WHITE);
}