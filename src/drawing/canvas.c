#include "canvas.h"
#include <stdint.h>
#include <wchar.h>

static uint32_t canvas[W * H];

void canvas_set(int x, int y, uint32_t color) {
    if (x < 0 || x >= W || y < 0 || y >= H) return;
    canvas[y * W + x] = color;
}

const uint32_t *canvas_pixels(void) { return canvas; }

void draw_pixel(int x, int y, uint32_t color){
    canvas[y * W + x] = color;
}

uint32_t get_pixel_color (int x, int y) {
    return canvas[y * W + x];
}