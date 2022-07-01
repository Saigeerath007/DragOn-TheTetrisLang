#include <stdio.h>
#include <stdlib.h>
#include "parser.tab.h"

extern FILE *yyin;

int main(){
    char filename[256]  = "test.drag";
    printf("Enter your filepath : ") ;
    // fgets(filename, 256, stdin) ;
    scanf("%s", filename) ;
    yyin = fopen(filename, "r");
    do{
        yyparse();
        //printf("%d\n", yylval.ival);
    }while(!feof(yyin));
    fclose(yyin);
    return 0;
}

void yyerror(char *s)
{
    fprintf(stderr, "%s\n", s);
}
