CFLAGS=-g  -Wall --std=c++11 -o2

all: test

test:test.o
	g++ ${CFLAGS} -o $@ $+

.cpp.o:
	g++ -c ${CFLAGS} -o $@ $+

clean:
	rm -f *.o test out.txt hash/*.o
