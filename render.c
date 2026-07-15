#include "render.h"

void map_render(struct FrameBuffer *m) {
  clear();
  fflush(stdout);
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      write(STDOUT_FILENO, &m->pixels[row][col], 1);
    }
    write(STDOUT_FILENO, "\n", 1);
  }
}
