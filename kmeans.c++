#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <iterator>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "kmeans.h"

using namespace std;

int clusters;
int max_iterations;
float threshold;
int workers;
string input;

int iterations;

int main(int argc, char *argv[]) {

  threshold = 0.0000001f;
  max_iterations = 0;

  int opt;
  while ((opt = getopt(argc, argv, "hc:t:i:w:I:")) != -1) {
    switch(opt) {
      case 'h':
        print_help();
        exit(0);
        break;
      case 'c':
        clusters = atoi(optarg);
        break;
      case 't':
        threshold = atof(optarg);
        break;
      case 'i':
        max_iterations = atoi(optarg);
        break;
      case 'w':
        workers = atoi(optarg);
        break;
      case 'I':
        input = optarg;
        break;
    }
  }
}

void kmeans(DataSet dataSet, int k) {
  iterations = 0;

  int numFeatures = dataSet.numFeatures();
  vector<Centroid> centroids = randomCentroids(numFeatures, k);

  vector<Centroid> oldCentroids = null;
  bool done = false;
  map<Point, Centroid> labels;

  while(!done) {
    oldCentroids = centroids;
    iterations++;

    labels = findNearestCentroids(dataSet, centroids);

    centroids = averageLabeledCentroids(dataSet, labels, k);
    done = iterations > max_iterations || converged(centroids, oldCentroids);
  }
}

vector<Centroids> randomCentroids(int numFeatures, int k) {

}

map<Point, Centroid> findNearestCentroids(DataSet dataSet, vector<Centroid> centroids) {
  return null;
}

vector<Centroid> averageLabeledCentroids(DataSet dataSet, map<Point, Centroid> labels, int k) {
  return null;
}

bool converged(vector<Centroid> centroids, vector<Centroid> oldCentroids) {
  return true; 
}

void print_help() {
  cout << "Format: " << endl
    << "kmeans -c clusters -t threshold -i iterations -w workers -I path/to/input"
    << endl;
}