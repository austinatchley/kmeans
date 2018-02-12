V=-2
CFLAGS=
export V
export CFLAGS


all: kmeans kmeans-2
	

clean:
	rm *.out

kmeans:
	g++ $(CFLAGS) -Ofast kmeans.c++ -o kmeans.out -std=c++0x

kmeans-2:
	g++ $(CFLAGS) -Ofast kmeans-2.c++ -o kmeans-2.out -std=c++0x -pthread

format:
	clang-format -i kmeans.c++
	clang-format -i kmeans.h

run: kmeans$(V)
	./kmeans$(V).out -c 4 -t 0.0000001 -i 20 -w 1 -I "sample/sample.in"

debug:
	g++ -g -Og kmeans-2.c++ -o kmeans-2.out --std=c++0x -pthread
	gdb --args ./kmeans-2.out -c 4 -t 0.0000001 -i 20 -w 1 -I "sample/sample.in"

add: format
	git add .
