# Makefile, versao 2
# Sistemas Operativos, DEI/IST/ULisboa 2019-20

CC   = gcc
LD   = gcc
CFLAGS =-Wall -std=gnu99 -I../ -pthread -DRWLOCK
LDFLAGS=-lm
TARGET = tecnicofs-rwlock

# A phony target is one that is not really the name of a file
# https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
.PHONY: all

all: tecnicofs-rwlock

lib/bst.o: lib/bst.c lib/bst.h
	$(CC) $(CFLAGS) -o lib/bst.o -c lib/bst.c

lib/inodes.o: lib/inodes.c lib/inodes.h
	$(CC) $(CFLAGS) -o lib/inodes.o -c lib/inodes.c

fs.o: fs.c fs.h lib/bst.h
	$(CC) $(CFLAGS) -o fs.o -c fs.c

sync.o: sync.c sync.h
	$(CC) $(CFLAGS) -o sync.o -c sync.c

server.o: server.c server.h
	$(CC) $(CFLAGS) -o server.o -c server.c

$(TARGET): lib/bst.o lib/inodes.o fs.o sync.o server.o
	$(LD) $(CFLAGS) $(LDFLAGS) -o $(TARGET) lib/bst.o lib/inodes.o fs.o sync.o server.o

clean: 
	@echo Cleaning...
	rm -f lib/*.o *.o $(TARGET)