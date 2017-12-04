#
# Makefile for TINY
# Gnu C Version
# K. Louden 2/3/98
#

CC = gcc

CFLAGS = 

OBJS = lex.yy.o  main.o util.o scan.o symtab.o analyze.o code.o cgen.o


cminus: main.o util.o scan.o symtab.o analyze.o code.o cgen.o
	$(CC) $(CFLAGS) main.o util.o scan.o symtab.o analyze.o code.o cgen.o -o  cminus

main.o: main.c globals.h util.h scan.h analyze.h cgen.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

scan.o: scan.c scan.h util.h globals.h
	$(CC) $(CFLAGS) -c scan.c

symtab.o: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

analyze.o: analyze.c globals.h symtab.h analyze.h
	$(CC) $(CFLAGS) -c analyze.c

code.o: code.c code.h globals.h
	$(CC) $(CFLAGS) -c code.c

cgen.o: cgen.c globals.h symtab.h code.h cgen.h
	$(CC) $(CFLAGS) -c cgen.c

clean:
	-rm tiny
	-rm tm
	-rm $(OBJS)

tm: tm.c
	$(CC) $(CFLAGS) tm.c -o tm
test :cminus
	-./cminus test.cm
all: cminus

#by flex
cminus_flex : $(OBJS_FLEX)
	$(CC) $(CFLAGS) main.o util.o lex.yy.o -o cminus_flex -lfl
lex.yy.o : cminus.l scan.h util.h globals.h
			 flex cminus.l
			$(CC) $(CFLAGS) -c lex.yy.c -lfl
