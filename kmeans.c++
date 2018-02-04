#include <assert.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <vector>

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

vector<Point> randomCentroids(int numFeatures, int k, int dimensions);
map<Point, Point> findNearestCentroids(DataSet dataSet,
                                       vector<Point> centroids);
vector<Point> averageLabeledCentroids(DataSet dataSet, map<Point, Point> labels,
                                      int k);
bool converged(vector<Point> centroids, vector<Point> oldCentroids);

void print_help();
DataSet &readFile(DataSet &ds, string filePath);

/*
* Class Functions
*/

int Point::getDimensions() { return this->vals.size(); }

int DataSet::numFeatures() { return this->points.size(); }

void DataSet::setPoints(vector<Point> points) { this->points = points; }

vector<Point> DataSet::getPoints() { return this->points; }

int DataSet::getDimensions() { return this->points[0].getDimensions(); }

/*
* Functions
*/

int main(int argc, char *argv[]) {

  threshold = 0.0000001f;
  max_iterations = 0;

  int opt;
  while ((opt = getopt(argc, argv, "hc:t:i:w:I:")) != -1) {
    switch (opt) {
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

  DataSet dataSet;
  readFile(dataSet, input);

  vector<Point> points = dataSet.getPoints();
  vector<float> vals = points[0].vals;

  kmeans(dataSet, clusters);
}

void kmeans(DataSet dataSet, int k) {
  iterations = 0;

  int numFeatures = dataSet.numFeatures();
  vector<Point> centroids =
      randomCentroids(numFeatures, k, dataSet.getDimensions());

  vector<Point> oldCentroids;
  bool done = false;
  map<Point, Point> labels;

  while (!done) {
    oldCentroids = centroids;
    iterations++;

    labels = findNearestCentroids(dataSet, centroids);

    // centroids = averageLabeledCentroids(dataSet, labels, k);
    done = iterations > max_iterations || converged(centroids, oldCentroids);
  }
}

vector<Point> randomCentroids(int numFeatures, int k, int dimensions) {
  vector<Point> centroids;

  for (int i = 0; i < numFeatures; ++i) {
    vector<float> vals;
    for (int j = 0; j < dimensions; ++j)
      vals.push_back(((float)rand()) / RAND_MAX);

    Point p(vals);
    centroids.push_back(p);
  }

  return centroids;
}

map<Point, Point> findNearestCentroids(DataSet dataSet,
                                       std::vector<Point> centroids) {
  map<Point, Point> m;
  return m;
}

vector<Point> averageLabeledCentroids(DataSet dataSet, map<Point, Point> labels,
                                      int k) {
  vector<Point> v;
  return v;
}

bool converged(vector<Point> centroids, vector<Point> oldCentroids) {
  assert(centroids.size() == oldCentroids.size());
  assert(centroids[0].vals.size() == oldCentroids[0].vals.size());

  for (int i = 0; i < centroids.size(); ++i) {
    Point curPoint = centroids[i];
    Point oldPoint = oldCentroids[i];
    for (int j = 0; j < curPoint.vals.size(); ++j)
      if (curPoint.vals[j] - oldPoint.vals[j] >
          threshold) // if any haven't converged, not done
        return false;
  }

  return true;
}

/*
* Utility Functions
*/

DataSet &readFile(DataSet &ds, string filePath) {
  vector<Point> points;

  ifstream inFile;
  inFile.open(filePath);

  if (!inFile) {
    cerr << "Unable to open input file";
    exit(1); // call system to stop
  }

  int size;
  inFile >> size;

  cout << "Size: " << size << endl;

  string line;
  getline(inFile, line);
  while (getline(inFile, line)) {
    vector<float> nums;
    istringstream is(line);

    float num;
    while (is >> num)
      nums.push_back(num);

    Point point(nums);
    points.push_back(point);
  }

  inFile.close();

  ds.setPoints(points);

  /*cout << "vector points of size" << points.size() << " contains:";
  for (unsigned i = 0; i < points.size(); ++i) {
    vector<float> p = points[i].vals;
    for(unsigned j = 0; j < p.size(); ++j)
      cout << ' ' << p[j] << endl;
  }
  cout << endl;*/

  return ds;
}

void print_help() {
  cout << "Format: " << endl
       << "kmeans -c clusters -t threshold -i iterations -w workers -I "
          "path/to/input"
       << endl;
}