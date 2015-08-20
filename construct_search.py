#!/usr/bin/env python
import sys
import time

t1 = t2 = lc = 0
d = dict()
with open(sys.argv[1],'r') as f:
  for line in f:
    start = time.time()
    d[line.rstrip()] = 'foo'
    t1 = t1 + (time.time() - start)
    lc += 1


miss = 0
with open(sys.argv[1],'r') as f:
  for line in f:
    key = line.rstrip()
    start = time.time()
    if key not in d and d[key] != 'foo':
      miss += 1
    t2 = t2 + (time.time() - start)


print "python\t%s\t%d\t%f\t%f\n" % (sys.argv[1],lc,t1,t2)
