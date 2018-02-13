#!/usr/bin/env python3

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
times_spin = []


def do_test(i, cores, spin):
    arg_list = args.split()
    arg_list[8] = str(cores)

    if spin:
        arg_list.append('-l')
        print(" ".join(str(val) for val in arg_list))

    output = subprocess.check_output(arg_list)
    result = output.decode('utf-8')

    f = StringIO(result)
    reader = csv.reader(f, delimiter=',')
    data = []
    for row in reader:
        data.append(' '.join(element.rstrip() for element in row))

    if i != -1:
        print('Points:\t', data[0])
        print('Iterations:\t', data[1])
        print('Duration:\t', data[2])

    return float(data[2])


def control_test():
    arg_list = args.split()
    arg_list[0] = "./kmeans.out"

    output = subprocess.check_output(arg_list)
    result = output.decode('utf-8')

    f = StringIO(result)
    reader = csv.reader(f, delimiter=',')
    data = []
    for row in reader:
        data.append(' '.join(element.rstrip() for element in row))

    return float(data[2])


control_test()
print("Control")
control = control_test()
for core in range(1, cores + 1):
    do_test(-1, core, False)

    both = 0.0
    both_spin = 0.0
    for i in range(TESTS):
        print("\nIteration ", i, "with ", core, "cores. Mutex")
        both += do_test(i, core, False)
    for i in range(TESTS):
        print("\nIteration ", i, "with ", core, "cores. Spinlock")
        both_spin += do_test(i, core, True)
    average = both / 2
    average_spin = both_spin / 2
    times.insert(core, control / average)
    times_spin.insert(core, control / average_spin)

print('\nSpeedups:')
for time in times:
    print(time)

color = '#1f10e0'
color_spin = '#ff0011'

plt.plot(
    list(map(int, range(1,
                        int(cores) + 1))),
    times,
    c=color,
    alpha=0.8,
    marker='o')
plt.plot(
    list(map(int, range(1,
                        int(cores) + 1))),
    times_spin,
    c=color_spin,
    alpha=0.8,
    marker='o')

plt.legend(['Mutex', 'Spinlock'])

plt.xlabel('Number of Cores')
plt.ylabel('Speedup')
#plt.show()
plt.savefig("graph" + version + ".pdf", bbox_inches='tight', format='pdf')
