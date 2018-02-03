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

void print_help();

vector<Centroids> randomCentroids(int numFeatures, int k);

map<Point, Centroid> findNearestCentroids(DataSet dataSet, vector<Centroid> centroids);

vector<Centroid> averageLabeledCentroids(DataSet dataSet, map<Point, Centroid> labels, int k);

bool converged(vector<Centroid> centroids, vector<Centroid> oldCentroids);

#endif