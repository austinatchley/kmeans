#include <algorithm>
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

vector<Point> randomCentroids(int numFeatures, int k, DataSet dataSet);
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
  vector<Point> centroids = randomCentroids(numFeatures, k, dataSet);

  vector<Point> oldCentroids;
  bool done = false;
  point::pointMap labels;

  while (!done) {
    oldCentroids = centroids;
    cout << iterations++ << endl;

    labels = findNearestCentroids(dataSet, centroids);

    centroids = averageLabeledCentroids(dataSet, labels, centroids);
    done = iterations > max_iterations || converged(centroids, oldCentroids);
  }
  for (const auto &point : centroids) {
    cout << "[";
    for (float val : point.vals) {
      cout << val << ",";
    }
    cout << "]" << endl;
  }
}

vector<Point> randomCentroids(int numFeatures, int k, DataSet dataSet) {
  vector<Point> centroids;
  vector<Point> points = dataSet.getPoints();
  vector<int> indicesUsed;

  int dimensions = dataSet.getDimensions();

  for (int i = 0; i < k; ++i) {
    // Generate rand index
    int index;

    do {
      index = (((int)rand()) % points.size());
    } while (find(indicesUsed.begin(), indicesUsed.end(), index) !=
             indicesUsed.end());

    indicesUsed.push_back(index);

    Point p(points[index]);
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
    if (index == -1)
      cout << dist << endl;
  }
  assert(index != -1);
  assert(min_dist != FLT_MAX);
  return index; // returns the index of the centroid in the centroids vector
}

vector<Point> averageLabeledCentroids(DataSet dataSet, point::pointMap labels,
                                      vector<Point> centroids) {
  vector<Point> updatedCentroids;
  updatedCentroids.reserve(centroids.size());

#ifdef DEBUG
  cout << "Initial size of: " << centroids.size() << endl;
  cout << "Initial size of elem 0: " << centroids[0].vals.size() << endl;
#endif

  float sums[centroids.size()]
            [dataSet.getDimensions()]; // indices correspond to centroids
  int numPointsPerCentroid[centroids.size()];

#ifdef DEBUG
  cout << "Size of sums elem: " << (sizeof(sums[0]) / sizeof(sums[0][0]))
       << endl;
#endif

  for (const auto &pair : labels) {
    Point point = pair.first;
    int centroidIndex = pair.second;

    numPointsPerCentroid[centroidIndex]++;

    float *centroidSum = sums[centroidIndex];
    for (int i = 0; i < dataSet.getDimensions(); ++i)
      centroidSum[i] += point.vals[i];
  }

  for (int i = 0; i < centroids.size(); ++i) {
    vector<float> nums;

    float *sum = sums[i];

#ifdef DEBUG
    cout << "Values of nums:" << endl;
#endif

    for (int j = 0; j < dataSet.getDimensions(); ++j) {
      nums.push_back(sum[j] / numPointsPerCentroid[i]);

#ifdef DEBUG
      cout << nums[i] << endl;
#endif
    }

#ifdef DEBUG
    cout << "Nums size: " << nums.size() << endl;
#endif

    Point point(nums);

#ifdef DEBUG
    cout << "Points size: " << point.vals.size() << endl;
#endif

    updatedCentroids.push_back(point);
  }

#ifdef DEBUG
  cout << "Final size of: " << updatedCentroids.size() << endl;
  cout << "Final size of elem 0: " << updatedCentroids[0].vals.size() << endl
       << endl;
#endif

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

#ifdef DEBUG
  cout << "Size: " << size << endl;
#endif

  string line;
  getline(inFile, line);
  while (--size) {
    getline(inFile, line);
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

  cout << "test";

  // inFile.close();

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
