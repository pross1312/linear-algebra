CC=g++
LIBS=
CFLAGS=-Wall -Wextra -pedantic -Wswitch-enum -Wswitch -ggdb

# test: Test.cpp
# 	$(CC) -o test Test.cpp $(CFLAGS)

k-mean: k-means.cpp
	$(CC) -o k-mean k-means.cpp $(CFLAGS)
