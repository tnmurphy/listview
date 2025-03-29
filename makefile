CC = gcc
LIBS = -lncurses

lv:	listview.c listfuncs.c
	$(CC) -c -g -o listview.o listview.c
	$(CC) -c -g -o listfuncs.o listfuncs.c
	$(CC) -g -o lv listview.o listfuncs.o $(LIBS)
