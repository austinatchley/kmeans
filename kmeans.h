#ifndef KMEANS
#define KMEANS

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <iterator>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


/*
* Classes
*/

class Point {
public:
  float x;
  float y;
};

class DataSet {
public:
  numFeatures();
};

/*
* Function Prototypes
*/

void print_help();

vector<Point> randomCentroids(int numFeatures, int k);

map<Point, Point> findNearestCentroids(DataSet dataSet, vector<Point> centroids);

vector<Point> averageLabeledCentroids(DataSet dataSet, map<Point, Point> labels, int k);

bool converged(vector<Point> centroids, vector<Point> oldCentroids);

#endif