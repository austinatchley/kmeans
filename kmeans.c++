#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <iterator>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "kmeans.h"

using namespace std;

/*
* Global Vars
*/

int clusters;
int max_iterations;
float threshold;
int workers;
string input;

int iterations;

/*
* Function Prototypes
*/

void kmeans(DataSet dataSet, int k);

vector<Point> randomCentroids(int numFeatures, int k);
map<Point, Point> findNearestCentroids(DataSet dataSet, vector<Point> centroids);
vector<Point> averageLabeledCentroids(DataSet dataSet, map<Point, Point> labels, int k);
bool converged(vector<Point> centroids, vector<Point> oldCentroids);

void print_help();
DataSet readFile(string filePath);

/*
* Functions
*/

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
        assert(optarg);
        clusters = atoi(optarg);
        break;
      
      case 't':
        assert(optarg);
        threshold = atof(optarg);
        break;
      
      case 'i':
        assert(optarg);
        max_iterations = atoi(optarg);
        break;
      
      case 'w':
        assert(optarg);
        workers = atoi(optarg);
        break;
      
      case 'I':
        assert(optarg);
        input = optarg;
        break;
    }
  }

  DataSet dataSet = readFile(input);
  kmeans(dataSet, clusters);
}

void kmeans(DataSet dataSet, int k) {
  iterations = 0;

  int numFeatures = dataSet.numFeatures();
  vector<Point> centroids = randomCentroids(numFeatures, k);

  vector<Point> oldCentroids;
  bool done = false;
  map<Point, Point> labels;

  while(!done) {
    oldCentroids = centroids;
    iterations++;

    labels = findNearestCentroids(dataSet, centroids);

    centroids = averageLabeledCentroids(dataSet, labels, k);
    done = iterations > max_iterations || converged(centroids, oldCentroids);
  }
}

vector<Point> randomCentroids(int numFeatures, int k) {
  vector<Point> v;

  for(int i = 0; i < numFeatures; ++i) {
    Point p(i,1-i);
    v.push_back(p);
  }

  return v;
}

map<Point, Point> findNearestCentroids(DataSet dataSet, std::vector<Point> centroids) {
  map<Point, Point> m;
  return m;
}

vector<Point> averageLabeledCentroids(DataSet dataSet, map<Point, Point> labels, int k) {
  vector<Point> v;
  return v;
}

bool converged(vector<Point> centroids, vector<Point> oldCentroids) {
  return true; 
}


/*
* Class Functions
*/

int DataSet::numFeatures(){
  return points.size();
}


/*
* Utility Functions
*/

DataSet readFile(string filePath) {
  DataSet ds;

  vector<float> v;

  ifstream inFile;
  inFile.open(filePath);

  if (!inFile) {
    cerr << "Unable to open input file";
    exit(1);   // call system to stop
  }

  int size;
  inFile >> size;

  float num;
  while (inFile >> num)
    v.push_back(num);

  inFile.close();

  /*cout << "vector v of size" << v.size() << " contains:";
  for (unsigned i = 0; i < v.size(); ++i)
    cout << ' ' << v[i] << endl;
  cout << endl;*/


  return ds;
}

void print_help() {
  cout << "Format: " << endl
    << "kmeans -c clusters -t threshold -i iterations -w workers -I path/to/input"
    << endl;
}