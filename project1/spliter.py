#!/usr/bin/python
f = open('result.o', 'r')
stream = ''
stream = f.readline()
for i in range(len(stream)/32):
    print stream[i*32:(i+1)*32]
f.close()
