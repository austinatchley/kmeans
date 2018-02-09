import csv
import subprocess
import numpy as np
import matplotlib.pyplot as plt

from io import StringIO


args = "./kmeans.out -c 4 -t 0.0000001 -i 20 -w 1 -I sample/sample.in".split()
output = subprocess.check_output(args)
result = output.decode('utf-8')
# print(result)

f = StringIO(result)
reader = csv.reader(f, delimiter=',')
data = []
for row in reader:
    data.append(' '.join(element.rstrip() for element in row))
    print(' '.join(row))

print('\nIterations:\t', data[0])
print('Duration:\t', data[len(data) - 1])

x = []
y = []
for i in range(1,len(data)-1):
    vals = data[i].split("  ")
    x.append(vals[0].rstrip())
    # print("\'" + vals[1].rstrip() + "\'")
    y.append(vals[1].rstrip())

floatX = [float(x_val) for x_val in x]
floatY = [float(y_val) for y_val in y]

colors = np.random.rand(len(x))
plt.scatter(floatX, floatY, s=10, c=colors, alpha=0.5)
plt.show()
