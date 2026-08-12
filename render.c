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

#include "render.h"

void
map_render (struct FrameBuffer *m, int score)
{
  clear ();
  fflush (stdout);
  for (int row = 0; row < ROWS; row++)
    {
      for (int col = 0; col < COLS; col++)
        {
          switch (m->pixels[row][col])
            {
            case '@':
              write (STDOUT_FILENO, PLR, strlen (PLR));
              break;
            case '$':
              write (STDOUT_FILENO, ENM, strlen (ENM));
              break;
            case '*':
              write (STDOUT_FILENO, STR, strlen (STR));
              write (STDOUT_FILENO, BCK, strlen (BCK));
              break;
            case '%':
              write (STDOUT_FILENO, GRD, strlen (GRD));
              break;
            case '.':
              write (STDOUT_FILENO, BCK, strlen (BCK));
              break;
            default:
              break;
            }

          switch (m->pixels[row][col])
            {
            case '*':
              write (STDOUT_FILENO, "*", 1);
              break;
            default:
              write (STDOUT_FILENO, " ", 1);
              break;
            }

          fflush (stdout);
          write (STDOUT_FILENO, RST, 4);
        }
      write (STDOUT_FILENO, "\n", 1);
    }

  fflush (stdout);
  printf ("space to jump - q to quit\n"
          "\n"
          "space while in air will\n"
          "go down instantly\n"
          "\n"
          "space while one row above\n"
          "the ground will double jump\n"
          "\n" ENM " " RST " - enemy, " PLR " " RST " - player\n"
          "\n");
  printf ("score: %d\n", score);
}
