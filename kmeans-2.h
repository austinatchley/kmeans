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
  vector<double> vals;

  Point(vector<double> newVals) : vals(newVals) {}

  const int getDimensions();

  bool operator<(const Point &other) const;
  bool operator>(const Point &other) const;
  bool operator==(const Point &other) const;
  bool operator!=(const Point &other) const;
  Point &operator+(const Point &other);
  Point &operator/(int val);
};

class DataSet {
public:
  int getDimensions();
  void setPoints(vector<Point>);
  const vector<Point> getPoints();
  const int numPoints();

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

Point &Point::operator+(const Point &other) {
  for (int i = 0; i < vals.size(); ++i)
    this->vals[i] += other.vals[i];

  return *this;
}

Point &Point::operator/(int val) {
  const int len = vals.size();
  for (int i = 0; i < len; ++i)
    this->vals[i] = this->vals[i] / val;

  return *this;
}

namespace point {
typedef std::map<Point, int> pointMap;
}

#endif
