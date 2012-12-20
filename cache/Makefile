CFLAGS:=$(shell pkg-config --cflags gtk+-2.0) -Werror -Wall
LDFLAGS:=$(shell pkg-config --libs gtk+-2.0)
TARGET:= main

all: $(TARGET)

main: utt_cache.o

clean:
	rm -f *~ *.o $(TARGET) cache-file

.PHONY: all clean
