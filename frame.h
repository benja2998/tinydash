#ifndef FRAME_H_
#define FRAME_H_

#define ROWS 8
#define COLS 32
#define JUMPPOWER 4

struct FrameBuffer {
  char pixels[ROWS][COLS];
};
#endif
