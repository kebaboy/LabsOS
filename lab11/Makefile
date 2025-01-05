.PHONY: all rwlock clean

all: rwlock

rwlock: main.c
	gcc main.c -o rwlock -pthread

clean:
	rm -rf rwlock