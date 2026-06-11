#include "types.h"
#include <stdio.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define BLUE    "\033[34m"
#define WHITE   "\033[37m"

void clear_screen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

const char *state_to_string(RabbitState state)
{
    switch (state) {
        case IDLE: return "IDLE";
        case SEEK: return "SEEK";
        case MOVE: return "MOVE";
        case REPRODUCE: return "REPRODUCE";
        default:   return "UNKNOWN";
    }
}

static void print_separator(void) {
    printf("══════════════════════════════════════════════\n");
}

static void print_bar (int i) {
    for (int j = 0; j < i; j++) {
        printf("▉");
    }
    printf("     %d", i);
    printf("\n");
}


static void draw_stats(RabbitList *rabbits) {

    print_separator();
    printf("🐇 |");
    print_bar(rabbits->count);
    print_separator();
    for (int i = 0; i < rabbits->count; i++) {

        Rabbit *r = &rabbits->rabbits[i];

        printf(BLUE "%d -> " RESET, r->generation);
        if (r->birthed) {
            printf(GREEN);
        } else {
            printf(RED);
        }
        printf("Rabbit %d | (%s) | Pos=(%d,%d) | Energy=%d%% | Vision=%d | Speed=%d\n",
            i,
            state_to_string(r->cur_state),
            r->x, r->y,
            (r->energy * 100) / r->max_energy,
            r->vision,
            r->speed
        );

        printf(RESET);
    }

    print_separator();
}

void draw_map(Map m, RabbitList *list, GameState *state) {


    clear_screen();

    printf("╔════════════════════════════════════════════╗\n");
    printf("║                  CELMATA                   ║\n");
    printf("╠════════════════════════════════════════════╣\n");

    printf(" Tick: %-5d  Born: %-5d  Dead: %-5d \n",
        state->ticks,
        state->total_birthed,
        state->total_dead
    );


    printf("╚════════════════════════════════════════════╝\n\n");


    printf("  ╔");
    for (int x = 0; x < m.width; x++) {
        printf("══");
    }
    printf("╗\n");


    for (int y = 0; y < m.height; y++) {

        printf("  ║");

        for (int x = 0; x < m.width; x++) {

            int hasRabbit = 0;
            int love = 0;

            for (int k = 0; k < list->count; k++) {
                if (list->rabbits[k].x == x &&
                    list->rabbits[k].y == y) {
                    hasRabbit = 1;
                    love = (list->rabbits[k].cooldown > 0) ? 1 : 0;
                    break;
                }
            }

            if (hasRabbit) {
                love ? printf("💕") : printf("🐇");
            }
            else if (m.grid[y][x].grass) {
                printf("🌿");
            }
            else {
                printf("· ");
            }
        }

        printf("║\n");
    }


    printf("  ╚");
    for (int x = 0; x < m.width; x++) {
        printf("══");
    }
    printf("╝\n\n");


    draw_stats(list);
}

void print_stats(GameState *gs, RabbitList *rabbits){
    print_separator();
    printf("Simulation Ended\n");
    printf("Stats:\n");
    printf("Total Born: %d, Total Dead: %d \n", gs->total_birthed, gs->total_dead);
    printf("Generations: %d \n", gs->max_generation);
    printf("Ticks: %d \n", gs->ticks);
    print_separator();

}