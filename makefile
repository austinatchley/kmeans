all: kmeans
	

clean:
	rm *.out

kmeans:
	g++ kmeans.c++ -o kmeans.out -std=c++0x

format:
	clang-format -i kmeans.c++
	clang-format -i kmeans.h

run: kmeans
	./kmeans.out -c 10 -t 0.0000001 -i 20 -w 1 -I "sample.in"

add: format
	git add .
