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

#include "frame.h"
#include "helpers.h"
#include "render.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#ifndef NUM_ENM
#define NUM_ENM 2
#endif

#ifndef FRAMETIME
#define FRAMETIME 40000
#endif

int frametime = FRAMETIME;

static struct termios old_termios;
static int old_flags;
int score = 0;

void
my_exit (void)
{
  fflush (stdout);
  clear ();
  fflush (stdout);
  char c[1025];

  char filename[512];
  char *home = getenv ("HOME");

  if (home == NULL)
    {
      exit (1);
    }

  snprintf (filename, sizeof (filename), "%s%s", home, "/tinydash_hs");

  FILE *fptr;
  if ((fptr = fopen (filename, "a+")) == NULL)
    {
      printf ("Error");
      exit (1);
    }

  struct stat st;
  if (stat (filename, &st) != 0)
    {
      exit (1);
    }

  int filesize = (int)st.st_size;
  if (filesize == 0)
    {
      fprintf (fptr, "0");
    }

  fscanf (fptr, "%[^\n]", c);
  fflush (fptr);
  fclose (fptr);

  if ((fptr = fopen (filename, "w+")) == NULL)
    {
      printf ("Error");
      exit (1);
    }

  int curhs = atoi (c);

  if (score > curhs)
    {
      snprintf (c, 512, "%d", score);
    }

  fprintf (fptr, "%s", c);

  fflush (fptr);
  fclose (fptr);

  printf ("Score: %d, High Score: %s\n", score, c);

  sleep (2);

  exit (0);
}

void
input_init (void)
{
  struct termios new_termios;
  tcgetattr (STDIN_FILENO, &old_termios);
  new_termios = old_termios;
  new_termios.c_lflag &= ~(ICANON | ECHO);
  tcsetattr (STDIN_FILENO, TCSANOW, &new_termios);

  old_flags = fcntl (STDIN_FILENO, F_GETFL, 0);
  fcntl (STDIN_FILENO, F_SETFL, old_flags | O_NONBLOCK);
}

void
input_cleanup (void)
{
  tcsetattr (STDIN_FILENO, TCSANOW, &old_termios);
  fcntl (STDIN_FILENO, F_SETFL, old_flags);
}

int
get_input (void)
{
  char c;

  if (read (STDIN_FILENO, &c, 1) == 1)
    {
      return c;
    }
  return -1;
}

typedef struct
{
  int row;
  int col;
  int ini;
} Enemy;

Enemy Enemies[NUM_ENM] = { 0 };

typedef struct
{
  int row;
  int col;
} Player;

void
map_init (struct FrameBuffer *m, Player *plr)
{
  memset (m->pixels, '.', sizeof (m->pixels));
  plr->row = ROWS / 2;
  plr->col = 0;

  int ini = COLS;
  int inc = 1;

  for (int i = 0; i < NUM_ENM; i++)
    {
      inc += 2;
      ini += inc;
      Enemies[i].row = ROWS / 2;
      Enemies[i].col = ini;
      Enemies[i].ini = ini;
    }
}

void
update_map (struct FrameBuffer *m, Player *plr)
{
  memset (m->pixels, '.', sizeof m->pixels);

  if (plr->row >= 0 && plr->row < ROWS && plr->col >= 0 && plr->col < COLS)
    {
      m->pixels[plr->row][plr->col] = '@';
    }

  for (int i = 0; i < NUM_ENM; ++i)
    {
      if (Enemies[i].row >= 0 && Enemies[i].row < ROWS && Enemies[i].col >= 0
          && Enemies[i].col < COLS)
        {
          m->pixels[Enemies[i].row][Enemies[i].col] = '$';
        }
    }
}

int
main (void)
{
  struct FrameBuffer my_map;
  Player plr = { 0 };

  map_init (&my_map, &plr);

  input_init ();
  atexit (input_cleanup);

  int slowdown = false;

  while (1)
    {
      int key = get_input ();

      if (key == ' ' && plr.row == ROWS / 2)
        {
          plr.row -= JUMPPOWER;
        }
      else if (key == ' ' && plr.row == ROWS + 1)
        {
          plr.row -= JUMPPOWER;
        }
      else if (key == ' ')
        {
          plr.row = ROWS / 2;
        }

      if (key == 'q')
        {
          my_exit ();
        }

      if (plr.row < ROWS / 2)
        {
          if (slowdown == false)
            {
              plr.row++;
              slowdown = true;
            }
          else
            {
              slowdown = false;
            }
        }

      for (int i = 0; i < NUM_ENM; ++i)
        {
          if (Enemies[i].row == plr.row && Enemies[i].col == plr.col)
            {
              my_exit ();
            }

          if (Enemies[i].row != plr.row && Enemies[i].col == plr.col)
            {
              score++;
              frametime -= 10;
            }

          if (Enemies[i].col > -1)
            {
              Enemies[i].col--;
            }
          else
            {
              Enemies[i].col = Enemies[i].ini;
            }
        }

      update_map (&my_map, &plr);
      map_render (&my_map);
      if (frametime <= 100)
        {
          frametime = FRAMETIME;
        }
      usleep (frametime);
    }
  return 0;
}
