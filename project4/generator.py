#!/usr/bin/env python

import sys
import random

length = 100

if len(sys.argv) > 1:
    length = int(sys.argv[1])

f = open('trace', 'w')
prev = []
for i in range(length):
    mode = random.randint(0, 1)
    if mode:
        f.write('W ')
    else:
        f.write('R ')

    p = random.randint(0, 1)
    if p and len(prev) > 1:
        paddr = random.randint(0, len(prev) - 1)
        addr = prev[paddr]
    else:
        addr = random.randint(1, 1 << 32)
        prev.append(addr)

    f.write('%#010x' % addr + '\n')

f.close()

