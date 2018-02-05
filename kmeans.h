#ifndef KMEANS
#define KMEANS

#define DEFAULT_THRESH 0.0000001f

#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

/*
 * Classes
 */

class Point {
public:
  vector<float> vals;

  Point(vector<float> newVals) : vals(newVals){};
  int getDimensions();
};

class DataSet {
public:
  int numFeatures();
  int getDimensions();
  void setPoints(vector<Point>);
  vector<Point> getPoints();

private:
  vector<Point> points;
};

#endif