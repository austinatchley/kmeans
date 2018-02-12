#!/usr/bin/env python

import sys
import csv
import subprocess
import numpy as np
import matplotlib.pyplot as plt

from io import StringIO

TESTS = 2

if len(sys.argv) < 6:
    print(
        "./harness [version] -c [clusters] -t [threshold] -i [iterations] -w [max cores to test] -I [path/to/file] "
    )
    sys.exit(0)

if sys.argv[1] != '1':
    version = '-' + sys.argv[1]
else:
    version = ''

c = sys.argv[2]
t = sys.argv[3]
i = sys.argv[4]
w = sys.argv[5]
I = sys.argv[6]

args = "./kmeans" + version  + ".out -c " + c +    \
    "  -t " + t +                 \
    " -i " + i  +                 \
    " -w " + w  +                 \
    " -I " + I

cores = int(w)
times = []


def do_test(i, cores):
    arg_list = args.split()
    arg_list[8] = str(cores)

    output = subprocess.check_output(arg_list)
    result = output.decode('utf-8')

    f = StringIO(result)
    reader = csv.reader(f, delimiter=',')
    data = []
    for row in reader:
        data.append(' '.join(element.rstrip() for element in row))

    points = int(data[0])
    print('Points:\t', points)
    print('Iterations:\t', data[1])
    print('Duration:\t', data[2])
    times.insert(i, float(data[2]))

for core in range(cores):
    for i in range(TESTS):
        print("\nIteration ", i, "with ", core, "cores.")
        do_test(i, core)

print('\nTimes:')
for time in times:
    print(time)

color = '#1f10e0'

#plt.scatter(points_x_float, points_y_float, s=10, c=points_color, alpha=0.5)
#plt.scatter(cent_x_float, cent_y_float, s=10, c=cent_color, alpha=1)
#plt.show()
