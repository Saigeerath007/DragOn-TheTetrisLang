%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "parser.tab.h"
extern int yylex();
extern int yyparse();
void yyerror(const char *s);
%}

%union
{
	int ival;
}

%token ID NUMBER RELOP ARTHOP
%token LT LE EQU NE GT GE SAME
%token PLUS MINUS MUL DIV MOD POW LPAREN RPAREN BRAC BITWOP CONDOP
%token OROP ANDOP
%token LFWBRAC RFWBRAC LSQBRAC RSQBRAC
%token COLON SEMICOLON COMMA
%token BITAND BITOR BITXOR BITNOT

%token AND OR NOT

%token TETRIMINO MINO SHAPE COLOR
%token SPEED LEVEL BUFFERZONE
%token DROP ROTATE  LEFT RIGHT SOFTDROP HARDDROP HOLD
%token GAMEOVER PAUSEMENU RANDINT GHOSTPIECE
%token PRINT SCAN
%token LENGTH
%token FUNCTION RETURN BREAK CONTINUE
%token VOID INT FLOAT STRING CHAR BOOL
%token LIST HEAD
%token IF ELSE FOR WHILE


%token NEXTTETRIMINO NEXTQUEUE MOVE BLOCK 





%start translation_unit

%%

primary_expression
	: ID
	| NUMBER
	| STRING
	| LPAREN expression RPAREN
	| BITNOT NUMBER
	| function_call
	;



multiplicative_expression
	: primary_expression
	| multiplicative_expression MUL primary_expression
	| multiplicative_expression DIV primary_expression
	| multiplicative_expression MOD primary_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression PLUS multiplicative_expression
	| additive_expression MINUS multiplicative_expression
	;
relational_expression
	: additive_expression
	| relational_expression LT additive_expression
	| relational_expression GT additive_expression
	;

equality_expression
	: relational_expression
	| equality_expression SAME relational_expression
	| equality_expression NE relational_expression
	;

and_expression
	: equality_expression
	| and_expression BITAND equality_expression
	;

logical_and_expression
	: and_expression
	| logical_and_expression ANDOP and_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OROP logical_and_expression
	;

conditional_expression
	: logical_or_expression

assignment_expression
	: conditional_expression
	| primary_expression assignment_operator assignment_expression
	;

assignment_operator
	: EQU
	;

expression
	: assignment_expression
	| expression COMMA assignment_expression
	;

constant_expression
	: conditional_expression
	;

declaration
	: declaration_specifiers SEMICOLON
	| declaration_specifiers init_declarator_list SEMICOLON
    | init_declarator_list SEMICOLON
	;

declaration_specifiers
	: type_specifier
	| RANDINT	
	| type_specifier declaration_specifiers
	;

init_declarator_list
	: init_declarator
	| init_declarator_list COMMA init_declarator
	;

init_declarator
	: direct_declarator
	| direct_declarator EQU initializer
	;

type_specifier
	: VOID
	| CHAR
	| INT
	| FLOAT
	| BOOL
	| STRING
	;

direct_declarator
	: ID
	| NEXTTETRIMINO
	| ROTATE
	| MOVE
	| GAMEOVER
	| PRINT
	| LPAREN direct_declarator RPAREN
	| direct_declarator LPAREN parameter_type_list RPAREN
	| direct_declarator LPAREN primary_expression_list RPAREN
	| direct_declarator LPAREN RPAREN
	;

function_call
	: direct_declarator
	| BITNOT function_call
	;

parameter_type_list
	: parameter_list
	;

parameter_list
	: parameter_declaration
	| parameter_list COMMA parameter_declaration
	;

parameter_declaration
	: declaration_specifiers direct_declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;


primary_expression_list
	: primary_expression
	| primary_expression_list COMMA primary_expression
	;

abstract_declarator
	: direct_abstract_declarator
	;

direct_abstract_declarator
	: LPAREN abstract_declarator RPAREN
	| LSQBRAC RSQBRAC
	| LSQBRAC constant_expression RSQBRAC
	| direct_abstract_declarator LSQBRAC RSQBRAC
	| direct_abstract_declarator LSQBRAC constant_expression RSQBRAC
	| LPAREN RPAREN
	| LPAREN parameter_type_list RPAREN
	| direct_abstract_declarator LPAREN RPAREN
	| direct_abstract_declarator LPAREN parameter_type_list RPAREN
	;

initializer
	: assignment_expression
	| LFWBRAC initializer_list RFWBRAC
	| LFWBRAC initializer_list COMMA RFWBRAC
	;

initializer_list
	: initializer
	| initializer_list COMMA initializer
	;

statement
	: labeled_statement
	| compound_statement
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	| function_call
	;

labeled_statement
	: ID COLON statement
	;

compound_statement
	: LFWBRAC RFWBRAC
	| LFWBRAC statement_list RFWBRAC
	| LFWBRAC declaration_list RFWBRAC
	| LFWBRAC declaration_list statement_list RFWBRAC
	| SEMICOLON
	;


declaration_list
	: declaration
	| declaration_list declaration
	;

statement_list
	: statement
	| statement_list statement
	;

expression_statement
	: expression SEMICOLON
	;

selection_statement
	: IF LPAREN expression RPAREN statement
	| IF LPAREN expression RPAREN statement ELSE statement
	| IF LPAREN function_call RPAREN statement ELSE statement
	| IF LPAREN function_call RPAREN statement
	;

iteration_statement
	: WHILE LPAREN expression RPAREN statement
	;

jump_statement
	: CONTINUE SEMICOLON
	| BREAK SEMICOLON
	| RETURN SEMICOLON
	| RETURN expression SEMICOLON
	;

translation_unit
	: external_declaration
	| translation_unit external_declaration
	|
	;

external_declaration
	: function_definition
	| declaration
	;


function_definition
	: declaration_specifiers direct_declarator declaration_list compound_statement
	| declaration_specifiers direct_declarator compound_statement
	| direct_declarator declaration_list compound_statement
	| direct_declarator compound_statement
	;


%%



