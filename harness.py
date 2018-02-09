import csv
import subprocess
from io import StringIO

args = "./kmeans.out -c 4 -t 0.0000001 -i 20 -w 1 -I sample/sample.in".split()
output = subprocess.check_output(args)
result = output.decode('utf-8')
# print(result)

f = StringIO(result)
reader = csv.reader(f, delimiter=',')
data = []
for row in reader:
    data.append(', '.join(row))
    #print(', '.join(row))

print(data)
