srcs=$(shell find src -name '*.c' -print)
objs=$(patsubst %.c, %.o, $(srcs))
deps=$(patsubst %.o, %.d, $(objs))
CC=gcc
CFLAGS=-c -g
LDLIBS=-lfftw3 -lm -lportaudio

-include $(deps)

gtune: $(objs)
	$(CC) $^ $(LDLIBS) -o $@

%.o: %.c 
	$(CC) $(CFLAGS) -MMD $< -o $@


clean:
	find src -name '*.[od]' -print -delete
	rm gtune

install:
	sudo cp -v gtune /usr/bin

uninstall:
	sudo rm -v /usr/bin/gtune

