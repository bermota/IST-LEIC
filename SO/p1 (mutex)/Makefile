# Makefile, versao 2
# Sistemas Operativos, DEI/IST/ULisboa 2019-20

CC   = gcc
LD   = gcc
CFLAGS =-Wall -std=gnu99 -I../ -pthread
LDFLAGS=-lm

# A phony target is one that is not really the name of a file
# https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
.PHONY: all

all: tecnicofs-mutex tecnicofs-rwlock tecnicofs-nosync

lib/bst.o: lib/bst.c lib/bst.h
	$(CC) $(CFLAGS) -o lib/bst.o -c lib/bst.c

fs.o: fs.c fs.h lib/bst.h
	$(CC) $(CFLAGS) -o fs.o -c fs.c

multithreading-mutex.o: multithreading.c
	$(CC) -DMUTEX $(CFLAGS) -o multithreading-mutex.o -c multithreading.c

multithreading-rwlock.o: multithreading.c
	$(CC) -DRWLOCK $(CFLAGS) -o multithreading-rwlock.o -c multithreading.c

multithreading-nosync.o: multithreading.c
	$(CC) $(CFLAGS) -o multithreading-nosync.o -c multithreading.c

main-mutex.o: main.c fs.h lib/bst.h multithreading-mutex.o fs.o
	$(CC) $(CFLAGS) -DMUTEX $(LDFLAGS) -o main-mutex.o -c main.c

main-rwlock.o: main.c fs.h lib/bst.h multithreading-rwlock.o fs.o
	$(CC) $(CFLAGS) -DRWLOCK $(LDFLAGS) -o main-rwlock.o -c main.c

main-nosync.o: main.c fs.h lib/bst.h multithreading-nosync.o fs.o
	$(CC) $(CFLAGS) -o main-nosync.o -c main.c

clean: 
	@echo Cleaning...
	rm -f lib/*.o *.o tecnicofs-mutex tecnicofs-rwlock tecnicofs-nosync 

tecnicofs-mutex: lib/bst.o fs.o main-mutex.o multithreading-mutex.o
	$(LD) $(CFLAGS) -DMUTEX $(LDFLAGS) -o tecnicofs-mutex lib/bst.o fs.o main-mutex.o multithreading-mutex.o 

tecnicofs-rwlock: lib/bst.o fs.o main-rwlock.o multithreading-rwlock.o
	$(LD) $(CFLAGS) $(LDFLAGS) -DRWLOCK -o tecnicofs-rwlock lib/bst.o fs.o main-rwlock.o multithreading-rwlock.o

tecnicofs-nosync: lib/bst.o fs.o main-nosync.o multithreading-nosync.o
	$(LD) $(CFLAGS) $(LDFLAGS) -o tecnicofs-nosync lib/bst.o fs.o main-nosync.o multithreading-nosync.o

