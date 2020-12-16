CC=gcc
CFLAGS= -std=gnu99 -Wall
LDLIBS= -lpthread
all: mole
mole: mole.c list.c file_type.c menu.c index_thread.c file_io.c

