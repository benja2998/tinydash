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
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#ifndef NUM_ENM
#define NUM_ENM 8
#endif

#ifndef NUM_STR
#define NUM_STR ROWS / 4 + (ROWS / 2)
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

  printf ("\033[?25l");

  old_flags = fcntl (STDIN_FILENO, F_GETFL, 0);
  fcntl (STDIN_FILENO, F_SETFL, old_flags | O_NONBLOCK);
}

void
input_cleanup (void)
{
  tcsetattr (STDIN_FILENO, TCSANOW, &old_termios);
  fcntl (STDIN_FILENO, F_SETFL, old_flags);
  printf ("\033[?25h");
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

typedef struct
{
  int row;
  int col;
} Star;

Star Stars[NUM_STR] = { 0 };

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
      inc += 4;
      ini += inc;
      Enemies[i].row = ROWS / 2;
      Enemies[i].col = ini;
      Enemies[i].ini = ini;
    }

  for (int i = 0; i < NUM_STR; i++)
    {
      int row = rand () % (ROWS + 1);
      int col = rand () % (COLS + 1);
      Stars[i].row = row;
      Stars[i].col = col;
    }
}

void
update_map (struct FrameBuffer *m, Player *plr)
{
  memset (m->pixels, '.', sizeof m->pixels);

  for (int i = 0; i < NUM_STR; ++i)
    {
      if (Stars[i].row >= 0 && Stars[i].row < ROWS && Stars[i].col >= 0
          && Stars[i].col < COLS)
        {
          m->pixels[Stars[i].row][Stars[i].col] = '*';
        }
    }

  for (int i = 0; i < ROWS; i++)
    {
      for (int j = 0; j < COLS; j++)
        {
          if (i >= (ROWS / 2) + 1)
            {
              m->pixels[i][j] = '%';
            }
        }
    }

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
  srand (time (0));
  signal (SIGINT, SIG_IGN);
  struct FrameBuffer my_map;
  Player plr = { 0 };

  map_init (&my_map, &plr);

  input_init ();
  atexit (input_cleanup);

  bool slowdown = false;
  bool background_slowdown = true;
  bool hasslowed = false;

  while (1)
    {
      int key = get_input ();

      if (key == ' ' && plr.row == ROWS / 2)
        {
          plr.row -= JUMPPOWER;
        }
      else if (key == ' ' && plr.row == (ROWS / 2) - 1)
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
              frametime -= 100;
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

      if (background_slowdown == false && hasslowed == true)
        {
          background_slowdown = true;
          hasslowed = false;
        }
      else if (background_slowdown == true && hasslowed == false)
        {
          background_slowdown = false;
          hasslowed = true;
        }

      for (int i = 0; i < NUM_STR; ++i)
        {
          if (Stars[i].col > -1)
            {
              if (background_slowdown == false)
                {
                  Stars[i].col--;
                }
            }
          else
            {
              Stars[i].col = COLS;
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
