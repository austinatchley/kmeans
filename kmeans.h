#ifndef KMEANS
#define KMEANS

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <iterator>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

/*
* Classes
*/

class Point {
public:
  vector<float> vals;

  Point(vector<float> newVals): vals(newVals) {};
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