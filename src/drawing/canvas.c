#include "src/canvas.h"
#include <stdint.h>
#include <wchar.h>

static uint32_t canvas[W * H];

const uint32_t *canvas_pixels(void) { return canvas; }

void clear_canvas() {
  for (int i = 0; i < W * H; i++)
    canvas[i] = BLACK;
}

void draw_pixel(int x, int y, uint32_t color) {
  if (x < 0 || x >= W || y < 0 || y >= H)
    return;
  canvas[y * W + x] = color;
}

uint32_t get_pixel_color(int x, int y) { return canvas[y * W + x]; }