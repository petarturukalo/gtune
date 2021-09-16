# TODO automate collecting object files instead of listing
OBJS=build/test.o build/test-note.o build/test-freq.o build/test-math.o
# Objects from main source.
MOBJS=../build/note.o ../build/freq.o ../build/win.o ../build/math.o \
      ../build/file.o ../build/hps.o
CC=gcc
CFLAGS=-c -g
LFLAGS=-lm -lfftw3

test: $(OBJS)
	$(CC) $(OBJS) $(MOBJS) $(LFLAGS) -o $@

build/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm test build/*.o vgcore*

ctags:
	ctags -R ..
