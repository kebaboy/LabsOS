.PHONY: all mycat run clean

all: run

mycat: main.c
	gcc main.c -o mycat

run: mycat
	./mycat -E main.c

clean:
	rm -f mycat
