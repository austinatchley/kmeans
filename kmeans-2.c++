#include <algorithm>
#include <assert.h>
#include <cfloat>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits.h>
#include <map>
#include <pthread.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <vector>

#include "kmeans-2.h"

using namespace std;

/*
 * Global Vars
 */

int clusters;
int max_iterations;
double threshold;
int workers;
string input;

vector<int> numPointsPerCentroid;
bool break_flag;
bool mutex;

pthread_mutex_t lock;
pthread_spinlock_t slock;
pthread_barrier_t barrier;

/*
 * Function Prototypes
 */

void *kmeans(void *arg);

void smart_lock();
void smart_unlock();
vector<Point> randomCentroids(int dimensions, int k, DataSet &dataSet);
point::pointMap findNearestCentroids(DataSet *dataSet,
                                     vector<Point> &centroids);
int findNearestCentroid(Point &point, vector<Point> &centroids);

vector<Point> averageLabeledCentroids(DataSet *dataSet, point::pointMap &labels,
                                      vector<Point> &centroids);

bool converged(vector<Point> &centroids, vector<Point> &oldCentroids);

void print_help();
DataSet *readFile(DataSet *ds, string filePath);
void printPointVector(const vector<Point> &points);
vector<DataSet> splitDataSet(DataSet &total, int workers);

typedef struct args {
  DataSet *dataSet;
  vector<Point> &centroids;
  int num_thread;

  args(DataSet *set, vector<Point> &cent, int num)
      : dataSet(set), centroids(cent), num_thread(num) {}

} args;

/*
 * Class Functions
 */

const int Point::getDimensions() { return this->vals.size(); }

void DataSet::setPoints(vector<Point> points) { this->points = points; }

const vector<Point> DataSet::getPoints() { return this->points; }

const int DataSet::numPoints() { return points.size(); }

int DataSet::getDimensions() { return this->points[0].getDimensions(); }

/*
 * Functions
 */

int main(int argc, char *argv[]) {

  threshold = DEFAULT_THRESH;
  max_iterations = INT_MAX;

  int opt, val;
  while ((opt = getopt(argc, argv, "hc:t:i:w:I:l")) != -1) {
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
      val = atoi(optarg);
      if (val != 0)
        max_iterations = val;
      break;

    case 'w':
      assert(optarg);
      workers = atoi(optarg);
      break;

    case 'I':
      assert(optarg);
      input = optarg;
      break;

    case 'l':
      mutex = false;
      break;
    }
  }

  DataSet *dataSet = new DataSet();
  readFile(dataSet, input);
  int numPoints = dataSet->numPoints();

  cout << numPoints << endl;

  vector<DataSet> dataSets = splitDataSet(*dataSet, workers);
  assert(dataSets.size() == workers);

  clock_t start;
  double duration;

  pthread_t workers_list[workers];

  pthread_barrier_init(&barrier, NULL, workers);
  pthread_mutex_init(&lock, NULL);
  pthread_spin_init(&slock, 0);

  vector<Point> centroids =
      randomCentroids(dataSet->getDimensions(), clusters, *dataSet);

  numPointsPerCentroid = vector<int>(centroids.size(), 0);
  break_flag = false;

  /*
  cpu_set_t cpuset;
  pthread_attr_t attr;
  pthread_attr_init(&attr);*/

  void **arg_void_list = (void **)malloc(workers * sizeof(void *));
  for (int i = 0; i < workers; ++i) {
    args *args_i = new args(&dataSets[i], centroids, i);
    arg_void_list[i] = static_cast<void *>(args_i);
  }

  start = clock();

  for (int i = 0; i < workers; ++i) {
    // CPU_ZERO(&cpuset);
    // CPU_SET(i % workers, &cpuset);

    // pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);

    int ret = pthread_create(&workers_list[i], NULL, &kmeans, arg_void_list[i]);

    if (ret)
      cerr << "Couldn't create thread" << i << endl;
  }

  for (int i = 0; i < workers; ++i)
    pthread_join(workers_list[i], NULL);

  duration = (clock() - start) / (double)CLOCKS_PER_SEC;

#ifdef DEBUG
  cout << numPoints << endl;
  cout << dataSet->numPoints() << endl;
#endif

  cout << duration << endl;

  printPointVector(dataSet->getPoints());
  printPointVector(centroids);

  delete dataSet;
  delete arg_void_list;
}

void *kmeans(void *arg) {
  struct args *args = static_cast<struct args *>(arg);
  DataSet *dataSet = args->dataSet;
  vector<Point> &centroids = args->centroids;
  int num_thread = args->num_thread;

  int iterations = 0;

  int dimensions = dataSet->getDimensions();

  /*
  cpu_set_t cpuset;

  CPU_ZERO(&cpuset);
  CPU_SET(num_thread % workers, &cpuset);

  int s = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
*/
  /*
  cpu_set_t cpuset;
  smart_lock();
  pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
  for (int j = 0; j < get_nprocs(); j++)
    if (CPU_ISSET(j, &cpuset))
      cout << "    CPU " << j << endl;
  smart_unlock();
  */

  vector<Point> oldCentroids;
  point::pointMap labels;

  do {
    oldCentroids = centroids;
    pthread_barrier_wait(&barrier);

#ifdef DEBUG
    cout << "Iteration " << iterations << endl;
#endif

    if (num_thread == 0) {
      fill(numPointsPerCentroid.begin(), numPointsPerCentroid.end(), 0);
    }
    pthread_barrier_wait(&barrier);

    labels = findNearestCentroids(dataSet, centroids);
    pthread_barrier_wait(&barrier);

    if (num_thread == 0) {
      vector<double> zero(dimensions, 0.0);
      fill(centroids.begin(), centroids.end(), Point(zero));
    }
    pthread_barrier_wait(&barrier);

    vector<Point> newCentroids =
        averageLabeledCentroids(dataSet, labels, centroids);

    smart_lock();
    for (int i = 0; i < centroids.size(); ++i)
      centroids[i] = centroids[i] + newCentroids[i];
    smart_unlock();
    pthread_barrier_wait(&barrier);

    iterations++;
    if (num_thread == 0)
      if (iterations >= max_iterations || converged(centroids, oldCentroids))
        break_flag = true;

    pthread_barrier_wait(&barrier);

    if (break_flag)
      break;

  } while (true);

  if (num_thread == 0)
    cout << iterations << endl;

  return nullptr;
}

vector<Point> randomCentroids(int dimensions, int k, DataSet &dataSet) {
  vector<Point> centroids;
  vector<Point> points = dataSet.getPoints();
  vector<int> indicesUsed;

  for (int i = 0; i < k; ++i) {
    int index;

    // Generate rand index that hasn't been used
    do {
      srand(time(NULL));
      index = ((int)rand()) % points.size();
    } while (find(indicesUsed.begin(), indicesUsed.end(), index) !=
             indicesUsed.end());

    indicesUsed.push_back(index);

    Point p(points[index].vals);
    centroids.push_back(p);
  }

#ifdef DEBUG
  cout << "Initialized centroids to " << endl;
  printPointVector(centroids);
#endif

  return centroids;
}

point::pointMap findNearestCentroids(DataSet *dataSet,
                                     vector<Point> &centroids) {
  point::pointMap map;
  vector<Point> points = dataSet->getPoints();

  for (int i = 0; i < points.size(); ++i) {
    Point point = points[i];
    int nearestCentroid =
        findNearestCentroid(point, centroids); // returns index of the centroid

    // cout << "Point " << i << " Nearest Cent: " << nearestCentroid << endl;
    pair<point::pointMap::iterator, bool> ret;
    ret = map.insert(make_pair(point, nearestCentroid));
    if (ret.second == false) {
      map[point] = nearestCentroid;
#ifdef DEBUG
      cout << i << " already exists with val of " << ret.first->second << endl;
      cout << "Now " << i << " is " << map.at(point) << endl;
#endif
    }

    // cout << map.at(point) << endl;
    assert(map.at(point) == nearestCentroid);
    assert(map.size() >= i);
  }

  return map;
}

int findNearestCentroid(Point &point, vector<Point> &centroids) {
  assert(centroids.size() > 0);

  double min_dist = 0.0;
  for (int i = 0; i < point.getDimensions(); ++i) {
    min_dist += pow(centroids[0].vals[i] - point.vals[i], 2.0);
  }
  min_dist = sqrt(min_dist);

  int index = 0;

  for (int i = 0; i < centroids.size(); ++i) {
    double sum = 0.0;

    for (int j = 0; j < point.getDimensions(); ++j) {
      assert(sum >= 0);
      double old_sum = sum;
      double square = pow(centroids[i].vals[j] - point.vals[j], 2.0);
      assert(!isnan(square));
      assert(abs(square) <= numeric_limits<double>::max() - abs(sum));

      assert(square >= 0);
      sum += square;
      assert(sum >= 0);
    }

    assert(sum >= 0);
    /*if (sum <= 0) {
      for (const auto &val : centroids) {
        cout << "Centroid:" << endl;
        for (const auto &num : val.vals)
          cout << num << endl;
      }
    }*/

    double dist = sqrt(sum);
    assert(!isnan(dist)); // make sure we don't have nan

    if (dist < min_dist) {
      min_dist = dist;
      index = i;
    }
    assert(index >= 0);
    assert(index < centroids.size());
  }

  smart_lock();
  numPointsPerCentroid[index]++;
  smart_unlock();

  return index; // returns the index of the centroid in the centroids vector
}

vector<Point> averageLabeledCentroids(DataSet *dataSet, point::pointMap &labels,
                                      vector<Point> &centroids) {
#ifdef DEBUG
  cout << "Initial size of centroids: " << centroids.size() << endl;
  cout << "Initial size of elem 0 of centroid: " << centroids[0].vals.size()
       << endl;
#endif

  vector<Point> updatedCentroids;

  // indices correspond to centroids
  vector<double> sum(dataSet->getDimensions(), 0.0);
  vector<vector<double>> sums(centroids.size(), sum);

#ifdef DEBUG
  cout << "Size of sums elem: " << (sizeof(sums[0]) / sizeof(sums[0][0]))
       << endl;
#endif

  for (const auto &pair : labels) {
    Point point = pair.first;
    int centroidIndex = pair.second;

    for (int i = 0; i < dataSet->getDimensions(); ++i) {
      assert(!isnan(sums[centroidIndex][i]));

      sums[centroidIndex][i] += point.vals[i];
      assert(!isnan(sums[centroidIndex][i]));
    }
  }

  for (int i = 0; i < centroids.size(); ++i) {
    vector<double> nums;

    for (int j = 0; j < dataSet->getDimensions(); ++j) {
      double finalNum = sums[i][j];
      if (numPointsPerCentroid[i] > 0)
        finalNum /= numPointsPerCentroid[i];

      if (isnan(finalNum)) {
        cout << "finalNum: " << finalNum << ", numPPC[" << i
             << "]: " << numPointsPerCentroid[i] << ", sum[" << j
             << "]: " << sums[i][j] << endl;
      }
      nums.push_back(finalNum);
    }

    Point point(nums);
    updatedCentroids.push_back(point);
  }

#ifdef DEBUG
  cout << "Final size of: " << updatedCentroids.size() << endl;
  cout << "Final size of elem 0: " << updatedCentroids[0].vals.size() << endl
       << endl;
  printPointVector(updatedCentroids);
#endif

  return updatedCentroids;
}

bool converged(vector<Point> &centroids, vector<Point> &oldCentroids) {
  assert(centroids.size() == oldCentroids.size());

  assert(centroids[0].vals.size() == oldCentroids[0].vals.size());

  for (int i = 0; i < centroids.size(); ++i) {
    Point curPoint = centroids[i];
    Point oldPoint = oldCentroids[i];
    for (int j = 0; j < curPoint.vals.size(); ++j)
      // if any haven't converged, not done
      if (fabs(curPoint.vals[j] - oldPoint.vals[j]) > threshold) {
#ifdef DEBUG
        cout << "\ncur at " << i << ": " << curPoint.vals[j]
             << "\nold: " << oldPoint.vals[j] << endl;
#endif
        return false;
      }
  }

#ifdef DEBUG
  cout << "Final transformation\n";
  printPointVector(oldCentroids);
  cout << "->\n";
  printPointVector(centroids);
#endif

  return true;
}

/*
 * Utility Functions
 */

DataSet *readFile(DataSet *ds, string filePath) {
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
  while (size--) {
    getline(inFile, line);
    vector<double> nums;
    istringstream is(line);

    int lineNumber;
    is >> lineNumber;

    assert(points.size() == lineNumber - 1);

    double num;
    while (is >> num)
      nums.push_back(num);

    Point point(nums);
    points.push_back(point);
  }

  inFile.close();

  ds->setPoints(points);

  return ds;
}

void print_help() {
  cout << "Format: " << endl
       << "kmeans -c clusters -t threshold -i iterations -w workers -I "
          "path/to/input"
       << endl;
}

void printPointVector(const vector<Point> &points) {
  for (const auto &point : points) {
    for (int i = 0; i < point.vals.size() - 1; ++i) {
      double val = point.vals[i];
      cout << val << ", ";
    }
    cout << point.vals[point.vals.size() - 1] << endl;
  }
}

vector<DataSet> splitDataSet(DataSet &total, int workers) {
  vector<DataSet> sets;
  vector<Point> points = total.getPoints();
  int size = points.size();
  for (int i = 0; i < workers; ++i) {
    DataSet set;
    vector<Point> setList;

    int begin = ((size * i) / workers);
    int end = ((size * (i + 1)) / workers);

    for (int i = begin; i < end; ++i)
      setList.push_back(points[i]);

#ifdef DEBUG
    cout << workers << " workers" << endl;
    cout << "Begin at " << ((size * i) / workers) << endl;
    cout << "End at " << ((size * (i + 1)) / workers) << endl;
#endif

    set.setPoints(setList);

    sets.push_back(set);
  }
  return sets;
}

void smart_lock() {
  if (mutex)
    pthread_mutex_lock(&lock);
  else
    pthread_spin_lock(&slock);
}

void smart_unlock() {
  if (mutex)
    pthread_mutex_unlock(&lock);
  else
    pthread_spin_unlock(&slock);
}
