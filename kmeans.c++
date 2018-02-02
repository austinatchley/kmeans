#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "kmeans.h"

using namespace std;

int clusters;
int max_iterations;
float threshold;
int workers;
string input;

int iterations;

int main(int argc, char *argv[]) {

  threshold = 0.0000001f;
  max_iterations = 0;

  int opt;
  while ((opt = getopt(argc, argv, "hc:t:i:w:I:")) != -1) {
    switch(opt) {
      case 'h':
        print_help();
        exit(0);
        break;
      case 'c':
        clusters = atoi(optarg);
        break;
      case 't':
        threshold = atof(optarg);
        break;
      case 'i':
        max_iterations = atoi(optarg);
        break;
      case 'w':
        workers = atoi(optarg);
        break;
      case 'I':
        input = optarg;
        break;
    }
  }
  cout << clusters << endl << threshold << endl << iterations << endl << workers << endl << input << endl;
}

void print_help() {
  cout << "Format: " << endl
    << "kmeans -c clusters -t threshold -i iterations -w workers -I path/to/input"
    << endl;
}