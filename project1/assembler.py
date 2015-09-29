#!/usr/bin/python
import sys

class static_var(object):
    def __init__(self):
        self.address = 0x10000000
        self.label_dic = {} #label to addr
        self.addr_dic = {} #addr to val
    # def insert(self, val):
    #     self.addr_dic[self.address] = val
    #     self.address += 4
    def insert(self, *args):
        if len(args) == 2:
            self.label_dic[args[0]] = self.address
            self.addr_dic[self.address] = args[1]
        elif len(args) == 1:
            self.addr_dic[self.address] = args[0]
        else:
            return
        self.address += 4
    def find_label(self, label):
        return self.label_dic[label]

section_data = static_var()

def data(line):
    global section_data
    line = line.strip()
    line = line.split()
    if len(line) == 3:
        if 'x' in line[2]:
            line[2] = int(line[2], 16)
        else:
            line[2] = int(line[2])
        line[0] = line[0][0:-1]
        section_data.insert(line[0], line[2])
    else:
        if 'x' in line[1]:
            line[1] = int(line[1], 16)
        else:
            line[1] = int(line[1])
        section_data.insert(line[1])
    return

def text(line):
    return 

def main(stream):
    stream = stream.split('\n')
    mode = 0
    for line in stream:
        line = line.strip()
        if line == '.data':
            mode = 1
        elif line == '.text':
            mode = 2
        else:
            if mode == 1:
                data(line)
            else:
                text(line)
    #testing section
    global section_data
    print section_data.addr_dic
    return

if __name__=='__main__':
    if len(sys.argv) == 2:
        filename = sys.argv[1]
        if filename[-2:] != '.s':
            print "Usage: ./assembler.py <*.s>"
        else:
            f = open(filename)
            stream = ''
            for line in f:
                stream+=line
            main(stream)
    else:
        print "Usage: ./assembler.py <*.s>"
