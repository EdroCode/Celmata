#pragma once // Solution to have all the types on this file

#include <stdbool.h>

#define SEED 1231231
#define SEED_BASED false

#define GAME_SPEED 200 // In ms (1000 - 1s) (5000 - 1/2s)

// World 
#define MAP_HEIGHT 20
#define MAP_WIDTH  20
#define START_RABBITS 8
#define START_GRASS_CHANCE 40
#define GRASS_CHANCE 25
#define GRASS_NEIGHBOOR_CHANCE_MULTIPLIER 1
#define GRASS_DECAY 20
#define GRASS_COOLDOWN 5

// Rabbit
#define MAX_START_ENERGY 60
#define MAX_START_VISION 1
#define MAX_START_SPEED 2
#define MUTATION_CHANCE 20
#define REPRODUCE_CHANCE 10

typedef struct { int x, y; } Pos;

typedef struct Cell {
    int x;
    int y;
    bool grass;
    int cooldown;
    int decay;
} Cell;

typedef enum {
    IDLE,
    SEEK,
    MOVE,
    REPRODUCE
} RabbitState;

typedef struct Rabbit {
    int x,y; // Posicao
    int energy; // Rabbit energy
    int max_energy; // Rabbit max energy
    int vision; //  células consegue ver à sua volta
    int speed;  // Quantos movimentos pode fazer por tick
    int cooldown;
    bool birthed;
    int generation;
    int dir;
    RabbitState cur_state;
} Rabbit;

typedef struct {
    Rabbit *rabbits;
    int count;
    int capacity;
} RabbitList;

typedef struct Map {
    int width;
    int height;
    Cell **grid;
} Map;

typedef struct GameState {
    Map map;
    int ticks;
    int game_speed;
    int total_birthed;
    int total_dead;
    int max_generation;
} GameState;

// MainLoop
void main_loop(Map m, RabbitList *rabbits, GameState *state);

// Map
void initMap(Map *map, int width, int height);
void update_map(Map *m);
void freeMap(Map *map);

// User Interface
void clear_screen(void);
void draw_map (Map m, RabbitList *list, GameState *state);
void print_stats(GameState *gs, RabbitList *rabbits);

// Game Logic

// Rabbits
void initRabbits(RabbitList *list, int count, int mapWidth, int mapHeight);
void add_rabbit(RabbitList *rabs, Rabbit rabbit);
void kill_rabbit(RabbitList *rabs, int i);
void update_rabbits(Map *m, RabbitList *rabs, GameState *state);
void freeRabbits(RabbitList *list);