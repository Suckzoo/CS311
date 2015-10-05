#!/usr/bin/python
import sys

def dec2bin(x):
    result = ''
    px = x
    while px!=0:
        result = str(px%2) + result
        px = px / 2
    return result

def raw2register(x):
    bin_x = dec2bin(x)
    bin_x = (5-len(bin_x))*'0' + bin_x
    return bin_x

def imm2bin(x):
    bin_x = ''
    if x<0:
        temp = dec2bin(-x-1)
        temp = (16-len(temp))*'0' + temp 
        for i in temp:
            if i == '0':
                bin_x = bin_x + '1'
            else:
                bin_x = bin_x + '0'
    else:
        bin_x = dec2bin(x)
        bin_x = (16-len(bin_x))*'0' + bin_x
    return bin_x

def addr2bin(x):
    bin_x = dec2bin(x)
    bin_x = (32-len(bin_x))*'0' + bin_x
    return bin_x

def int2bin(x):
    bin_x = ''
    if x<0:
        temp = dec2bin(-x-1)
        temp = (32-len(temp))*'0' + temp 
        for i in temp:
            if i == '0':
                bin_x = bin_x + '1'
            else:
                bin_x = bin_x + '0'
    else:
        bin_x = dec2bin(x)
        bin_x = (32-len(bin_x))*'0' + bin_x
    return bin_x

def abs_to_rel(abs_addr, current):
    # return 'word' not 'byte'
    return imm2bin((int(abs_addr) - (current+4))/4)

raw2shift = raw2register

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
    def print_size(self):
        return addr2bin(self.address - 0x10000000)
    def print_var(self):
        stream = ''
        for i in range(0x10000000, self.address, 4):
            stream = stream + int2bin(self.addr_dic[i])
        return stream
    

class program(object):
    R = ['addu', 'and', 'jr', 'nor', \
            'or', 'sltu', 'sll', 'srl', 'subu']
    I = ['addiu', 'beq', 'bne', 'lw', 'sltiu', \
            'sw', 'lui', 'ori', 'la']
    J = ['j', 'jal']
    def __init__(self):
        self.pc = 0x400000
        self.label_dic = {}
        self.addr_dic = {}
    def print_size(self):
        return addr2bin(self.pc-0x400000)
    def print_text(self):
        stream = ''
        for i in range(0x400000, self.pc, 4):
            stream = stream + self.addr_dic[i]
        return stream
    def set_label(self, label):
        self.label_dic[label] = self.pc
    def op_eval(self, line):
        line = line.split()
        op = line[0]
        if op in ['and', 'addu', 'nor', 'or', 'subu', 'sltu']:
            p1 = line[1][1:-1]
            p2 = line[2][1:-1]
            p3 = line[3][1:]
            eval('self.op_' + op + '(' + p1 + ', ' + p2 + ', ' + p3 + ')')
        elif op in ['srl', 'sll']:
            p1 = line[1][1:-1]
            p2 = line[2][1:-1]
            p3 = line[3]
            if 'x' in p3:
                p3 = str(int(p3, 16))
            eval('self.op_' + op + '(' + p1 + ', ' + p2 + ', ' + p3 + ')')
        elif op in ['jr']:
            p1 = line[1][1:]
            eval('self.op_' + op + '(' + p1 + ')')
        elif op in ['andi', 'addiu', 'ori', 'sltiu']:
            p1 = line[1][1:-1]
            p2 = line[2][1:-1]
            p3 = line[3]
            if 'x' in p3:
                p3 = str(int(p3, 16))
            eval('self.op_' + op + '(' + p1 + ', ' + p2 + ', ' + p3 + ')')
        elif op in ['beq', 'bne']:
            p1 = line[1][1:-1]
            p2 = line[2][1:-1]
            p3 = "'" + line[3] + "'"
            eval('self.op_' + op + '(' + p1 + ', ' + p2 + ', ' + p3 + ')')
        elif op in ['j', 'jal']:
            p1 = "'" + line[1] + "'"
            eval('self.op_' + op + '(' + p1 + ')')
        elif op in ['lw', 'sw']:
            p1 = line[1][1:-1]
            temp_stream = line[2].split('($')
            p2 = temp_stream[0]
            if 'x' in p2:
                p2 = str(int(p2, 16))
            p3 = temp_stream[1][:-1]
            eval('self.op_' + op + '(' + p1 + ', ' + p2 + ', ' + p3 + ')')
        elif op in ['lui']:
            p1 = line[1][1:-1]
            p2 = line[2]
            if 'x' in p2:
                p2 = str(int(p2, 16))
            eval('self.op_' + op + '(' + p1 + ', ' + p2 + ')')
        elif op in ['la']:
            p1 = line[1][1:-1]
            p2 = "'" + addr2bin(section_data.find_label(line[2])) + "'"
            eval('self.op_' + op + '(' + p1 + ', ' + p2 + ')')

    def op_and(self, rd, rs, rt):
        reg_rd = raw2register(rd)
        reg_rs = raw2register(rs)
        reg_rt = raw2register(rt)
        self.addr_dic[self.pc] = '000000' + reg_rs + reg_rt + reg_rd + \
                                 '00000' +'100100'
        self.pc += 4
    def op_addu(self, rd, rs, rt):
        reg_rd = raw2register(rd)
        reg_rs = raw2register(rs)
        reg_rt = raw2register(rt) 
        self.addr_dic[self.pc] = '000000' + reg_rs + reg_rt + reg_rd + \
                                 '00000' +'100001'
        self.pc += 4
    def op_nor(self, rd, rs, rt):
        reg_rd = raw2register(rd)
        reg_rs = raw2register(rs)
        reg_rt = raw2register(rt) 
        self.addr_dic[self.pc] = '000000' + reg_rs + reg_rt + reg_rd + \
                                 '00000' +'100111'
        self.pc += 4
    def op_or(self, rd, rs, rt):
        reg_rd = raw2register(rd)
        reg_rs = raw2register(rs)
        reg_rt = raw2register(rt) 
        self.addr_dic[self.pc] = '000000' + reg_rs + reg_rt + reg_rd + \
                                 '00000' +'100101'
        self.pc += 4
    def op_subu(self, rd, rs, rt):
        reg_rd = raw2register(rd)
        reg_rs = raw2register(rs)
        reg_rt = raw2register(rt) 
        self.addr_dic[self.pc] = '000000' + reg_rs + reg_rt + reg_rd + \
                                 '00000' +'100011'
        self.pc += 4
    def op_sltu(self, rd, rs, rt):
        reg_rd = raw2register(rd)
        reg_rs = raw2register(rs)
        reg_rt = raw2register(rt) 
        self.addr_dic[self.pc] = '000000' + reg_rs + reg_rt + reg_rd + \
                                 '00000' +'101011'
        self.pc += 4
    def op_sll(self, rd, rt, sa):
        reg_rs = '00000'
        reg_rd = raw2register(rd)
        reg_rt = raw2register(rt)
        bin_sa = raw2shift(sa)
        self.addr_dic[self.pc] = '000000' + reg_rs + reg_rt + reg_rd + \
                                 bin_sa +'000000'
        self.pc += 4
    def op_srl(self, rd, rt, sa):
        reg_rs = '00000'
        reg_rd = raw2register(rd)
        reg_rt = raw2register(rt)
        bin_sa = raw2shift(sa)
        self.addr_dic[self.pc] = '000000' + reg_rs + reg_rt + reg_rd + \
                                 bin_sa +'000010'
        self.pc += 4
    def op_jr(self, rs):
        reg_rs = raw2register(rs)
        reg_rd = '00000'
        reg_rt = '00000'
        self.addr_dic[self.pc] = '000000' + reg_rs + reg_rt + reg_rd + \
                                 '00000' +'001000'
        self.pc += 4
    def op_addiu(self, rt, rs, imm):
        reg_rs = raw2register(rs)
        reg_rt = raw2register(rt)
        bin_imm = imm2bin(imm)
        self.addr_dic[self.pc] = '001001' + reg_rs + reg_rt + bin_imm
        self.pc += 4
    def op_andi(self, rt, rs, imm):
        reg_rs = raw2register(rs)
        reg_rt = raw2register(rt)
        bin_imm = imm2bin(imm)
        self.addr_dic[self.pc] = '001100' + reg_rs + reg_rt + bin_imm
        self.pc += 4
    def op_ori(self, rt, rs, imm):
        reg_rs = raw2register(rs)
        reg_rt = raw2register(rt)
        bin_imm = imm2bin(imm)
        self.addr_dic[self.pc] = '001101' + reg_rs + reg_rt + bin_imm
        self.pc += 4
    def op_sltiu(self, rt, rs, imm):
        reg_rs = raw2register(rs)
        reg_rt = raw2register(rt)
        bin_imm = imm2bin(imm)
        self.addr_dic[self.pc] = '001011' + reg_rs + reg_rt + bin_imm
        self.pc += 4
    def op_sw(self, rt, imm, rs):
        reg_rt = raw2register(rt)
        reg_rs = raw2register(rs)
        bin_imm = imm2bin(imm)
        self.addr_dic[self.pc] = '101011' + reg_rs + reg_rt + bin_imm
        self.pc += 4
    def op_lw(self, rt, imm, rs):
        reg_rt = raw2register(rt)
        reg_rs = raw2register(rs)
        bin_imm = imm2bin(imm)
        self.addr_dic[self.pc] = '100011' + reg_rs + reg_rt + bin_imm
        self.pc += 4
    def op_lui(self, rt, imm):
        reg_rt = raw2register(rt)
        reg_rs = '00000'
        bin_imm = imm2bin(imm)
        self.addr_dic[self.pc] = '001111' + reg_rs + reg_rt + bin_imm
        self.pc += 4
    def op_la(self, rt, bin_addr):
        reg_rt = raw2register(rt)
        reg_rs = '00000'
        upper = bin_addr[0:16]
        lower = bin_addr[16:]
        # lui $at, upper
        self.addr_dic[self.pc] = '001111' + reg_rs + reg_rt + upper
        self.pc += 4
        # ori rt, at, disp
        if lower != '0'*16:
            self.addr_dic[self.pc] = '001101' + reg_rt + reg_rt + lower
            self.pc += 4
    def op_beq(self, rs, rt, label):
        reg_rs = raw2register(rs)
        reg_rt = raw2register(rt)
        self.addr_dic[self.pc] = 'beq' + reg_rs + reg_rt + label
        self.pc += 4
    def op_bne(self, rs, rt, label):
        reg_rs = raw2register(rs)
        reg_rt = raw2register(rt)
        self.addr_dic[self.pc] = 'bne' + reg_rs + reg_rt + label
        self.pc += 4
    def op_j(self, target):
        self.addr_dic[self.pc] = 'j' + target
        self.pc += 4
    def op_jal(self, target):
        self.addr_dic[self.pc] = 'jal' + target
        self.pc += 4
    def label_relocate(self):
        for i in range(0x400000, self.pc, 4):
            line = self.addr_dic[i]
            if 'bne' == line[0:3]:
                self.addr_dic[i] = '000101' + line[3:13] + \
                                   abs_to_rel(self.label_dic[line[13:]], i)
            elif 'beq' == line[0:3]:
                self.addr_dic[i] = '000100' + line[3:13] + \
                                   abs_to_rel(self.label_dic[line[13:]], i)
            elif 'jal' == line[0:3]:
                self.addr_dic[i] = '000011' + \
                                   addr2bin(self.label_dic[line[3:]])[4:-2]
            elif 'j' == line[0]:
                # j, jal only changes lower 28bytes!!
                # Assume : label_dic has byte address, without leading zeros.
                self.addr_dic[i] = '000010' + \
                                   addr2bin(self.label_dic[line[1:]])[4:-2]

section_data = static_var()
section_text = program()

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
    line = line.strip()
    if not line:
        return
    if line[-1] == ':':
        section_text.set_label(line[0:-1])
    else:
        section_text.op_eval(line)
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

if __name__=='__main__':
    if len(sys.argv) == 2:
        filename = sys.argv[1]
        if filename[-2:] != '.s':
            print "Usage: ./assembler.py <*.s>"
        else:
            f = open(filename, 'r')
            stream = ''
            for line in f:
                stream+=line
            f.close()
            main(stream)
            section_text.label_relocate()
            f = open('result.o', 'w')
            f.write(section_text.print_size())
            f.write(section_data.print_size())
            f.write(section_text.print_text())
            f.write(section_data.print_var())
            f.close()

    else:
        print "Usage: ./assembler.py <*.s>"


