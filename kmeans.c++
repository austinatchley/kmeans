#include <assert.h>
#include <cfloat>
#include <cmath>
#include <ctime>
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
point::pointMap findNearestCentroids(DataSet dataSet, vector<Point> centroids);
int findNearestCentroid(Point point, vector<Point> centroids);

vector<Point> averageLabeledCentroids(DataSet dataSet, point::pointMap labels,
                                      vector<Point> centroids);

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

  threshold = DEFAULT_THRESH;
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

  clock_t start;
  double duration;

  start = clock();
  kmeans(dataSet, clusters);
  duration = (clock() - start) / (double)CLOCKS_PER_SEC;

  cout << "Duration: " << duration << endl;
}

void kmeans(DataSet dataSet, int k) {
  if (k > dataSet.numFeatures())
    return;

  iterations = 0;

  int numFeatures = dataSet.numFeatures();
  vector<Point> centroids =
      randomCentroids(numFeatures, k, dataSet.getDimensions());

  vector<Point> oldCentroids;
  bool done = false;
  point::pointMap labels;

  while (!done) {
    oldCentroids = centroids;
    iterations++;

    labels = findNearestCentroids(dataSet, centroids);

    centroids = averageLabeledCentroids(dataSet, labels, centroids);
    done = iterations > max_iterations || converged(centroids, oldCentroids);
  }
}

vector<Point> randomCentroids(int numFeatures, int k, int dimensions) {
  vector<Point> centroids;

  for (int i = 0; i < k; ++i) {
    vector<float> vals;
    for (int j = 0; j < dimensions; ++j)
      // Generate rand between 0 and 1
      vals.push_back(((float)rand()) / RAND_MAX);

    Point p(vals);
    centroids.push_back(p);
  }

  return centroids;
}

point::pointMap findNearestCentroids(DataSet dataSet, vector<Point> centroids) {
  point::pointMap map;
  vector<Point> points = dataSet.getPoints();

  for (int i = 0; i < points.size(); ++i) {
    Point point = points[i];
    int nearestCentroid =
        findNearestCentroid(point, centroids); // returns index of the centroid
    map.insert(pair<Point, int>(point, nearestCentroid));
  }

  return map;
}

int findNearestCentroid(Point point, vector<Point> centroids) {
  assert(centroids.size() > 0);

  double min_dist = FLT_MAX;
  int index = -1;
  double sum = 0.0;

  for (int i = 0; i < centroids.size(); ++i) {
    for (int j = 0; j < point.getDimensions(); ++j) {
      sum += pow(centroids[i].vals[j] - point.vals[j], 2.0);
    }
    double dist = sqrt(sum);
    if (dist < min_dist) {
      min_dist = dist;
      index = i;
    }
  }
  assert(index != -1);
  assert(min_dist != FLT_MAX);
  return index; // returns the index of the centroid in the centroids vector
}

vector<Point> averageLabeledCentroids(DataSet dataSet, point::pointMap labels,
                                      vector<Point> centroids) {
  vector<Point> updatedCentroids;
  updatedCentroids.reserve(centroids.size());

  cout << "Initial size of " << centroids.size() << endl;

  vector<float> sums[centroids.size()]; // indices correspond to centroids
  int numPointsPerCentroid[centroids.size()];

  for (vector<float> elem : sums)
    elem.reserve(dataSet.getDimensions());

  for (const auto &pair : labels) {
    Point point = pair.first;
    int centroidIndex = pair.second;

    numPointsPerCentroid[centroidIndex]++;

    vector<float> centroidSum = sums[centroidIndex];
    for (int i = 0; i < centroidSum.size(); ++i)
      centroidSum[i] += point.vals[i];
  }

  vector<float> nums;
  nums.reserve(dataSet.getDimensions());
  for (int i = 0; i < centroids.size(); ++i) {
    vector<float> sum = sums[i];

    for (int j = 0; j < sum.size(); ++j) {
      nums[i] = (sum[j] / numPointsPerCentroid[i]);
    }

    Point point(nums);
    updatedCentroids.push_back(point);
  }

  cout << "Final size of " << updatedCentroids.size() << endl << endl;

  return updatedCentroids;
}

bool converged(vector<Point> centroids, vector<Point> oldCentroids) {
  assert(centroids.size() == oldCentroids.size());
  assert(centroids[0].vals.size() == oldCentroids[0].vals.size());

  for (int i = 0; i < centroids.size(); ++i) {
    Point curPoint = centroids[i];
    Point oldPoint = oldCentroids[i];
    for (int j = 0; j < curPoint.vals.size(); ++j)
      // if any haven't converged, not done
      if (curPoint.vals[j] - oldPoint.vals[j] > threshold)
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

    int lineNumber;
    is >> lineNumber;

    assert(points.size() == lineNumber - 1);

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
