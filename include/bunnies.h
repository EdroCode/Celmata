#include <sys/types.h>

void initialize_bunnies(void *b_list, int size);

void update_bunnies(void *b_list, int size);

void *create_bunnies(int size);

void destroy_bunnies(void *bunnies);

void draw_bunnies(void *b_list, int size);