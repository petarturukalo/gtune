OBJS=build/main.o build/math.o build/freq.o build/mic.o build/win.o \
     build/note.o build/gtune.o build/file.o build/hps.o build/sig.o
CC=gcc
CFLAGS=-c
LFLAGS=-lfftw3 -lm -lportaudio

gtune: $(OBJS)
	$(CC) $(LFLAGS) $^ -o $@

build/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm gtune build/*.o

ctags:
	myctags -R . /usr/include/fftw3.h /usr/include/portaudio.h

install:
	cp -vf gtune /usr/local/bin
