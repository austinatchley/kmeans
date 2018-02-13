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
  Point operator+(const Point &other) const;
  Point operator/(int val) const;
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

Point Point::operator+(const Point &other) const {
  vector<double> v;

  for (int i = 0; i < vals.size(); ++i)
    v.push_back(other.vals[i] + vals[i]);

  return Point(v);
}

Point Point::operator/(int val) const {
  vector<double> v;

  for (int i = 0; i < vals.size(); ++i)
    v.push_back(vals[i] / val);

  Point p = Point(v);
  return p;
}

namespace point {
typedef std::map<Point, int> pointMap;
}

#endif
