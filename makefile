V=-2
export V

all: kmeans kmeans-2
	

clean:
	rm *.out

kmeans:
	g++ kmeans.c++ -o kmeans.out -std=c++0x

kmeans-2:
	g++ kmeans-2.c++ -o kmeans-2.out -std=c++0x -pthread

format:
	clang-format -i kmeans.c++
	clang-format -i kmeans.h

run: kmeans kmeans-2
	./kmeans$(V).out -c 4 -t 0.0000001 -i 20 -w 1 -I "sample/sample.in"

debug:
	g++ -g kmeans.c++ -o kmeans.out --std=c++0x
	gdb --args ./kmeans.out -c 4 -t 0.0000001 -i 20 -w 1 -I "sample.in"

add: format
	git add .
