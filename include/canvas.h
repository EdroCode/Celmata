#include <stdint.h>

#define W 125
#define H 125
#define SCALE 10

#define WHITE 0xFFFFFFFF
#define BLACK 0x00000000
#define RED   0xFFFF0000

void canvas_set(int x, int y, uint32_t color);
const uint32_t *canvas_pixels(void);
void draw_pixel(int x, int y, uint32_t color);
uint32_t get_pixel_color (int x, int y);
