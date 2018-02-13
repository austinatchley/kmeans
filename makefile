V=-3
CFLAGS=
export V
export CFLAGS


all: kmeans kmeans-2 kmeans-3
	

clean:
	rm *.out

kmeans:
	g++ $(CFLAGS) -O3 kmeans.c++ -o kmeans.out -std=c++0x

kmeans-2:
	g++ $(CFLAGS) -O3 kmeans-2.c++ -o kmeans-2.out -std=c++0x -pthread

kmeans-3:
	g++ $(CFLAGS) -O3 kmeans-3.c++ -o kmeans-3.out -std=c++0x -pthread

format:
	clang-format -i kmeans.c++ kmeans-2.c++ kmeans-3.c++
	clang-format -i kmeans.h kmeans-2.h

run: kmeans$(V)
	./kmeans$(V).out -c 16 -t 0.0000001 -i 20 -w 4 -I "sample/sample.in"

debug:
	g++ -g -Og kmeans$(V).c++ -o kmeans$(V).out --std=c++0x -pthread
	gdb --args ./kmeans$(V).out -c 16 -t 0.0000001 -i 20 -w 4 -I "sample/sample.in"

add: format yapf
	git add .

yapf:
	yapf -i timer_harness.py harness.py 
