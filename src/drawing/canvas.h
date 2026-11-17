#include "types.h"
#include <stdint.h>

#define COLOR_BG 0xFF1A1C23   // Dark slate background
#define COLOR_GRID 0xFF242731 // Subtle grid line

#define WHITE 0xFFFFFFFF
#define BLACK 0xFF1A1C23
#define RED 0xFFFF6B6B
#define GREEN 0x3cb371
#define BLUE 0xFFA7C7E7
#define YELLOW 0xFFFFF1A8
#define ORANGE 0xFFFFD1A9
#define PURPLE 0xFFC3B1E1
#define PINK 0xFFFFC8DD
#define CYAN 0xFFB5EAEA
#define GRAY 0xFF8395A7
#define BROWN 0xFF835843

const uint32_t *canvas_pixels(void);
void clear_canvas(void);
void draw_pixel(int x, int y, uint32_t color);
uint32_t get_pixel_color(int x, int y);
void get_canvas_positions(Position *pos);
void debug_map_details(void);
