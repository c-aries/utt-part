CFLAGS:=$(shell pkg-config --cflags gtk+-2.0) -Werror -Wall
LDFLAGS:=$(shell pkg-config --libs gtk+-2.0)
TARGET:= main

all: $(TARGET)

install:
	mkdir -p /usr/share/gnome/help/test/zh_CN

uninstall:
	rm -R /usr/share/gnome/help/test/zh_CN

clean:
	rm -f *~ *.o $(TARGET)

.PHONY: all clean
