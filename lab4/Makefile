.PHONY: all mychmod run clean

all: run

mychmod: main.c
	gcc main.c -o mychmod

run: mychmod
	./mychmod

clean:
	rm -f  mychmod