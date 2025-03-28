CC = gcc
LIBS = -lncurses

lv:	listview.c
	$(CC) -g -o lv listview.c $(LIBS)
