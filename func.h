#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#include "keys.h"
#include "settings.h"

// DO NOT CHANGE
#define WIDTH 25
#define HEIGHT 24
#define TOPLSITMAXLINELENGTH 34

// START
#define maxQueueSize 5

int nextTrue = 1;
int nextQueue[maxQueueSize];

// END

// init variables
char piece;
char *name;
// set flags to default values
int level, score, showtext = 1, next, shownext = 1, end, clrlines = 0;
int startlevel, dropped = 0;
int fixedpoint[2] = {0};
// screen is divided into three sections - left, right and center
char left[HEIGHT][WIDTH] = {0};
char center[HEIGHT][WIDTH] = {0};
char right[HEIGHT][WIDTH] = {0};
// all possible tetrominos
char TetrominoI[2][WIDTH] = {"<! . . . . . . . . . .!>",
                             "<! . . .[][][][] . . .!>"};
char TetrominoJ[2][WIDTH] = {"<! . . .[] . . . . . .!>",
                             "<! . . .[][][] . . . .!>"};
char TetrominoL[2][WIDTH] = {"<! . . . . .[] . . . .!>",
                             "<! . . .[][][] . . . .!>"};
char TetrominoO[2][WIDTH] = {"<! . . . .[][] . . . .!>",
                             "<! . . . .[][] . . . .!>"};
char TetrominoS[2][WIDTH] = {"<! . . . .[][] . . . .!>",
                             "<! . . .[][] . . . . .!>"};
char TetrominoT[2][WIDTH] = {"<! . . . .[] . . . . .!>",
                             "<! . . .[][][] . . . .!>"};
char TetrominoZ[2][WIDTH] = {"<! . . .[][] . . . . .!>",
                             "<! . . . .[][] . . . .!>"};
char TetrominoE[2][WIDTH] = {"<!                    !>",
                             "<!                    !>"};

struct timeval t1, t2;

void updatescore()
{
  // updates score
  char *tmp = malloc(sizeof *tmp * 15);
  sprintf(tmp, "%-14d", score);
  memcpy(left[7] + 9, tmp, 14);
  free(tmp);
}

void addscore()
{
  // add score to toplist if needed
  if (!score)
    return;
  FILE *f;
  if (!(f = fopen("toplist", "r")))
  {
    if (!(f = fopen("toplist", "w")))
      exit(1);
    fprintf(f, "NAME          LVL SCORE        \n%-13s %2d  %-14d\n",
            name, level, score);
    fclose(f);
    return;
  }
  f = fopen("toplist", "r");
  int num, added = 0;
  char *buffer = malloc(sizeof *buffer * TOPLSITMAXLINELENGTH);
  FILE *tmp;
  if (!(tmp = fopen("tmp", "a+")))
    exit(1);
  int cntr = 21;
  while (fgets(buffer, TOPLSITMAXLINELENGTH, f) != NULL && --cntr)
  {
    num = strtol(buffer + 18, NULL, 10);
    if (!added && score > num && num != 0)
    {
      fprintf(tmp, "%-13s %2d  %-14d\n", name, level, score);
      score = 0;
    }
    fputs(buffer, tmp);
  }
  if (cntr && score)
    fprintf(tmp, "%-13s %2d  %-14d\n", name, level, score);
  fclose(f);
  fclose(tmp);
  remove("toplist");
  rename("tmp", "toplist");
  free(buffer);
}

int gameover()
{
  // prints game over screen
  nodelay(stdscr, FALSE);
  if (!end)
    addscore();
  end = 1;
  memcpy(left[9], "    __      __      ___ \0"
                  "  /'_ `\\  /'__`\\  /' __`\0"
                  " /\\ \\L\\ \\/\\ \\L\\.\\_/\\ \\/\\\0"
                  " \\ \\____ \\ \\__/.\\_\\ \\_\\ \0"
                  "  \\/___L\\ \\/__/\\/_/\\/_/\\\0"
                  "    /\\____/             \0"
                  "    \\_/__/              \0",
         WIDTH * 7);
  memcpy(center[9], "___      __         ___ \0"
                    " __`\\  /'__`\\      / __`\0"
                    " \\/\\ \\/\\  __/     /\\ \\L\\\0"
                    "\\_\\ \\_\\ \\____\\    \\ \\___\0"
                    "/_/\\/_/\\/____/     \\/___\0",
         WIDTH * 5);
  memcpy(right[9], "  __  __    __  _ __    \0"
                   "\\/\\ \\/\\ \\ /'__`/\\`'__\\  \0"
                   " \\ \\ \\_/ /\\  __\\ \\ \\/   \0"
                   "_/\\ \\___/\\ \\____\\ \\_\\   \0"
                   "/  \\/__/  \\/____/\\/_/   \0",
         WIDTH * 5);
  memcpy(center[16], "    : QUIT    : RESET   \0"
                     "        : TOPLIST       \0",
         WIDTH * 2);
  center[16][3] = toupper(EXT);
  center[16][13] = toupper(RSET);
  center[17][7] = toupper(TPLS);
  clear();
  printw("\n");
  for (int i = 2; i < HEIGHT; ++i)
  {
    if (i == 9 || i == 15 || i == 16)
      attron(COLOR_PAIR(2));
    else if (i == 18)
      attron(COLOR_PAIR(1));
    printw("%s", left[i]);
    if (i == 14 || i == 15)
      attron(COLOR_PAIR(1));
    printw("%s", center[i]);
    printw("%s\n", right[i]);
  }
  refresh();
  return 1;
}

void updatelevel()
{
  // updates level
  char *tmp = malloc(sizeof *tmp * 15);
  sprintf(tmp, "%-14d", level);
  memcpy(left[5] + 9, tmp, 14);
  free(tmp);
}

void setkeybind()
{
  // reads key bindings from config
  right[2][3] = toupper(MOVL);
  right[3][3] = toupper(MOVR);
  right[4][3] = toupper(ROTA);
  right[5][3] = toupper(DROP);
  right[5][14] = toupper(RSET);
  right[6][3] = toupper(SNXT);
  right[7][3] = toupper(STXT);
  right[8][3] = toupper(EXT);
  right[9][3] = toupper(HARD);
}

// START

void initNextQ()
{
  for (int i = 0; i < maxQueueSize; i++)
  {
    nextQueue[i] = rand() % 7;
  }
}

void updateNextQueue()
{

  // next = nextQueue[0];
  for (int i = 0; i < maxQueueSize - 1; i++)
  {
    nextQueue[i] = nextQueue[i + 1];
  }

  nextQueue[maxQueueSize - 1] = rand() % 7;
}

void showNextQueue()
{
  int j = 5;
  for (int i = 0; i < maxQueueSize; i++)
  {
    switch (nextQueue[i])
    {
    case 0:
      memcpy(left[j] + WIDTH - 10, TetrominoI[1] + 8, 8);
      j = j + 3;
      break;
    case 1:
      memcpy(left[j] + WIDTH - 8, TetrominoJ[0] + 8, 2);
      memcpy(left[j + 1] + WIDTH - 8, TetrominoJ[1] + 8, 6);
      j = j + 3;
      break;
    case 2:
      memcpy(left[j] + WIDTH - 4, TetrominoL[0] + 12, 2);
      memcpy(left[j + 1] + WIDTH - 8, TetrominoL[1] + 8, 6);
      j = j + 3;
      break;
    case 3:
      memcpy(left[j] + WIDTH - 6, TetrominoO[0] + 10, 4);
      memcpy(left[j + 1] + WIDTH - 6, TetrominoO[1] + 10, 4);
      j = j + 3;
      break;
    case 4:
      memcpy(left[j] + WIDTH - 6, TetrominoS[0] + 10, 4);
      memcpy(left[j + 1] + WIDTH - 8, TetrominoS[1] + 8, 4);
      j = j + 3;
      break;
    case 5:
      memcpy(left[j] + WIDTH - 6, TetrominoT[0] + 10, 2);
      memcpy(left[j + 1] + WIDTH - 8, TetrominoT[1] + 8, 6);
      j = j + 3;
      break;
    case 6:
      memcpy(left[j] + WIDTH - 8, TetrominoZ[0] + 8, 4);
      memcpy(left[j + 1] + WIDTH - 6, TetrominoZ[1] + 10, 4);
      j = j + 3;
      break;
    }
  }
}

void afterSNQ()
{
  int j = 4 + 3 * maxQueueSize;
  for (int i = 4; i < j; i++)
  {
    memcpy(left[i] + WIDTH - 10, "        ", 8);
  }
}

// END