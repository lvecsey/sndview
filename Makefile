
CC=gcc

CFLAGS=-O2 -Wall -g -pg

sndview : sndview.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)
