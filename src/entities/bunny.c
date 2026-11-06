#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "canvas.h"

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
 *                     +  genes.herd  * (IDEAL_DENSITY - density(c)) * bunny_pull(c)
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
 *    (optional) on death: adjacent bushes gain fertility -> bunnies feed the grass
 *
 * 6. WOLF ENCOUNTER  (resolved on the WOLF's turn, not here)
 *    escape = clamp(0.1 + 0.2 * genes.speed + (adjacent_to_bush ? 0.3 : 0), 0, 0.9)
 *    attack_success = wolf.hunger * (1 - escape)  // wolf.hunger = 1 - wolf.energy / MAX
 *    success -> bunny.alive = false, wolf.energy += bunny_value
 */

typedef struct { int x, y; } Position;
typedef enum { false, true } bool;

// Inheritable traits, shared by bunny and wolf so mutation code is reusable
typedef struct Genes {
    float vision;   // float, not int: see note below
    float speed;
    float fear;
    float herd;
} Genes;

typedef struct Bunny {
    // dynamic state
    float energy;
    int   age;
    int   repro_cooldown;
    float move_budget;
    bool  alive;

    // inheritable
    Genes genes;

    // position
    Position pos;
    Position old_pos;
} Bunny;



static Bunny initialize_bunny(Bunny *b){
    int as = rand() % (100-1);
    int bs = rand() % (100-1);
    
    b->alive = true;
    
    b->energy = 100;

    b->genes.vision = 1;
    b->genes.fear = 1;
    b->genes.herd = 1;
    b->genes.speed = 1;
    
    b->pos.x = as;
    b->pos.y = bs;
    b->old_pos = b->pos;

    return *b;
}

void initialize_bunnies(void *b_list, int size){
    if (b_list == NULL) return;

    Bunny *bunnies = (Bunny *)b_list;

    for (int i = 0; i < size; i++){
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

void destroy_bunnies(void *bunnies){
    free(bunnies);
}

static void process_bunny(Bunny *b) {
    int direction = rand() % 4;

    b->old_pos = b->pos;

    switch (direction) {
        case 0: b->pos.y++; break;
        case 1: b->pos.x++; break;
        case 2: b->pos.y--; break;
        case 3: b->pos.x--; break;
    }

}

void update_bunnies(void *b_list, int size){
    Bunny *bunnies = (Bunny *)b_list;
    
    for (int i = 0; i < size; i++) {
        process_bunny(&bunnies[i]);
    }
}

void draw_bunnies(void *b_list, int size) {
    Bunny *bunnies = b_list;

    for (int i = 0; i < size; i++) {
        // clean old position draw
        int old_x = bunnies[i].old_pos.x;
        int old_y = bunnies[i].old_pos.y;

        if (old_x >= 0 && old_x < W &&
            old_y >= 0 && old_y < H) {
            draw_pixel(old_x, old_y, BLACK);
        }

        int x = bunnies[i].pos.x;
        int y = bunnies[i].pos.y;

        if (x >= 0 && x < W && y >= 0 && y < H) {
            draw_pixel(x, y, WHITE);
        }
    }
}
