#!/usr/bin/env python3

import os
import sys

datadir = sys.argv[1]

data_points = []
headers = []

for logfile in os.listdir(datadir):
    path = os.path.join(datadir, logfile)

    with open(path, "r") as log:
        timelog = []

        for line in log:
            if not timelog:
                timelog += [('started', int(line))]
            else:
                timelog += [(line.split()[0], int(line.split()[1]))]

        point = [x[1] for x in timelog]
        data_points += [point]

        if not headers:
            headers = [x[0] for x in timelog]

data_points.sort(key=lambda x: x[0])

T0 = data_points[0][0]

for d in data_points:
    d[0] = d[0] - T0
    d.append(sum(d[1:6]))

#print", ".join(['"timing_data" using %d t "%s"' % (i+1,x) for x, i in enumerate(headers) ])

print("\t".join(["#"] + headers))
for i, d in enumerate(data_points):
    print(" ".join([str(i)] + [str(x) for x in d]))