all: kmeans
	

clean:
	rm *.o

kmeans:
	g++ kmeans.c++ -o kmeans.o -std=c++0x