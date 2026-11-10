#ifndef BUNNY_H
#define BUNNY_H

typedef struct BunnyList BunnyList;

BunnyList *create_bunnies(int size);
void destroy_bunnies(BunnyList *list);

void update_bunnies(BunnyList *list);
void draw_bunnies(const BunnyList *list);

void print_bunnies(const BunnyList *list);
void print_summary(const BunnyList *list);

#endif