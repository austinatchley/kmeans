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


/*
* Classes
*/

class Point {
public:
  float x;
  float y;
};

class DataSet {
public:
  int numFeatures();
private:
  std::vector<Point> points;
};

#endif