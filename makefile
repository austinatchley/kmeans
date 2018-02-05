all: kmeans
	

clean:
	rm *.out

kmeans:
	g++ kmeans.c++ -o kmeans.out -std=c++0x

format:
	clang-format -i kmeans.c++
	clang-format -i kmeans.h
