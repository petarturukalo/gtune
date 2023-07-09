srcs=$(shell find src -name '*.c' -print)
objs=$(patsubst %.c, %.o, $(srcs))
# Objects from main source.
MOBJS=../src/note.o ../src/math.o ../src/norm.o
CC=gcc
CFLAGS=-c -g
LFLAGS=-lm -lfftw3

test: $(objs)
	$(CC) $(objs) $(MOBJS) $(LFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	find src -name '*.o' -print -delete
	rm -v test 

