import sys
import csv
import subprocess
import numpy as np
import matplotlib.pyplot as plt

from io import StringIO


args = "./kmeans.out -c 4 -t 0.0000001 -i 20 -w 1 -I" + sys.argv[1]
output = subprocess.check_output(args.split())
result = output.decode('utf-8')
# print(result)

f = StringIO(result)
reader = csv.reader(f, delimiter=',')
data = []
for row in reader:
    data.append(' '.join(element.rstrip() for element in row))
    # print(' '.join(row))

points = int(data[0])
print('\nPoints:\t', points)
print('Iterations:\t', data[len(data) - 2])
print('Duration:\t', data[len(data) - 1])

points_x = []
points_y = []
points_color = '#2122dd';
for i in range(1,points+1):
    vals = data[i].split("  ")
    points_x.append(vals[0].rstrip())
    points_y.append(vals[1].rstrip())

cent_x = []
cent_y = []
cent_color = '#ff1010'
for i in range(points+1, len(data)-2):
    vals = data[i].split("  ")
    cent_x.append(vals[0].rstrip())
    cent_y.append(vals[1].rstrip())

points_x_float = [float(x_val) for x_val in points_x]
points_y_float = [float(y_val) for y_val in points_y]
cent_x_float = [float(x_val) for x_val in cent_x]
cent_y_float = [float(y_val) for y_val in cent_y]

print("Points: " + str(len(points_x_float)) + ", Cent: " + str(len(cent_x_float)))

colors = np.random.rand(len(points_x_float))

plt.scatter(points_x_float, points_y_float, s=10, c=points_color, alpha=0.5)
plt.scatter(cent_x_float, cent_y_float, s=10, c=cent_color, alpha=1)
plt.show()
