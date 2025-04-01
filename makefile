CC = gcc
LIBS = -lncurses

lv:	listview.o listfuncs.o
	$(CC) -g -o lv $^ $(LIBS)

listview.o: listview.c listfuncs.h
	$(CC) -c -g -o $@ listview.c

listfuncs.o: listfuncs.c listfuncs.h
	$(CC) -c -g -o $@ listfuncs.c

test: $(TESTS) listfuncs_test
	echo "Test executables: $(TESTS)"
	for T in $(TESTS); do ./"$$T"; done

listfuncs_test: listfuncs_test.o listfuncs.o
	$(CC) -g -o $@ $^ $(LIBS)
TESTS += listfuncs_test

listfuncs_test.o: listfuncs_test.c 
	$(CC) -c -g -o $@ listfuncs_test.c 


