#include <sys/types.h>

void print_summary(void *b_list, int N);

void print_bunnies(void *b_list, int N);

void initialize_bunnies(void *b_list, int size);

void update_bunnies(void *b_list, int size);

void *create_bunnies(int size);

void destroy_bunnies(void *bunnies);

void draw_bunnies(void *b_list, int size);