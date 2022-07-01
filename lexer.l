

%option noyywrap
%{
/* definitions of manifest constants
LT, LE, EQ, NE, GT, GE,
ID, NUMBER, RELOP, ARTHOP,
PLUS, MINUS, MUL, DIV, MOD, POW, LPAREN, RPAREN, BRAC, BITWOP, CONDOP,
LFWBRAC, RFWBRAC, LSQBRAC, RSQBRAC,
COLON, SEMICOLON, COMMA,
BITAND, BITOR, BITXOR
True, False,
AND, OR, NOT,

TETRIMINO, MINO, SHAPE, COLOR,
SPEED, LEVEL, BUFFERZONE,
DROP, ROTATE,  LEFT, RIGHT, SOFTDROP, HARDDROP, HOLD,
GAMEOVER, PAUSEMENU, RANDINT, GHOSTPIECE,
PRINT, SCAN,
LENGTH,
FUNCTION, RETURN, BREAK, CONTINUE,
VOID, INT, FLOAT, STRING, CHAR, BOOL,
LIST, HEAD,
IF, ELSE, FOR, WHILE


NEXTTETRIMINO, NEXTQUEUE, MOVE 
*/


    #include "parser.tab.h"
    #include <stdio.h>
    #include <stdlib.h>

%}

/* regular definitions */
delim   [ \t\r\n]
ws      {delim}+
letter   [A-Za-z]
digit   [0-9]
id          [a-zA-Z_][a-zA-Z0-9_]{0,31}
number  {digit}+(\.{digit}+)?(E[+-]?{digit}+)?



char        [a-zA-Z_+@#$]
string      "([^"\\]|\\.)*"






%%

{ws}    {/* no action and no return */}
"\/\/[^\n\r]+?(?:\*\)|[\n\r])" {/* no action and no return */}


 
";"     {return SEMICOLON;}
":"     {return COLON ;}
","     {return COMMA;}
"<"     { return(LT);}
"<="    { return(LE);}
"="     {return(EQU);}
"=="     {return(SAME);}
"!="    { return(NE);}
">"     { return(GT);}
">="    { return(GE);}



"+"     { return(PLUS);}
"-"     { return(MINUS);}
"*"     { return(MUL);}
"**"    { return(POW);}
"/"     { return(DIV);}
"%"     { return(MOD);}


"("    {return(LPAREN);}
")"    {  return(RPAREN);}
"["    { return(LSQBRAC);}
"]"    { return(RSQBRAC);}
"{"    { return(LFWBRAC);}
"}"    {return(RFWBRAC);}

"&&"    { return(ANDOP);}
"||"    {return(OROP);}

"&"    { return(BITAND);}
"|"    { return(BITOR);}
"^"    {return(BITXOR);}
"!"     {return(BITNOT);}

tetrimino      {return(TETRIMINO);}
mino            {return(MINO);}
shape           {return(SHAPE);}
color           {return(COLOR);}
speed           {return(SPEED);}
level           {return(LEVEL);}
block           {return(BLOCK);}
bufferzone      {return(BUFFERZONE);}
drop            {return(DROP);}
rotate          {return(ROTATE);}
gameover        {return(GAMEOVER);}
pausemenu       {return(PAUSEMENU);}
rand        {return(RANDINT);}
harddrop        {return(HARDDROP);}
hold            {return(HOLD);}
ghostpiece      {return(GHOSTPIECE);}
softdrop        {return(SOFTDROP);}
print           {return(PRINT);}
scan            {return(SCAN);}
length          {return(LENGTH);}
function        {return(FUNCTION);}
return          {return(RETURN);}
break           {return(BREAK);}
continue        {return(CONTINUE);}
void            {return(VOID);}
int             {return INT;}
float           {return(FLOAT);}
string          {return(STRING);}
char            {return(CHAR);}
bool            {return(BOOL);}
list            {return(LIST);}
head            {return(HEAD);}
if              {return(IF);}
else            {return(ELSE);}
for             {return(FOR);}
while           {return(WHILE);}
left            {return(LEFT);}
right           {return(RIGHT);}
nexttetrimino   {return(NEXTTETRIMINO);}
nextqueue       {return(NEXTQUEUE);}
move            {return(MOVE);}

{id}     {yylval.ival = atoi(yytext); return ID;}
{number} {return NUMBER;}

. {fprintf(yyout,"Use of unmatched character: %c\n", *yytext);}

%%



