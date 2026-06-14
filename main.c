#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>



int main() {

    SEED_BASED ? srand(SEED) : srand(time(NULL));

    Map m;
    initMap(&m, MAP_WIDTH, MAP_HEIGHT);

    RabbitList rabbits;
    initRabbits(&rabbits, START_RABBITS, m.width, m.height);

    GameState gstate;
    gstate.ticks = 0;

    main_loop(m, &rabbits, &gstate);

    freeRabbits(&rabbits);
    freeMap(&m);

    return 0;
}