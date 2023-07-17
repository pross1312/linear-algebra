CC=g++
LIBS=
CFLAGS=-Wall -Wextra -pedantic -Wswitch-enum -Wswitch -ggdb

# test: Test.cpp
# 	$(CC) -o test Test.cpp $(CFLAGS)

# test2: test2.cpp lin_al_test.h
# 	$(CC) -o test2 test2.cpp $(CFLAGS)

k-mean: k-means.cpp
	$(CC) -o k-mean k-means.cpp $(CFLAGS)
