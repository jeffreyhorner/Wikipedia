CC=gcc
CFLAGS=-ggdb3 -Wall
PROGRAMS=only_ascii mean_strlen orderfreq
OBJS=only_ascii.o mean_strlen.o orderfreq.o

all: $(PROGRAMS)
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

only_ascii: only_ascii.o
	gcc -o $@ $^ $(CFLAGS)

mean_strlen: mean_strlen.o
	gcc -o $@ $^ $(CFLAGS)

orderfreq.o: orderfreq.c
	gcc -c -o $@ $< $(CFLAGS)

orderfreq: orderfreq.o
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *~ core $(PROGRAMS) $(OBJS)

