# TODO automate collecting object files instead of listing
OBJS=build/test.o build/test-note.o build/test-math.o build/test-norm.o
# Objects from main source.
MOBJS=../build/note.o ../build/win.o ../build/math.o \
      ../build/file.o ../build/norm.o
CC=gcc
CFLAGS=-c -g
LFLAGS=-lm -lfftw3

test: $(OBJS)
	$(CC) $(OBJS) $(MOBJS) $(LFLAGS) -o $@

build/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm test build/*.o

