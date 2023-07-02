CC=g++
LIBS=
CFLAGS=-Wall -Wextra -pedantic -Wswitch-enum -Wswitch -ggdb
SRC=main.cpp
main: main.cpp lin_al.h
	$(CC) -o main $(SRC) $(CFLAGS)
