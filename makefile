CC=g++
LIBS=
CFLAGS=-Wall -Wextra -pedantic -Wswitch-enum -Wswitch -Ofast

k-mean: k-means.cpp lin_al.h
	$(CC) -o k-mean k-means.cpp $(CFLAGS)
