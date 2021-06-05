OBJS=build/main.o build/pcm.o build/math.o build/freq.o build/mic.o
CC=mygcc
CFLAGS=-c
LFLAGS=-lmylib -lopenal -lfftw3 -lm -lportaudio

gtune: $(OBJS)
	$(CC) $(LFLAGS) $^ -o $@

build/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm gtune build/*.o vgcore*

ctags:
	myctagslib -R . /usr/include/AL /usr/include/fftw3.h /usr/include/portaudio.h
