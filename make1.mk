
parse : lexer.l parser.y
	bison -d parser.y
	flex lexer.l
	gcc main.c parser.tab.c lex.yy.c -o parse -lm
	./parse
	python3 translator.py mygame.drag
clean :
	rm -f *.o *.tab.c *.tab.h parse
