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

    WolfList wolf_pack;
    initWolfs(&wolf_pack, START_WOLFS, MAP_WIDTH, MAP_HEIGHT);

    GameState gstate;
    gstate.ticks = 0;

    main_loop(m, &rabbits,&wolf_pack, &gstate);

    freeRabbits(&rabbits);
    freeWolfs(&wolf_pack);
    freeMap(&m);

    return 0;
}