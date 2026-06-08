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

static void print_separator(void) {
    printf("════════════════════════════════════════════\n");
}


static void draw_stats(RabbitList *rabbits) {

    print_separator();

    for (int i = 0; i < rabbits->count; i++) {

        Rabbit *r = &rabbits->rabbits[i];

        printf(BLUE "%d -> " RESET, r->generation);
        if (r->birthed) {
            printf(GREEN);
        } else {
            printf(RED);
        }

        printf("Rabbit %d | Pos=(%d,%d) | Energy=%d | Vision=%d | Speed=%d\n",
            i,
            r->x, r->y,
            r->energy,
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

    printf(" Tick: %-5d  Alive: %-5d  Born: %-5d  \n",
        state->ticks,
        list->count,
        state->total_birthed
    );


    printf("╚════════════════════════════════════════════╝\n\n");


    printf("   ╔");
    for (int x = 0; x < m.width; x++) {
        printf("══");
    }
    printf("╗\n");


    for (int y = 0; y < m.height; y++) {

        printf("   ║");

        for (int x = 0; x < m.width; x++) {

            int hasRabbit = 0;

            for (int k = 0; k < list->count; k++) {
                if (list->rabbits[k].x == x &&
                    list->rabbits[k].y == y) {
                    hasRabbit = 1;
                    break;
                }
            }

            if (hasRabbit) {
                printf("🐇");
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


    printf("   ╚");
    for (int x = 0; x < m.width; x++) {
        printf("══");
    }
    printf("╝\n\n");


    draw_stats(list);
}