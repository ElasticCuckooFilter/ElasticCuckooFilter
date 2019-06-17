CFLAGS=-g  -Wall --std=c++11 -o2

all: syn_dat_gen

syn_dat_gen:syn_dat_gen.o
	g++ ${CFLAGS} -o $@ $+

.cpp.o:
	g++ -c ${CFLAGS} -o $@ $+

clean:
	rm -f *.o syn_dat_gen syn_out.txt hash/*.o
