#ifndef FRAME_H_
#define FRAME_H_

#ifndef ROWS
#define ROWS 16
#endif

#ifndef COLS
#define COLS 64
#endif

#define JUMPPOWER 4

struct FrameBuffer
{
  char pixels[ROWS][COLS];
};
#endif
