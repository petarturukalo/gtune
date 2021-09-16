srcs=$(shell find src -name "*.c" -print)
objs=$(patsubst src/%.c, build/%.o, $(srcs))
CC=gcc
CFLAGS=-c
LFLAGS=-lfftw3 -lm -lportaudio

gtune: $(objs)
	$(CC) $(LFLAGS) $^ -o $@

# All header files reside in the same directory as its corresponding
# .c file, but might not exist for some files, so use wildcard since it 
# expands to nothing if it doesn't exist.
.SECONDEXPANSION:
build/%.o: src/%.c $$(wildcard src/%.h)
	$(CC) $(CFLAGS) $< -o $@


clean:
	rm gtune build/*.o

install:
	cp -vf gtune /usr/local/bin

uninstall:
	rm /usr/local/bin/gtune

