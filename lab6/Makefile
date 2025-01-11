.PHONY: all mypipe myfifo clean

all: mypipe myfifo

mypipe: pipe.c
	gcc -o mypipe pipe.c

myfifo: fifo.c
	gcc -o myfifo fifo.c

clean:
	rm -f myfifo mypipe