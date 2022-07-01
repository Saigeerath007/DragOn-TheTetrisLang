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
#include "myhead.h"

#define maxQueueSize 5

int nextTrue = 1;
int nextQueue[maxQueueSize];


void addscore(){
  //add score to toplist if needed
  if(!score) return;
  FILE *f;
  if(!(f=fopen("toplist", "r"))){
    if(!(f=fopen("toplist", "w")))
      exit(1);
    fprintf(f, "NAME          LVL SCORE        \n%-13s %2d  %-14d\n",
            name, level, score);
    fclose(f);
    return;
  }
  f=fopen("toplist", "r");
  int num, added = 0;
  char *buffer=malloc(sizeof *buffer * TOPLSITMAXLINELENGTH);
  FILE *tmp;
  if(!(tmp=fopen("tmp", "a+")))
    exit(1);
  int cntr = 21;
  while(fgets(buffer, TOPLSITMAXLINELENGTH, f) != NULL && --cntr){
    num = strtol(buffer+18, NULL, 10);
    if(!added && score>num && num!=0){
      fprintf(tmp, "%-13s %2d  %-14d\n", name, level, score);
      score=0;
    }
    fputs(buffer, tmp);
  }
  if(cntr && score) fprintf(tmp, "%-13s %2d  %-14d\n", name, level, score);
  fclose(f);
  fclose(tmp);
  remove("toplist");
  rename("tmp", "toplist");
  free(buffer);
}

void updatescore(){
  //updates score
  char *tmp=malloc(sizeof *tmp * 15);
  sprintf(tmp, "%-14d", score);
  memcpy(left[7]+9, tmp, 14);
  free(tmp);
}

void updatelevel(){
  //updates level
  char *tmp = malloc(sizeof *tmp * 15);
  sprintf(tmp, "%-14d", level);
  memcpy(left[5]+9, tmp, 14);
  free(tmp);
}

void setkeybind(){
  //reads key bindings from config
    right[3][3]=toupper(MOVL);
    right[3][14]=toupper(MOVR);
    right[4][8]=toupper(ROTA);
    right[5][3]=toupper(DROP);
    right[5][14]=toupper(RSET);
    right[6][3]=toupper(SNXT);
    right[7][3]=toupper(STXT);
    right[8][3]=toupper(EXT);
}



//START

void initNextQ() {
  for(int i=0;i<maxQueueSize;i++) {
    nextQueue[i] = nexttetrimino();
  }
}


void updateNextQueue() {

  //next = nextQueue[0];
  for(int i=0;i<maxQueueSize-1;i++) {
    nextQueue[i] = nextQueue[i+1];
  }

  nextQueue[maxQueueSize-1] = nexttetrimino();
}


void showNextQueue() {
  int j = 5;
  for(int i=0;i<maxQueueSize;i++) {
    switch(nextQueue[i]){
      case 0:
        memcpy(left[j]+WIDTH-10, TetrominoI[1]+8, 8);
        j = j + 3;
        break;
      case 1:
        memcpy(left[j]+WIDTH-8, TetrominoJ[0]+8, 2);
        memcpy(left[j+1]+WIDTH-8, TetrominoJ[1]+8, 6);
        j = j + 3;
        break;
      case 2:
        memcpy(left[j]+WIDTH-4, TetrominoL[0]+12, 2);
        memcpy(left[j+1]+WIDTH-8, TetrominoL[1]+8, 6);
        j = j + 3;
        break;
      case 3:
        memcpy(left[j]+WIDTH-6, TetrominoO[0]+10, 4);
        memcpy(left[j+1]+WIDTH-6, TetrominoO[1]+10, 4);
        j = j + 3;
        break;
      case 4:
        memcpy(left[j]+WIDTH-6, TetrominoS[0]+10, 4);
        memcpy(left[j+1]+WIDTH-8, TetrominoS[1]+8, 4);
        j = j + 3;
        break;
      case 5:
        memcpy(left[j]+WIDTH-6, TetrominoT[0]+10, 2);
        memcpy(left[j+1]+WIDTH-8, TetrominoT[1]+8, 6);
        j = j + 3;
        break;
      case 6:
        memcpy(left[j]+WIDTH-8, TetrominoZ[0]+8, 4);
        memcpy(left[j+1]+WIDTH-6, TetrominoZ[1]+10, 4);
        j = j + 3;
        break;
    }
  }
}

void afterSNQ() {
  int j = 4 + 3*maxQueueSize;
  for(int i=4; i<j; i++) {
    memcpy(left[i]+WIDTH-10, "        ", 8);
  }
}

//END


void show_next(){
  //function shows next tetromino
  if(nextTrue == 0) {
       memcpy(left[10]+WIDTH-4, TetrominoZ[0]+12, 2);
       memcpy(left[11]+WIDTH-8, TetrominoZ[1]+8, 6);
       return;
  }

  showNextQueue();
}

void updatescrn(){
  //self explanatory, refreshes screen
  clear();
  printw("\n"); 
  for(int i=2; i<HEIGHT; ++i){
    if(shownext) show_next();
    printw("%s", left[i]);
    printw("%s", center[i]);
    if(showtext) printw("%s", right[i]);
    printw("\n");
  }
  refresh();
  if(shownext){
    afterSNQ();
  }
}

void toplist(){
  //show toplist
  char *buffer=malloc(sizeof *buffer * TOPLSITMAXLINELENGTH);
  FILE *f;
  clear();
  if(!(f=fopen("toplist", "r")))
    printw("\n\n\n    Toplist doesn't exist! Your score has to be higher than 0"
           " to be added ;)\n");
  else{
    printw("\n");
    while(fgets(buffer, TOPLSITMAXLINELENGTH, f) != NULL)
      printw("                        %s", buffer);
    fclose(f);
  }
  refresh();
  free(buffer);
  getch();
}

void checkclr(){
  //check if a line should be cleared
  int cleared = 0;
  if(fixedpoint[0]>2){
    for(int i=-2; i<2; ++i){
      if(!strncmp(center[fixedpoint[0]+i]+3,"][][][][][][][][][", 18)){
        ++cleared;
        for(int j=fixedpoint[0]+i; j>0; --j)
          memcpy(center[j]+2, center[j-1]+2, 20);
      }
    }
    if(cleared){
      score+=SCORE;
      score+=dropped;
      dropped = 0;
      updatescore();
      updatescrn();
    }
  }
  clrlines+=cleared;
}

void initpiece(){
  //initializes a new piece
  checkclr();

  int current;
  current = nextQueue[0];
  
  //###############
  updateNextQueue();
  //##############
  //next = rand() % 7;
  switch(current){
    case 0:
      memcpy(center[0], TetrominoI[0], WIDTH*2);
      fixedpoint[0] = 1;
      fixedpoint[1] = 12;
      piece = 'I';
      return;
    case 1:
      memcpy(center[0], TetrominoJ[0], WIDTH*2);
      fixedpoint[0] = 1;
      fixedpoint[1] = 10;
      piece = 'J';
      return;
    case 2:
      memcpy(center[0], TetrominoL[0], WIDTH*2);
      fixedpoint[0] = 1;
      fixedpoint[1] = 10;
      piece = 'L';
      return;
    case 3:
      memcpy(center[0], TetrominoO[0], WIDTH*2);
      fixedpoint[0] = 1;
      fixedpoint[1] = 10;
      piece = 'O';
      return;
    case 4:
      memcpy(center[0], TetrominoS[0], WIDTH*2);
      fixedpoint[0] = 0;
      fixedpoint[1] = 10;
      piece = 'S';
      return;
    case 5:
      memcpy(center[0], TetrominoT[0], WIDTH*2);
      fixedpoint[0] = 1;
      fixedpoint[1] = 10;
      piece = 'T';
      return;
    case 6:
      memcpy(center[0], TetrominoZ[0], WIDTH*2);
      fixedpoint[0] = 0;
      fixedpoint[1] = 10;
      piece = 'Z';
      return;
  }
}

void rotate(){
  //rotate current piece (clockwise obviously)
  switch(piece){
    case 'I':
      if(center[fixedpoint[0]+1][fixedpoint[1]]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]]=='='
         || center[fixedpoint[0]-1][fixedpoint[1]]=='['
         || center[fixedpoint[0]-2][fixedpoint[1]]=='[') return;
      memcpy(center[fixedpoint[0]-2]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-4,
             TetrominoI[0]+fixedpoint[1]-4, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 2);
      piece ='i';
      return;
    case 'i':
      if(center[fixedpoint[0]][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]][fixedpoint[1]+2]=='!'
         || center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]][fixedpoint[1]-4]=='['
         || center[fixedpoint[0]][fixedpoint[1]-4]=='<') return;
      memcpy(center[fixedpoint[0]-2]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-4,
             TetrominoI[1]+8, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[1]+12, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      piece ='I';
      return;
    case 'J':
      if(center[fixedpoint[0]+1][fixedpoint[1]]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]]=='='
         || center[fixedpoint[0]-1][fixedpoint[1]]=='['
         || center[fixedpoint[0]-1][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             TetrominoO[0]+8, 6);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 2);
      piece ='K';
      return;
    case 'K':
      if(center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]][fixedpoint[1]-2]=='<'
         || center[fixedpoint[0]][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      piece ='j';
      return;
    case 'j':
      if(center[fixedpoint[0]+1][fixedpoint[1]]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]-1][fixedpoint[1]]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoO[0]+10, 6);
      piece ='k';
      return;
    case 'k':
      if(center[fixedpoint[0]][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]][fixedpoint[1]+2]=='!'
         || center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]-1][fixedpoint[1]-2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 4);
      piece ='J';
      return;
    case 'L':
      if(center[fixedpoint[0]+1][fixedpoint[1]]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]]=='='
         || center[fixedpoint[0]-1][fixedpoint[1]]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoT[0]+10, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoO[0]+10, 4);
      piece ='M';
      return;
    case 'M':
      if(center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]][fixedpoint[1]-2]=='<'
         || center[fixedpoint[0]][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]-2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoT[0]+10, 6);
      piece ='l';
      return;
    case 'l':
      if(center[fixedpoint[0]-1][fixedpoint[1]]=='['
         || center[fixedpoint[0]-1][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]]=='[') return;
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoT[0]+8, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             TetrominoO[0]+10, 4);
      piece ='m';
      return;
    case 'm':
      if(center[fixedpoint[0]][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]][fixedpoint[1]+2]=='!'
         || center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]-1][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             TetrominoT[0]+6, 6);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      piece ='L';
      return;
    case 'S':
      if(center[fixedpoint[0]-1][fixedpoint[1]]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoT[0]+6, 6);
      piece = 's';
      return;
    case 's':
      if(center[fixedpoint[0]+1][fixedpoint[1]]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]-2]=='<'
         || center[fixedpoint[0]+1][fixedpoint[1]-2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoO[0]+10, 6);
      piece = 'S';
      return;
    case 'T':
      if(center[fixedpoint[0]+1][fixedpoint[1]]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]]=='=') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 2);
      piece ='U';
      return;
    case 'U':
      if(center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]][fixedpoint[1]-2]=='<') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      piece ='t';
      return;
    case 't':
      if(center[fixedpoint[0]-1][fixedpoint[1]]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 2);
      piece ='u';
      return;
    case 'u':
      if(center[fixedpoint[0]][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]][fixedpoint[1]+2]=='!') return;
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      piece ='T';
      return;
    case 'Z':
      if(center[fixedpoint[0]-1][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]+2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoO[0]+8, 6);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      piece = 'z';
      return;
    case 'z':
      if(center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]][fixedpoint[1]-2]=='<'
         || center[fixedpoint[0]+1][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoO[0]+10, 6);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]+2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      piece = 'Z';
      return;
  }
}

void moveleft(){
  //move current piece to the left
  switch(piece){
    case 'I':
      if(center[fixedpoint[0]][fixedpoint[1]+-6]=='['
         || center[fixedpoint[0]][fixedpoint[1]-6]=='<') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-6,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      fixedpoint[1]-=2;
      return;
    case 'i':
    if(center[fixedpoint[0]][fixedpoint[1]-2]=='<') return;
      for(int i=-2; i<2; ++i){
        if(center[fixedpoint[0]+i][fixedpoint[1]-2]=='[') return;
      }
      for(int i=-2; i<2; ++i){
        memcpy(center[fixedpoint[0]+i]+fixedpoint[1]-2,
               TetrominoT[0]+10, 4);
      }
      fixedpoint[1]-=2;
      return;
    case 'J':
      if(center[fixedpoint[0]][fixedpoint[1]-4]=='['
         || center[fixedpoint[0]][fixedpoint[1]-4]=='<'
         || center[fixedpoint[0]-1][fixedpoint[1]-4]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      fixedpoint[1]-=2;
      return;
    case 'K':
      if(center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]][fixedpoint[1]-2]=='<'
         || center[fixedpoint[0]-1][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]-2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      fixedpoint[1]-=2;
      return;
    case 'j':
      if(center[fixedpoint[0]][fixedpoint[1]-4]=='['
         || center[fixedpoint[0]][fixedpoint[1]-4]=='<'
         || center[fixedpoint[0]+1][fixedpoint[1]]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoT[0]+10, 4);
      fixedpoint[1]-=2;
      return;
    case 'k':
      if(center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]-4]=='<'
         || center[fixedpoint[0]-1][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]-4]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-4,
             center[fixedpoint[0]]+fixedpoint[1]-2, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      fixedpoint[1]-=2;
      return;
    case 'L':
      if(center[fixedpoint[0]][fixedpoint[1]-4]=='['
         || center[fixedpoint[0]][fixedpoint[1]-4]=='<'
         || center[fixedpoint[0]-1][fixedpoint[1]]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      fixedpoint[1]-=2;
      return;
    case 'M':
      if(center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]][fixedpoint[1]-2]=='<'
         || center[fixedpoint[0]-1][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]-2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoT[0]+10, 2);
      fixedpoint[1]-=2;
      return;
    case 'l':
      if(center[fixedpoint[0]][fixedpoint[1]-4]=='['
         || center[fixedpoint[0]][fixedpoint[1]-4]=='<'
         || center[fixedpoint[0]+1][fixedpoint[1]-4]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-4,
             TetrominoT[0]+10, 4);
      fixedpoint[1]-=2;
      return;
    case 'm':
      if(center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]-1][fixedpoint[1]-4]=='<'
         || center[fixedpoint[0]+1][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]-1][fixedpoint[1]-4]=='[') return;
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-4,
             center[fixedpoint[0]]+fixedpoint[1]-2, 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      fixedpoint[1]-=2;
      return;
    case 'O':
      if(center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]][fixedpoint[1]-2]=='<'
         || center[fixedpoint[0]-1][fixedpoint[1]-2]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             center[fixedpoint[0]-1]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      fixedpoint[1]-=2;
      return;
    case 'S':
      if(center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]-4]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]-4]=='<') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-4,
             center[fixedpoint[0]+1]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      fixedpoint[1]-=2;
      return;
    case 's':
      if(center[fixedpoint[0]-1][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]-1][fixedpoint[1]-2]=='<'
         || center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoO[0]+10, 6);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoT[0]+10, 4);
      fixedpoint[1]-=2;
      return;
    case 'T':
      if(center[fixedpoint[0]][fixedpoint[1]-4]=='['
         || center[fixedpoint[0]][fixedpoint[1]-4]=='<'
         || center[fixedpoint[0]-1][fixedpoint[1]-2]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      fixedpoint[1]-=2;
      return;
    case 'U':
      if(center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]][fixedpoint[1]-2]=='<'
         || center[fixedpoint[0]-1][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]-2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoO[0]+10, 6);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      fixedpoint[1]-=2;
      return;
    case 't':
      if(center[fixedpoint[0]][fixedpoint[1]-4]=='['
         || center[fixedpoint[0]][fixedpoint[1]-4]=='<'
         || center[fixedpoint[0]+1][fixedpoint[1]-2]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-4,
             TetrominoT[0]+10, 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      fixedpoint[1]-=2;
      return;
    case 'u':
      if(center[fixedpoint[0]][fixedpoint[1]-4]=='['
         || center[fixedpoint[0]][fixedpoint[1]-4]=='<'
         || center[fixedpoint[0]-1][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]-2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-4,
             TetrominoO[0]+10, 6);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      fixedpoint[1]-=2;
      return;
    case 'Z':
      if(center[fixedpoint[0]][fixedpoint[1]-4]=='['
         || center[fixedpoint[0]][fixedpoint[1]-4]=='<'
         || center[fixedpoint[0]+1][fixedpoint[1]-2]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             center[fixedpoint[0]+1]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      fixedpoint[1]-=2;
      return;
    case 'z':
      if(center[fixedpoint[0]-1][fixedpoint[1]]=='['
         || center[fixedpoint[0]][fixedpoint[1]-2]=='<'
         || center[fixedpoint[0]][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]-2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoT[0]+10, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoO[0]+10, 6);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoT[0]+10, 4);
      fixedpoint[1]-=2;
      return;
  }
}

void moveright(){
  //move current piece to the right
  switch(piece){
    case 'I':
      if(center[fixedpoint[0]][fixedpoint[1]+4]=='['
         || center[fixedpoint[0]][fixedpoint[1]+4]=='!') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-4,
             TetrominoI[0]+fixedpoint[1]-4, 2);
      fixedpoint[1]+=2;
      return;
    case 'i':
      if(center[fixedpoint[0]][fixedpoint[1]+2]=='!') return;
      for(int i=-2; i<2; ++i){
        if(center[fixedpoint[0]+i][fixedpoint[1]+2]=='[') return;
      }
      for(int i=-2; i<2; ++i){
        memcpy(center[fixedpoint[0]+i]+fixedpoint[1],
               TetrominoT[0]+8, 4);
      }
      fixedpoint[1]+=2;
      return;
    case 'J':
      if(center[fixedpoint[0]][fixedpoint[1]+4]=='['
         || center[fixedpoint[0]][fixedpoint[1]+4]=='!'
         || center[fixedpoint[0]-1][fixedpoint[1]]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      fixedpoint[1]+=2;
      return;
    case 'K':
      if(center[fixedpoint[0]-1][fixedpoint[1]+4]=='['
         || center[fixedpoint[0]-1][fixedpoint[1]+4]=='!'
         || center[fixedpoint[0]][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]+4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      fixedpoint[1]+=2;
      return;
    case 'j':
      if(center[fixedpoint[0]][fixedpoint[1]+4]=='['
         || center[fixedpoint[0]][fixedpoint[1]+4]=='!'
         || center[fixedpoint[0]+1][fixedpoint[1]+4]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]+2,
             TetrominoT[0]+8, 4);
      fixedpoint[1]+=2;
      return;
    case 'k':
      if(center[fixedpoint[0]][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]][fixedpoint[1]+2]=='!'
         || center[fixedpoint[0]-1][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]+2,
             center[fixedpoint[0]]+fixedpoint[1]+2, 2);
      fixedpoint[1]+=2;
      return;
    case 'L':
      if(center[fixedpoint[0]][fixedpoint[1]+4]=='['
         || center[fixedpoint[0]][fixedpoint[1]+4]=='!'
         || center[fixedpoint[0]-1][fixedpoint[1]+4]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]+4,
             center[fixedpoint[0]]+fixedpoint[1]+4, 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      fixedpoint[1]+=2;
      return;
    case 'M':
      if(center[fixedpoint[0]-1][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]+4]=='!'
         || center[fixedpoint[0]][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]+4]=='[') return;
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]+4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      fixedpoint[1]+=2;
      return;
    case 'l':
      if(center[fixedpoint[0]][fixedpoint[1]+4]=='['
         || center[fixedpoint[0]][fixedpoint[1]+4]=='!'
         || center[fixedpoint[0]+1][fixedpoint[1]]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoT[0]+8, 4);
      fixedpoint[1]+=2;
      return;
    case 'm':
      if(center[fixedpoint[0]][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]][fixedpoint[1]+2]=='!'
         || center[fixedpoint[0]-1][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]+2,
             center[fixedpoint[0]]+fixedpoint[1]+2, 2);
      fixedpoint[1]+=2;
      return;
    case 'O':
      if(center[fixedpoint[0]][fixedpoint[1]+4]=='['
         || center[fixedpoint[0]][fixedpoint[1]+4]=='!'
         || center[fixedpoint[0]-1][fixedpoint[1]+4]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]+4,
             center[fixedpoint[0]-1]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      fixedpoint[1]+=2;
      return;
    case 'S':
      if(center[fixedpoint[0]][fixedpoint[1]+4]=='['
         || center[fixedpoint[0]][fixedpoint[1]+4]=='!'
         || center[fixedpoint[0]+1][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]+2,
             center[fixedpoint[0]+1]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      fixedpoint[1]+=2;
      return;
    case 's':
      if(center[fixedpoint[0]-1][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]][fixedpoint[1]+4]=='!'
         || center[fixedpoint[0]][fixedpoint[1]+4]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]+4]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1],
             TetrominoO[0]+8, 6);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]+2,
             TetrominoT[0]+8, 4);
      fixedpoint[1]+=2;
      return;
    case 'T':
      if(center[fixedpoint[0]][fixedpoint[1]+4]=='['
         || center[fixedpoint[0]][fixedpoint[1]+4]=='!'
         || center[fixedpoint[0]-1][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+4,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]+2,
             center[fixedpoint[0]]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      fixedpoint[1]+=2;
      return;
    case 'U':
      if(center[fixedpoint[0]][fixedpoint[1]+4]=='['
         || center[fixedpoint[0]][fixedpoint[1]+4]=='!'
         || center[fixedpoint[0]-1][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1],
             TetrominoO[0]+8, 6);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      fixedpoint[1]+=2;
      return;
    case 't':
      if(center[fixedpoint[0]][fixedpoint[1]+4]=='['
         || center[fixedpoint[0]][fixedpoint[1]+4]=='!'
         || center[fixedpoint[0]+1][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+4,
             TetrominoT[0]+10, 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      fixedpoint[1]+=2;
      return;
    case 'u':
      if(center[fixedpoint[0]][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]][fixedpoint[1]+2]=='!'
         || center[fixedpoint[0]-1][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoO[0]+8, 6);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      fixedpoint[1]+=2;
      return;
    case 'Z':
      if(center[fixedpoint[0]][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]+4]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]+4]=='!') return;
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]+4,
             center[fixedpoint[0]+1]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      fixedpoint[1]+=2;
      return;
    case 'z':
      if(center[fixedpoint[0]-1][fixedpoint[1]+4]=='['
         || center[fixedpoint[0]][fixedpoint[1]+4]=='!'
         || center[fixedpoint[0]][fixedpoint[1]+4]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]+2]=='[') return;
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]+2,
             TetrominoT[0]+8, 4);
      memcpy(center[fixedpoint[0]]+fixedpoint[1],
             TetrominoO[0]+8, 6);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      fixedpoint[1]+=2;
      return;
  }
}


int movedown(){
  //move current piece down
  switch(piece){
    case 'I':
      if(center[fixedpoint[0]+1][fixedpoint[1]-4]=='='){
        initpiece();
        return 0;
      }
      for(int i=0; i<4; ++i){
        if(center[fixedpoint[0]+1][fixedpoint[1]-4+i*2]=='['){
          if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
          initpiece();
          return 0;
        }
      }
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-4,
             center[fixedpoint[0]]+fixedpoint[1]-4, 8);
      memcpy(center[fixedpoint[0]++]+fixedpoint[1]-4,
             TetrominoI[0]+fixedpoint[1]-4, 8);
      return 0;
    case 'i':
      if(center[fixedpoint[0]+2][fixedpoint[1]]=='['
         || center[fixedpoint[0]+2][fixedpoint[1]]=='='){
        if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
        initpiece();
        return 0;
      }
      memcpy(center[fixedpoint[0]+2]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]++ -2]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      return 0;
    case 'J':
      if(center[fixedpoint[0]+1][fixedpoint[1]-2]=='='){
        initpiece();
        return 0;
      }
      for(int i=0; i<3; ++i){
        if(center[fixedpoint[0]+1][fixedpoint[1]-2+i*2]=='['){
          if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
          initpiece();
          return 0;
        }
      }
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             center[fixedpoint[0]]+fixedpoint[1]-2, 6);
      memcpy(center[fixedpoint[0]]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 4);
      memcpy(center[fixedpoint[0]++ -1]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      return 0;
    case 'K':
      if(center[fixedpoint[0]+2][fixedpoint[1]]=='['
         || center[fixedpoint[0]+2][fixedpoint[1]]=='='
         || center[fixedpoint[0]][fixedpoint[1]+2]=='['){
        if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
        initpiece();
        return 0;
      }
      memcpy(center[fixedpoint[0]+2]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 4);
      memcpy(center[fixedpoint[0]++]+fixedpoint[1]+2,
             TetrominoT[0]+10, 2);
      return 0;
    case 'j':
      if(center[fixedpoint[0]+2][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]+2][fixedpoint[1]+2]=='='
         || center[fixedpoint[0]+1][fixedpoint[1]]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]-2]=='['){
        if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
        initpiece();
        return 0;
      }
      memcpy(center[fixedpoint[0]+2]+fixedpoint[1]+2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 6);
      memcpy(center[fixedpoint[0]++ +1]+fixedpoint[1]-2,
             TetrominoO[0]+10, 4);
      return 0;
    case 'k':
      if(center[fixedpoint[0]+2][fixedpoint[1]]=='['
         || center[fixedpoint[0]+2][fixedpoint[1]]=='='
         || center[fixedpoint[0]+2][fixedpoint[1]-2]=='['){
        if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
        initpiece();
        return 0;
      }
      memcpy(center[fixedpoint[0]+2]+fixedpoint[1]-2,
             TetrominoO[0]+10, 4);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]++ +1]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      return 0;
    case 'L':
      if(center[fixedpoint[0]+1][fixedpoint[1]-2]=='='){
        initpiece();
        return 0;
      }
      for(int i=0; i<3; ++i){
        if(center[fixedpoint[0]+1][fixedpoint[1]-2+i*2]=='['){
          if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
          initpiece();
          return 0;
        }
      }
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             center[fixedpoint[0]]+fixedpoint[1]-2, 6);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 4);
      memcpy(center[fixedpoint[0]++ -1]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      return 0;
    case 'M':
      if(center[fixedpoint[0]+2][fixedpoint[1]]=='['
         || center[fixedpoint[0]+2][fixedpoint[1]]=='='
         || center[fixedpoint[0]+2][fixedpoint[1]+2]=='['){
        if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
        initpiece();
        return 0;
      }
      memcpy(center[fixedpoint[0]+2]+fixedpoint[1],
             TetrominoO[0]+10, 4);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]++ +1]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      return 0;
    case 'l':
      if(center[fixedpoint[0]+2][fixedpoint[1]-2]=='['
         || center[fixedpoint[0]+2][fixedpoint[1]-2]=='='
         || center[fixedpoint[0]+1][fixedpoint[1]]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]+2]=='['){
        if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
        initpiece();
        return 0;
      }
      memcpy(center[fixedpoint[0]+2]+fixedpoint[1]-2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 6);
      memcpy(center[fixedpoint[0]++ +1]+fixedpoint[1],
             TetrominoO[0]+10, 4);
      return 0;
    case 'm':
      if(center[fixedpoint[0]+2][fixedpoint[1]]=='['
         || center[fixedpoint[0]+2][fixedpoint[1]]=='='
         || center[fixedpoint[0]][fixedpoint[1]-2]=='['){
        if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
        initpiece();
        return 0;
      }
      memcpy(center[fixedpoint[0]+2]+fixedpoint[1],
             TetrominoT[0]+10, 2);
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 4);
      memcpy(center[fixedpoint[0]++]+fixedpoint[1]-2,
             TetrominoT[0]+10, 2);
      return 0;
    case 'O':
      if(center[fixedpoint[0]+1][fixedpoint[1]]=='='){
        initpiece();
        return 0;
      }
      for(int i=0; i<2; ++i){
        if(center[fixedpoint[0]+1][fixedpoint[1]+i*2]=='['){
          if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
          initpiece();
          return 0;
        }
      }
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 4);
      memcpy(center[fixedpoint[0]++ -1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 4);
      return 0;
    case 'S':
      if(center[fixedpoint[0]+2][fixedpoint[1]-2]=='='){
        initpiece();
        return 0;
      }
      for(int i=0; i<2; ++i){
        if(center[fixedpoint[0]+2][fixedpoint[1]-2+i*2]=='['
           || center[fixedpoint[0]+1][fixedpoint[1]+2]=='['){
          if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
          initpiece();
          return 0;
        }
      }
      memcpy(center[fixedpoint[0]+2]+fixedpoint[1]-2,
             center[fixedpoint[0]+1]+fixedpoint[1]-2, 4);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoS[0]+8, 6);
      memcpy(center[fixedpoint[0]++]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 4);
      return 0;
    case 's':
      if(center[fixedpoint[0]+1][fixedpoint[1]]=='['
         || center[fixedpoint[0]+2][fixedpoint[1]]=='='
         || center[fixedpoint[0]+2][fixedpoint[1]+2]=='['){
        if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
        initpiece();
        return 0;
      }
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1],
             TetrominoT[0]+10, 4);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoO[0]+10, 4);
      memcpy(center[fixedpoint[0]++ +2]+fixedpoint[1]+2,
             TetrominoT[0]+10, 2);
      return 0;
    case 'T':
      if(center[fixedpoint[0]+1][fixedpoint[1]-2]=='='){
        initpiece();
        return 0;
      }
      for(int i=0; i<3; ++i){
        if(center[fixedpoint[0]+1][fixedpoint[1]-2+i*2]=='['){
          if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
          initpiece();
          return 0;
        }
      }
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             center[fixedpoint[0]]+fixedpoint[1]-2, 6);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoT[0]+8, 6);
      memcpy(center[fixedpoint[0]++ -1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      return 0;
    case 'U':
      if(center[fixedpoint[0]+2][fixedpoint[1]]=='['
         || center[fixedpoint[0]+2][fixedpoint[1]]=='='
         || center[fixedpoint[0]+1][fixedpoint[1]+2]=='['){
        if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
        initpiece();
        return 0;
      }
      memcpy(center[fixedpoint[0]+2]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]+2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1]+2, 2);
      memcpy(center[fixedpoint[0]++ -1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      return 0;
    case 't':
      if(center[fixedpoint[0]+2][fixedpoint[1]]=='['
         || center[fixedpoint[0]+2][fixedpoint[1]]=='='
         || center[fixedpoint[0]+1][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]+1][fixedpoint[1]-2]=='['){
        if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
        initpiece();
        return 0;
      }
      memcpy(center[fixedpoint[0]+2]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]+2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]++]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 6);
      return 0;
    case 'u':
      if(center[fixedpoint[0]+2][fixedpoint[1]]=='['
         || center[fixedpoint[0]+2][fixedpoint[1]]=='='
         || center[fixedpoint[0]+1][fixedpoint[1]-2]=='['){
        if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
        initpiece();
        return 0;
      }
      memcpy(center[fixedpoint[0]+2]+fixedpoint[1],
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             center[fixedpoint[0]]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 2);
      memcpy(center[fixedpoint[0]++ -1]+fixedpoint[1],
             TetrominoI[0]+fixedpoint[1], 2);
      return 0;
    case 'Z':
      if(center[fixedpoint[0]+2][fixedpoint[1]]=='='){
        initpiece();
        return 0;
      }
      for(int i=0; i<2; ++i){
        if(center[fixedpoint[0]+2][fixedpoint[1]+i*2]=='['
           || center[fixedpoint[0]+1][fixedpoint[1]-2]=='['){
          if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
          initpiece();
          return 0;
        }
      }
      memcpy(center[fixedpoint[0]+2]+fixedpoint[1],
             center[fixedpoint[0]+1]+fixedpoint[1], 4);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1]-2,
             TetrominoZ[0]+8, 6);
      memcpy(center[fixedpoint[0]++]+fixedpoint[1]-2,
             TetrominoI[0]+fixedpoint[1]-2, 4);
      return 0;
    case 'z':
      if(center[fixedpoint[0]+1][fixedpoint[1]+2]=='['
         || center[fixedpoint[0]+2][fixedpoint[1]]=='='
         || center[fixedpoint[0]+2][fixedpoint[1]]=='['){
        if(fixedpoint[0]==1 || fixedpoint[0]==2) return gameOver();
        initpiece();
        return 0;
      }
      memcpy(center[fixedpoint[0]-1]+fixedpoint[1]+2,
             TetrominoI[0]+fixedpoint[1], 2);
      memcpy(center[fixedpoint[0]]+fixedpoint[1],
             TetrominoT[0]+8, 4);
      memcpy(center[fixedpoint[0]+1]+fixedpoint[1],
             TetrominoO[0]+10, 4);
      memcpy(center[fixedpoint[0]++ +2]+fixedpoint[1],
             TetrominoT[0]+10, 2);
      return 0;
  }
  return 0;
}

void init(){
  //initializes screen
  memcpy(left[0], "                        \0"
                  "                        \0"
                  "                        \0"
                  "  PLAYER:               \0"
                  "                        \0"
                  "  LEVEL:                \0"
                  "                        \0"
                  "  SCORE:                \0"
                  "                        \0"
                  "                        \0"
                  "                        \0"
                  "                        \0"
                  "                        \0"
                  "                        \0"
                  "                        \0"
                  "                        \0"
                  "                        \0"
                  "                        \0"
                  "                        \0"
                  "                        \0"
                  "                        \0"
                  "                        \0"
                  " HIGHSCORE:             \0"
                  "                        \0", (HEIGHT)*(WIDTH));
  memcpy(center[0], "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<! . . . . . . . . . .!>\0"
                    "<!====================!>\0"
                    "  \\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\0", (HEIGHT)*(WIDTH));
  memcpy(right[0], "                        \0"
                   "                        \0"
                   "                        \0"
                   "    : LEFT     :RIGHT   \0"
                   "         :ROTATE        \0"
                   "    : DROP     :RESET   \0"
                   "    : SHOW/HIDE NEXT    \0"
                   "    : HIDE THIS TEXT    \0"
                   "    : EXIT              \0"
                   "                        \0"
                   "                        \0"
                   "                        \0"
                   "                        \0"
                   "                        \0"
                   "                        \0"
                   "                        \0"
                   "                        \0"
                   "                        \0"
                   "                        \0"
                   "                        \0"
                   "                        \0"
                   "                        \0"
                   "                        \0"
                   "                        \0", (HEIGHT)*(WIDTH));
}

int game(){
  //simulates a game of tetris
  
  /////////
  initNextQ();
  ////////

  nodelay(stdscr, FALSE);
  init();
  setkeybind();
  clear();
  end = 0;
  score = 0;
  level = startlevel;
  clrlines = 0;
  memcpy(left[3]+10, name, strlen(name));

  ///############3
  char highscore[6] = "34";
       FILE *fp = fopen("HighScore.txt", "r");
       if (fp)
       {
              fscanf(fp, "%s", highscore);
              fclose(fp);
       }
       memcpy(left[22] + 13, highscore, strlen(highscore));
  ////###############33

  printw("\n\n\n       Press any key to start\n");
  refresh();
  getch();
  updatescore();
  updatelevel();
  initpiece();
  updatescrn();
  nodelay(stdscr, TRUE);
  gettimeofday(&t1, NULL);
  while(!usleep(DELAY)){
    switch(getch()){
      case DROP:
        if(movedown()) continue;
        ++dropped;
        updatescrn();
        continue;
      case EXT:
        return 1;
      case RSET:
        return 0;
      case STXT:
        if(end) continue;
        showtext=!showtext;
        updatescrn();
        continue;
      case SNXT:
        if(end) continue;
        shownext=!shownext;
        updatescrn();
        continue;
      case MOVR:
        if(end) continue;
        moveright();
        updatescrn();
        continue;
      case MOVL:
        if(end) continue;
        moveleft();
        updatescrn();
        continue;
      case ROTA:
        if(end || fixedpoint[0]<2) continue;
        rotate();
        updatescrn();
        continue;
      case TPLS:
        if(!end) continue;
        toplist();
    }
    if(end){
      gameOver();
      continue;
    }
    if(clrlines == LINESFORLVLUP){
      if(level<MAXLEVEL){
        ++level;
        updatelevel();
        clrlines = 0;
      }
    }
    gettimeofday(&t2, NULL);
    if((((t2.tv_sec-t1.tv_sec) * 1000) + ((t2.tv_usec-t1.tv_usec)/1000))
       > DROPINTERVAL){
      if(movedown()) continue;
      updatescrn();
      gettimeofday(&t1, NULL);
    }
  }
  return 1;
}

int main(void){
  //main function
  name=malloc(sizeof *name * 14);
  srand(time(NULL));
  initscr();
  start_color();
  cbreak();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  attron(COLOR_PAIR(1));
  do{
    clear();
    printw("\n\n\n       Enter starting level (1-%d): ", MAXLEVEL);
    refresh();
    scanw("%d ", &startlevel);
  }while(startlevel<1 || startlevel>MAXLEVEL);
  clear();
  printw("\n\n\n       Enter your name: ");
  refresh();
  scanw("%13s ", name);
  noecho();
  curs_set(0);
  next=nexttetrimino();
  while(!game());
  free(name);
  endwin();
  return 0;
}
