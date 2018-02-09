#ifndef KMEANS
#define KMEANS

// uncomment this to enable debug output
//#define DEBUG

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
  float total;

  Point(vector<float> newVals) : vals(newVals) {
    for (int n : vals)
      total += n;
  };

  int getDimensions();

  bool operator<(const Point &other) const;
  bool operator=(const Point &other) const;
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

bool Point::operator<(const Point &other) const {
  return this->total < other.total;
}

bool Point::operator=(const Point &other) const {
  if (this->vals.size() != other.vals.size())
    return false;

  for (int i = 0; i < this->vals.size(); ++i) {
    if (this->vals[i] != other.vals[i])
      return false;
  }

  return true;
}

namespace point {
typedef std::map<Point, int> pointMap;
}

#endif
