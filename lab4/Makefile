.PHONY: all mychmod run clean

all: run

mychmod: main.c
	gcc main.c -o mychmod

run: mychmod
	./mychmod u-r test.txt

clean:
	rm -f  mychmod