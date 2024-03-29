#ifndef KMEANS
#define KMEANS

// uncomment this to enable debug output
// #define DEBUG

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

  const int getDimensions();

  bool operator<(const Point &other) const;
  bool operator>(const Point &other) const;
  bool operator==(const Point &other) const;
  bool operator!=(const Point &other) const;
};

class DataSet {
public:
  int getDimensions();
  void setPoints(vector<Point>);
  const vector<Point> getPoints();

private:
  vector<Point> points;
};

bool Point::operator<(const Point &other) const {
  return this->vals < other.vals;
}

bool Point::operator>(const Point &other) const {
  return this->vals > other.vals;
}

bool Point::operator==(const Point &other) const {
  return this->vals == other.vals;
}

bool Point::operator!=(const Point &other) const {
  return this->vals != other.vals;
}

namespace point {
typedef std::map<Point, int> pointMap;
}

#endif
