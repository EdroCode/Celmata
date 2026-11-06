#include <stdint.h>

#define W 100
#define H 100
#define SCALE 8


#define WHITE 0xFFFFFFFF
#define RED   0xFFFF0000

void canvas_set(int x, int y, uint32_t color);
const uint32_t *canvas_pixels(void);
void draw_pixel(int x, int y, uint32_t color);