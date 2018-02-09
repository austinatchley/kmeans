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

  int dimensions = dataSet.getDimensions();
  vector<Point> centroids = randomCentroids(dimensions, k, dataSet);

  vector<Point> oldCentroids;
  bool done = false;
  point::pointMap labels;

  while (!done) {
    oldCentroids = centroids;

#ifdef DEBUG
    cout << "Iteration" << iterations << endl;
#endif
    iterations++;

    labels = findNearestCentroids(dataSet, centroids);

    centroids = averageLabeledCentroids(dataSet, labels, centroids);
    done = iterations > max_iterations || converged(centroids, oldCentroids);
  }

  for (const auto &point : centroids) {
    cout << "[";
    for (int i = 0; i < point.vals.size() - 1; ++i) {
      float val = point.vals[i];
      cout << val << ",";
    }
    cout << point.vals[point.vals.size() - 1];
    cout << "]" << endl;
  }
  cout << endl;
}

vector<Point> randomCentroids(int dimensions, int k, DataSet dataSet) {
  vector<Point> centroids;
  vector<Point> points = dataSet.getPoints();
  vector<int> indicesUsed;

  for (int i = 0; i < k; ++i) {
    int index;

    // Generate rand index
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
    cout << "Nearest Cent: " << nearestCentroid << endl;
    map.insert(pair<Point, int>(point, nearestCentroid));
  }

#ifdef DEBUG
  for (const auto &pair : map) {
    cout << endl << "Point: [";
    for (float val : pair.first.vals)
      cout << val << ",";
    cout << "]" << endl;

    cout << "Index: " << pair.second << endl;
  }
#endif

  return map;
}

int findNearestCentroid(Point point, vector<Point> centroids) {
  assert(centroids.size() > 0);

  double min_dist = 0;
  for (int i = 0; i < point.getDimensions(); ++i) {
    min_dist += pow(centroids[0].vals[i] - point.vals[i], 2.0);
  }
  min_dist = sqrt(min_dist);

  int index = 0;
  double sum;

  for (int i = 0; i < centroids.size(); ++i) {
    sum = 0.0;

    for (int j = 0; j < point.getDimensions(); ++j) {
      sum += pow(centroids[i].vals[j] - point.vals[j], 2.0);
    }

    assert(sum >= 0);
    if (sum <= 0)
      for (const auto &val : centroids)
        for (const auto &num : val.vals)
          cout << num << endl;

    double dist = sqrt(sum);
    assert(dist - dist == 0); // make sure we don't have nan

    if (dist < min_dist) {
      min_dist = dist;
      index = i;
    }
    assert(index >= 0);
    assert(index < centroids.size());
  }
  return index; // returns the index of the centroid in the centroids vector
}

vector<Point> averageLabeledCentroids(DataSet dataSet, point::pointMap labels,
                                      vector<Point> centroids) {
#ifdef DEBUG
  cout << "Initial size of: " << centroids.size() << endl;
  cout << "Initial size of elem 0: " << centroids[0].vals.size() << endl;
#endif

  vector<Point> updatedCentroids;

  // indices correspond to centroids
  float sums[centroids.size()][dataSet.getDimensions()];
  int numPointsPerCentroid[centroids.size()];

  for (int i = 0; i < centroids.size() * dataSet.getDimensions(); ++i)
    *((int *)sums + i) = 0;

  for (int i = 0; i < centroids.size(); ++i)
    numPointsPerCentroid[i] = 0;

#ifdef DEBUG
  cout << "Size of sums elem: " << (sizeof(sums[0]) / sizeof(sums[0][0]))
       << endl;
#endif

  for (const auto &pair : labels) {
    Point point = pair.first;
    int centroidIndex = pair.second;

#ifdef DEBUG
    cout << "Point: [";
    for (float val : point.vals)
      cout << val << ",";
    cout << "]" << endl;
    cout << "Centroid index: " << centroidIndex << endl;
#endif

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

    cout << "Centroid " << i << ": " << numPointsPerCentroid[i] << endl;

    for (int j = 0; j < dataSet.getDimensions(); ++j) {
      float finalNum = sum[j];
      if (numPointsPerCentroid[i] > 0)
        finalNum /= numPointsPerCentroid[i];

      nums.push_back(finalNum);
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

  cout << endl;

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
