#include "canvas.h"
#include "types.h"
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>

static uint32_t canvas[W * H];

const uint32_t *canvas_pixels(void) { return canvas; }

void get_canvas_positions(Position *pos) {
  for (int i = 0; i < W * H; i++) {
    pos[i].x = i % W;
    pos[i].y = i / W; // review
  }
}

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

void debug_map_details() {
  printf("\n\n\n\n------ MAP ------\n");
  printf("Size: %d * %d\n", H, W);
  printf("Scale Factor: %d\n", SCALE);
  printf("-----------------\n\n");
}