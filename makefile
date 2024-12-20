CC	= gcc
YACC= yacc
LEX	= flex

comp:	y.tab.c lex.yy.c ast.c comp.c cfg.c lr.c
	$(CC) lex.yy.c y.tab.c ast.c comp.c cfg.c lr.c -o comp -ll

y.tab.c: parser.y
	$(YACC) -d parser.y

lex.yy.c: scanner.l y.tab.h
	$(LEX) scanner.l

clean: 
	rm comp lex.yy.c y.tab.c y.tab.h
