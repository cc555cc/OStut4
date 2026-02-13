CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -Wpedantic

OBJS=jeopardy.o questions.o players.o

all: jeopardy

jeopardy: $(OBJS)
	$(CC) $(CFLAGS) -o jeopardy $(OBJS)

jeopardy.o: jeopardy.c jeopardy.h questions.h players.h
	$(CC) $(CFLAGS) -c jeopardy.c

questions.o: questions.c questions.h
	$(CC) $(CFLAGS) -c questions.c

players.o: players.c players.h
	$(CC) $(CFLAGS) -c players.c

clean:
	rm -f $(OBJS) jeopardy
