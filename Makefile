CC = gcc
CCFLAGS= -g3
LDFLAGS = -lpng

all: test

test: test.o tmx_parser.o
	$(CC) $(CCFLAGS) -o test test.o tmx_parser.o $(LDFLAGS)

test.o: test.c
	$(CC) $(CCFLAGS) -c test.c -o test.o

tmx_parser.o: tmx_parser.c
	$(CC) $(CCFLAGS) -c tmx_parser.c -o tmx_parser.o

clean:
	rm -f *.o test