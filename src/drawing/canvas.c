#include "canvas.h"
#include "types.h"
#include <stdint.h>
#include <stdio.h>

static uint32_t canvas[W * H];

const uint32_t *canvas_pixels(void) { return canvas; }

void get_canvas_positions(Position *pos) {
  for (int i = 0; i < W * H; i++) {
    pos[i].x = i % W;
    pos[i].y = i / W;
  }
}

void clear_canvas(void) {
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      // Grid background
      if (x % 10 == 0 || y % 10 == 0) {
        canvas[y * W + x] = COLOR_GRID;
      } else {
        canvas[y * W + x] = COLOR_BG;
      }
    }
  }
}

void draw_pixel(int x, int y, uint32_t color) {
  if (x < 0 || x >= W || y < 0 || y >= H)
    return;
  canvas[y * W + x] = color;
}

uint32_t get_pixel_color(int x, int y) {
  if (x < 0 || x >= W || y < 0 || y >= H)
    return COLOR_BG;
  return canvas[y * W + x];
}

void debug_map_details(void) {
  printf("\n\n\n\n------ MAP ------\n");
  printf("Size: %d * %d\n", H, W);
  printf("Scale Factor: %d\n", SCALE);
  printf("-----------------\n\n");
}
