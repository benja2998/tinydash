/*
  tinydash - a terminal-based game
  Copyright (C) 2026 benja2998

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
