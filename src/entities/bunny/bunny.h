#include "types.h"

typedef struct BunnyList BunnyList;

BunnyList *create_bunnies(int size);
void destroy_bunnies(BunnyList *list);
void update_bunnies(BunnyList *list);
void draw_bunnies(const BunnyList *list);
void print_bunnies(const BunnyList *list);
void print_summary(const BunnyList *list);
void print_bunny_count(BunnyList *b_list);
void print_bunny_csv(BunnyList *b_list);
int get_alive_bunnies_count(const BunnyList *list);
int get_king_bunny(const BunnyList *list);
int get_king_bunny_stats(const BunnyList *list, Genes *genes, int *age);
