
// G L O B A L     V A R I A B L E S

//DO NOT CHANGE
#define WIDTH 25
#define HEIGHT 24
#define TOPLSITMAXLINELENGTH 34
//init variables
char piece;
char *name;
//set flags to default values
int level, score, showtext = 1, next, shownext = 1, end, clrlines = 0;
int startlevel, dropped = 0;
int fixedpoint[2] = {0};
//screen is divided into three sections - left, right and center
char left[HEIGHT][WIDTH] = {0};
char center[HEIGHT][WIDTH] = {0};
char right[HEIGHT][WIDTH] = {0};
//all possible tetrominos
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
struct timeval t1, t2;

#include "settings.h"
#include "keys.h"
#define DROPINTERVAL (DELAY/1000) * ((300 - level*13)/5)
int nexttetrimino(){
  int x = 5;
  x += 4;
  return rand() % 2;
} 
int gameOver(){
  //prints game over screen
  nodelay(stdscr, FALSE);
  if(!end) addscore();
  end = 1;
  memcpy(left[9], "    __      __      ___ \0"
                  "  /'_ `\\  /'__`\\  /' __`\0"
                  " /\\ \\L\\ \\/\\ \\L\\.\\_/\\ \\/\\\0"
                  " \\ \\____ \\ \\__/.\\_\\ \\_\\ \0"
                  "  \\/___L\\ \\/__/\\/_/\\/_/\\\0"
                  "    /\\____/             \0"
                  "    \\_/__/              \0", WIDTH*7);
  memcpy(center[9], "___      __         ___ \0"
                    " __`\\  /'__`\\      / __`\0"
                    " \\/\\ \\/\\  __/     /\\ \\L\\\0"
                    "\\_\\ \\_\\ \\____\\    \\ \\___\0"
                    "/_/\\/_/\\/____/     \\/___\0", WIDTH*5);
  memcpy(right[9], "  __  __    __  _ __    \0"
                   "\\/\\ \\/\\ \\ /'__`/\\`'__\\  \0"
                   " \\ \\ \\_/ /\\  __\\ \\ \\/   \0"
                   "_/\\ \\___/\\ \\____\\ \\_\\   \0"
                   "/  \\/__/  \\/____/\\/_/   \0", WIDTH*5);
  memcpy(center[16], "    : QUIT    : RESET   \0"
                     "        : TOPLIST       \0", WIDTH*2);
  center[16][3] = toupper(EXT);
  center[16][13] = toupper(RSET);
  center[17][7] = toupper(TPLS);
  clear();
  printw("\n");
  for(int i=2; i<HEIGHT; ++i){
    if(i == 9 || i == 15 || i == 16) attron(COLOR_PAIR(2));
    else if( i == 18) attron(COLOR_PAIR(1));
    printw("%s", left[i]);
    if(i == 14 || i == 15) attron(COLOR_PAIR(1));
    printw("%s", center[i]);
    printw("%s\n", right[i]);
  }
  refresh();
  return 1;
}
    