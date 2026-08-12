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
          if (m->pixels[row][col] == '@')
            {
              write (STDOUT_FILENO, "\033[38;5;123m",
                     strlen ("\033[38;5;123m"));
              write (STDOUT_FILENO, "\033[48;5;0m", strlen ("\033[48;5;0m"));
            }
          else if (m->pixels[row][col] == '$')
            {
              write (STDOUT_FILENO, "\033[38;5;208m",
                     strlen ("\033[38;5;208m"));
              write (STDOUT_FILENO, "\033[48;5;0m", strlen ("\033[48;5;0m"));
            }
          else if (m->pixels[row][col] == '*')
            {
              write (STDOUT_FILENO, "\033[38;5;227m",
                     strlen ("\033[38;5;227m"));
              write (STDOUT_FILENO, "\033[48;5;0m", strlen ("\033[48;5;0m"));
            }
          else if (m->pixels[row][col] == '%')
            {
              write (STDOUT_FILENO, "\033[38;5;64m", strlen ("\033[38;5;64m"));
              write (STDOUT_FILENO, "\033[48;5;64m", strlen ("\033[48;5;64m"));
            }
          else if (m->pixels[row][col] == '.')
            {
              write (STDOUT_FILENO, "\033[48;5;0m", strlen ("\033[48;5;0m"));
            }

          write (STDOUT_FILENO, &m->pixels[row][col], 1);
          fflush (stdout);
          write (STDOUT_FILENO, "\033[0m", 4);
        }
      write (STDOUT_FILENO, "\n", 1);
    }

  fflush (stdout);
  printf ("space to jump - q to quit\n");
  printf ("score: %d\n", score);
}
