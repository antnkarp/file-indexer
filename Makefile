CC=gcc
CFLAGS= -std=gnu99 -Wall -g
LDLIBS= -lpthread
all: file_indexer
file_indexer: file_indexer.c list.c file_type.c menu.c index_thread.c file_io.c signal.c
