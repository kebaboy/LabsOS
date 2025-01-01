.PHONY: all mygrep run clean

all: run

mygrep: main.c
	gcc main.c -o mygrep

run: mygrep
	./mygrep

clean:
	rm -f mygrep
