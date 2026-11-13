#include "types.h"
#include <stdint.h>

#define WHITE 0xFFFFFFFF
#define BLACK 0xFF2B2B33
#define RED 0xFFFF9AA2
#define GREEN 0xFFA8E6A1
#define BLUE 0xFFA7C7E7
#define YELLOW 0xFFFFF1A8
#define ORANGE 0xFFFFD1A9
#define PURPLE 0xFFC3B1E1
#define PINK 0xFFFFC8DD
#define CYAN 0xFFB5EAEA
#define GRAY 0xFFD3D3DB
#define BROWN 0xFFD9B99B

const uint32_t *canvas_pixels(void);
void clear_canvas();
void draw_pixel(int x, int y, uint32_t color);
uint32_t get_pixel_color(int x, int y);
void get_canvas_positions(Position *pos);
void debug_map_details();