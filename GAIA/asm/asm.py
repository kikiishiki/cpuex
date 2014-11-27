#!/usr/bin/env python2.7

import sys
import os.path
import re
import struct
import argparse


srcs = {'_main': {0: 'jl r31, main'}}
filename = ''
library = 'libmincaml_.s'
pos = 0

def error(msg):
    print >> sys.stderr, '{}:{}: error:'.format(filename, pos), msg
    print >> sys.stderr, '    ' + srcs[filename][pos]
    sys.exit(1)


# ----------------------------------------------------------------------
#       utility functions (mainly parsing)
# ----------------------------------------------------------------------

iregs = {}
for i in range(0, 32):
    iregs['r' + str(i)] = i

fregs = {}
for i in range(0, 32):
    fregs['f' + str(i)] = i

regs = {}
regs.update(iregs)
regs.update(fregs)

def is_reg(operand):
    return operand in regs

def parse_imm(operand):
    try:
        imm = int(operand, 0)
        return True, imm
    except ValueError:
        return False, 0

def parse_float(operand):
    try:
        f = float(operand)
        return True, f
    except ValueError:
        return False, 0.0

def parse_addr(operand):
    m = re.match(r'(\$\w+)\s*([+-])\s*(\w+)$', operand)
    if m:
        base = m.group(1)
        offset = m.group(2) + m.group(3)
        if is_reg(base) and parse_imm(offset)[0]:
            return True, base, offset
    m = re.match(r'\$\w+$', operand)
    if m and is_reg(m.group()):
        return True, m.group(), '0'
    m = re.match(r'[+-]?\w+$', operand)
    if m and parse_imm(m.group())[0]:
        return True, '$0', m.group()
    return False, '$0', '0'

def try_parse_addr(operand):
    success, base, offset = parse_addr(operand)
    if success:
        return '{}, {}'.format(base, offset)
    return operand

def try_parse_memaccess(operand):
    m = re.match(r'\[(.*)\]$', operand)
    if m:
        return True, try_parse_addr(m.group(1))
    return False, ''

def parse_mnemonic(mnemonic):
    s = re.split(r'^([^+-\.]*)', mnemonic)
    return s[1], s[2]

def check_operands_n(operands, n, m=-1):
    if len(operands) < n:
        error('too few operands')
    if len(operands) > max(n, m):
        error('too many operands')

def check_imm_range(imm, lo=-0x8000, hi=0x7fff):
    if not lo <= imm <= hi:
        error('immediate value exceeds valid range')

def ireg_num(reg):
    if reg not in iregs:
        error('invalid syntax in ireg_num')
    return iregs[reg]

def freg_num(reg):
    if reg not in fregs:
        error('invalid syntax in freg_num')
    return fregs[reg]

def mkop_alu(op, a0, a1, a2, imm, tag):
    a0 = ireg_num(a0)
    a1 = ireg_num(a1)
    a2 = ireg_num(a2)
    success, imm = parse_imm(imm)
    if not success:
        error('invalid syntax')
    check_imm_range(imm)
    head = chr((op << 3) + (a0 >> 2)) + chr(((a0 & 0x03) << 6) + (a1 << 1) + (a2 >> 4))
    tail = chr(((a2 & 0x0f) << 4) + (imm >> 4)) + chr(((imm & 0x0f) << 4) + tag)
    return head + tail

def mkop_fpu(op, a0, a1, a2, a3, signmode, tag):
    a0 = freg_num(a0)
    a1 = freg_num(a1)
    a2 = freg_num(a2)
    a3 = freg_num(a3)
    head = chr((op << 3) + (a0 >> 2)) + chr(((a0 & 0x03) << 6) + (a1 << 1) + (a2 >> 4))
    tail = chr(((a2 & 0x0f) << 4) + (a3 >> 4)) + chr(((a3 & 0x01) << 7) + (signmode << 5) + tag)
    return head + tail

def mkop_mem(op, a0, a1, pred, disp):
    if op in [2,3,8,9,16,17,18,19,24,25]:
        a0 = ireg_num(a0)
    else:
        a0 = freg_num(a0)
    if op in [2,3,8,9,10,11,16,17,18,19,24,25]:
        a1 = ireg_num(a1)
    else:
        a1 = freg_num(a1)
    success, disp = parse_imm(disp)
    if not success:
        error('invalid syntax')
    check_imm_range(disp)
    head = chr((op << 3) + (a0 >> 2)) + chr(((a0 & 0x03) << 6) + (a1 << 1) + pred)
    tail = chr((disp >> 8) & 0xff) + chr(disp & 0xff)
    return head + tail

def parse(line):
    line = line.strip()
    m = re.match(r'\S+', line)
    mnemonic = m.group()
    t = line[m.end():].strip()
    operands = re.split(r',\s*', t)
    if operands == ['']:
        return mnemonic, []
    return mnemonic,  operands


# ----------------------------------------------------------------------
#       mnemonic definitions
# ----------------------------------------------------------------------

#alu

def on_add(op, operands):
    check_operands_n(operands, 4)
    return mkop_alu(op, operands[0], operands[1], operands[2], operands[3], 0)

def on_sub(op, operands):
    check_operands_n(operands, 4)
    return mkop_alu(op, operands[0], operands[1], operands[2], operands[3], 1)

def on_shl(op, operands):
    check_operands_n(operands, 4)
    return mkop_alu(op, operands[0], operands[1], operands[2], operands[3], 2)

def on_shr(op, operands):
    check_operands_n(operands, 4)
    return mkop_alu(op, operands[0], operands[1], operands[2], operands[3], 3)

def on_sar(op, operands):
    check_operands_n(operands, 4)
    return mkop_alu(op, operands[0], operands[1], operands[2], operands[3], 4)

def on_and(op, operands):
    check_operands_n(operands, 3)
    return mkop_alu(op, operands[0], operands[1], operands[2], '0', 5)

def on_or(op, operands):
    check_operands_n(operands, 2)
    return mkop_alu(op, operands[0], operands[1], 'r0', '0', 6)

def on_not(op, operands):
    check_operands_n(operands, 3)
    return mkop_alu(op, operands[0], operands[1], operands[2], '0', 7)

def on_xor(op, operands):
    check_operands_n(operands, 3)
    return mkop_alu(op, operands[0], operands[1], operands[2], '0', 8)

def on_cat(op, operands):
    check_operands_n(operands, 3)
    return mkop_alu(op, operands[0], operands[1], operands[2], '0', 9)

def on_cmpne(op, operands):
    check_operands_n(operands, 3)
    return mkop_alu(op, operands[0], operands[1], operands[2], '0', 12)

def on_cmpeq(op, operands):
    check_operands_n(operands, 3)
    return mkop_alu(op, operands[0], operands[1], operands[2], '0', 13)

def on_cmplt(op, operands):
    check_operands_n(operands, 3)
    return mkop_alu(op, operands[0], operands[1], operands[2], '0', 14)

def on_cmple(op, operands):
    check_operands_n(operands, 3)
    return mkop_alu(op, operands[0], operands[1], operands[2], '0', 15)

def on_ldl(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(2, operands[0], operands[1], 0, operands[2])

def on_ldh(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(3, operands[0], operands[1], 0, operands[2])

#fpu

def on_fadd(signmode, operands):
    check_operands_n(operands, 3)
    return mkop_fpu(4, operands[0], operands[1], operands[2], 'f0', signmode, 0)

def on_fsub(signmode, operands):
    check_operands_n(operands, 3)
    return mkop_fpu(4, operands[0], operands[1], operands[2], 'f0', signmode, 1)

def on_fmul(signmode, operands):
    check_operands_n(operands, 3)
    return mkop_fpu(4, operands[0], operands[1], operands[2], 'f0', signmode, 2)

def on_fdiv(signmode, operands):
    check_operands_n(operands, 3)
    return mkop_fpu(4, operands[0], operands[1], operands[2], 'f0', signmode, 3)

def on_finv(signmode, operands):
    check_operands_n(operands, 2)
    return mkop_fpu(4, operands[0], operands[1], 'f0', 'f0', signmode, 4)

def on_fsqrt(signmode, operands):
    check_operands_n(operands, 2)
    return mkop_fpu(4, operands[0], operands[1], 'f0', 'f0', signmode, 5)

def on_ftoi(signmode, operands):
    check_operands_n(operands, 2)
    return mkop_fpu(4, operands[0], operands[1], 'f0', 'f0', signmode, 6)

def on_itof(signmode, operands):
    check_operands_n(operands, 2)
    return mkop_fpu(4, operands[0], operands[1], 'f0', 'f0', signmode, 7)

def on_floor(signmode, operands):
    check_operands_n(operands, 2)
    return mkop_fpu(4, operands[0], operands[1], 'f0', 'f0', signmode, 8)

def on_ffma(signmode, operands):
    check_operands_n(operands, 4)
    return mkop_fpu(4, operands[0], operands[1], operands[2], operands[3], signmode, 9)

def on_fcat(signmode, operands):
    check_operands_n(operands, 3)
    return mkop_fpu(4, operands[0], operands[1], operands[2], 'f0', signmode, 10)

def on_fand(signmode, operands):
    check_operands_n(operands, 3)
    return mkop_fpu(4, operands[0], operands[1], operands[2], 'f0', signmode, 11)

def on_for(signmode, operands):
    check_operands_n(operands, 3)
    return mkop_fpu(4, operands[0], operands[1], operands[2], 'f0', signmode, 12)

def on_fxor(signmode, operands):
    check_operands_n(operands, 3)
    return mkop_fpu(4, operands[0], operands[1], operands[2], 'f0', signmode, 13)

def on_fnot(signmode, operands):
    check_operands_n(operands, 2)
    return mkop_fpu(4, operands[0], operands[1], 'f0', 'f0', signmode, 14)

def on_fcmpne(signmode, operands):
    check_operands_n(operands, 3)
    return mkop_fpu(4, operands[0], operands[1], operands[2], 'f0', signmode, 28)

def on_fcmpeq(signmode, operands):
    check_operands_n(operands, 3)
    return mkop_fpu(4, operands[0], operands[1], operands[2], 'f0', signmode, 29)

def on_fcmplt(signmode, operands):
    check_operands_n(operands, 3)
    return mkop_fpu(4, operands[0], operands[1], operands[2], 'f0', signmode, 30)

def on_fcmple(signmode, operands):
    check_operands_n(operands, 3)
    return mkop_fpu(4, operands[0], operands[1], operands[2], 'f0', signmode, 31)

def on_fldl(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(6, operands[0], operands[1], 0, operands[2])

def on_fldh(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(7, operands[0], operands[1], 0, operands[2])

#mem,branch,jump

def on_ld(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(8, operands[0], operands[1], 0, operands[2])

def on_st(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(9, operands[0], operands[1], 0, operands[2])

def on_fld(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(10, operands[0], operands[1], 0, operands[2])

def on_fst(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(11, operands[0], operands[1], 0, operands[2])

def on_bne(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(16, operands[0], operands[1], pred, operands[2])

def on_beq(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(17, operands[0], operands[1], pred, operands[2])

def on_blt(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(18, operands[0], operands[1], pred, operands[2])

def on_ble(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(19, operands[0], operands[1], pred, operands[2])

def on_fbne(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(20, operands[0], operands[1], pred, operands[2])

def on_fbeq(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(21, operands[0], operands[1], pred, operands[2])

def on_fblt(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(22, operands[0], operands[1], pred, operands[2])

def on_fble(pred, operands):
    check_operands_n(operands, 3)
    return mkop_mem(23, operands[0], operands[1], pred, operands[2])

def on_jl(pred, operands):
    check_operands_n(operands, 2)
    return mkop_mem(24, operands[0], 'r0', pred, operands[1])

def on_jr(pred, operands):
    check_operands_n(operands, 1)
    return mkop_mem(25, operands[0], 'r0', pred, '0')


def on_dot_int(operands):
    check_operands_n(operands, 1)
    success, imm = parse_imm(operands[0])
    if not success:
        error('invalid syntax')
    check_imm_range(imm, -0x80000000, 0xffffffff)
    head = chr((imm >> 24) & 0xff) + chr((imm >> 16) & 0xff)
    tail = chr((imm >>  8) & 0xff) + chr( imm        & 0xff)
    return head + tail

def on_dot_float(operands):
    check_operands_n(operands, 1)
    success, f = parse_float(operands[0])
    if not success:
        error('invalid syntax')
    try:
        s = struct.pack('>f', f)
        imm = struct.unpack('>i', s)[0]
        return on_dot_int([str(imm)])
    except OverflowError:
        error('float too large')

alu_table = {
    'add':    on_add,
    'sub':    on_sub,
    'shl':    on_shl,
    'shr':    on_shr,
    'sar':    on_sar,
    'and':    on_and,
    'or':     on_or,
    'not':    on_not,
    'xor':    on_xor,
    'cat':    on_cat,
    'cmpne':  on_cmpne,
    'cmpeq':  on_cmpeq,
    'cmplt':  on_cmplt,
    'cmple':  on_cmple,
    'ldl':    on_ldl,
    'ldh':    on_ldh
}

fpu_table = {
    'fadd':   on_fadd,
    'fsub':   on_fsub,
    'fmul':   on_fmul,
    'fdiv':   on_fdiv,
    'finv':   on_finv,
    'fsqrt':  on_fsqrt,
    'ftoi':   on_ftoi,
    'itof':   on_itof,
    'floor':  on_floor,
    'ffma':   on_ffma,
    'fcat':   on_fcat,
    'fand':   on_fand,
    'for':    on_for,
    'fxor':   on_fxor,
    'fnot':   on_fnot,
    'fcmpne': on_fcmpne,
    'fcmpeq': on_fcmpeq,
    'fcmplt': on_fcmplt,
    'fcmple': on_fcmple,
    'fldl':   on_fldl,
    'fldh':   on_fldh
}

mem_table = {
    'ld':     on_ld,
    'st':     on_st,
    'fld':    on_fld,
    'fst':    on_fst,
    'bne':    on_bne,
    'beq':    on_beq,
    'blt':    on_blt,
    'ble':    on_ble,
    'fbne':   on_fbne,
    'fbeq':   on_fbeq,
    'fblt':   on_fblt,
    'fble':   on_fble,
    'jl':     on_jl,
    'jr':     on_jr
}

all_table = {}
all_table.update(alu_table)
all_table.update(fpu_table)
all_table.update(mem_table)

alu_suffix = {
    '':          0,
    '.s':        0,
    '.u':        1
}

fpu_suffix = {
    '':          0,
    '.neg':      1,
    '.abs':      2,
    '.abs.neg':  3
}

mem_suffix = {
    '':          0,
    '-':         0,
    '+':         1
}

all_suffix = {}
all_suffix.update(alu_suffix)
all_suffix.update(fpu_suffix)
all_suffix.update(mem_suffix)

# ----------------------------------------------------------------------
#       macro definitions
# ----------------------------------------------------------------------

def expand_nop(operands):
    check_operands_n(operands, 0)
    return ['add r0, r0, r0, 0']

def expand_mov(operands):
    check_operands_n(operands, 2)
    success, ret = try_parse_memaccess(operands[1])
    if success:
        return ['ld {}, {}'.format(operands[0], ret)]
    success, ret = try_parse_memaccess(operands[0])
    if success:
        return ['st {}, {}'.format(operands[1], ret)]
    ret = try_parse_addr(operands[1])
    return ['add {}, r0, {}'.format(operands[0], ret)]

def expand_add(operands):
    check_operands_n(operands, 3, 4)
    if len(operands) == 4:
        return ['add {}'.format(', '.join(operands))]
    if is_reg(operands[2]):
        return ['add {}, {}, {}, 0'.format(operands[0], operands[1], operands[2])]
    if parse_imm(operands[2])[0]:
        return ['add {}, {}, r0, {}'.format(operands[0], operands[1], operands[2])]
    error('invalid syntax')

# def expand_sub(operands):
#     check_operands_n(operands, 4)
#     success, imm = parse_imm(operands[2])
#     if success:
#         return ['add {}, {}, r0, {}'.format(operands[0], operands[1], str(-imm))]
#     return ['sub {}'.format(', '.join(operands))]

# def expand_neg(operands):
#     check_operands_n(operands, 2)
#     return ['sub {}, r0, {}'.format(operands[0], operands[1])]

# def expand_shift(operands):
#     check_operands_n(operands, 3, 4)
#     if len(operands) == 4:
#         return ['shift {}'.format(', '.join(operands))]
#     success, base, offset = parse_addr(operands[2])
#     if not success:
#         error('invalid syntax')
#     return ['shift {}, {}, {}, {}'.format(operands[0], operands[1], base, offset)]

# def expand_shl(operands):
#     check_operands_n(operands, 3)
#     return ['shift {}, {}, $0, {}'.format(operands[0], operands[1], operands[2])]

# def expand_shr(operands):
#     check_operands_n(operands, 3)
#     success, imm = parse_imm(operands[2])
#     if success:
#         return ['shift {}, {}, $0, {}'.format(operands[0], operands[1], str(-imm))]
#     error('invalid syntax')

# def expand_fsub(operands):
#     check_operands_n(operands, 3)
#     return [
#         'fneg {}, {}'.format(operands[2], operands[2]),
#         'fadd {}, {}, {}'.format(operands[0], operands[1], operands[2]),
#         'fneg {}, {}'.format(operands[2], operands[2])
#     ]

# def expand_br(operands):
#     check_operands_n(operands, 1)
#     ret = try_parse_addr(operands[0])
#     return ['beq $0, $0, {}'.format(ret)]

# def expand_beq(operands):
#     check_operands_n(operands, 3, 4)
#     if len(operands) == 4:
#         return ['beq {}'.format(', '.join(operands))]
#     ret = try_parse_addr(operands[2])
#     return ['beq {}, {}, {}'.format(operands[0], operands[1], ret)]

# def expand_ble(operands):
#     check_operands_n(operands, 3, 4)
#     if len(operands) == 4:
#         return ['ble {}'.format(', '.join(operands))]
#     ret = try_parse_addr(operands[2])
#     return ['ble {}, {}, {}'.format(operands[0], operands[1], ret)]

# def expand_bge(operands):
#     check_operands_n(operands, 3)
#     ret = try_parse_addr(operands[2])
#     return ['ble {}, {}, {}'.format(operands[1], operands[0], ret)]

def expand_push(operands):
    check_operands_n(operands, 1)
    return [
        'add $sp, $sp, $0, -1',
        'store {}, $sp, 0'.format(operands[0])
    ]

def expand_pop(operands):
    check_operands_n(operands, 1)
    return [
        'load {}, $sp, 0'.format(operands[0]),
        'add $sp, $sp, $0, 1'
    ]

def expand_call(operands):
    check_operands_n(operands, 1)
    ret = try_parse_addr(operands[0])
    return [
        'store $bp, $sp, -1',
        'store $ip, $sp, -2',
        'add $sp, $sp, $0, -2',
        'add $bp, $sp, $0, 0',
        'beq $0, $0, {}'.format(ret),
        'add $sp, $bp, $0, 2',
        'load $bp, $sp, -1'
    ]

def expand_ret(operands):
    check_operands_n(operands, 0)
    return [
        'load $12, $bp, 0',
        'beq $0, $0, $12, 4'
    ]

def expand_halt(operands):
    check_operands_n(operands, 0)
    return ['beq r0, r0, 0']

def expand_dot_data(operands):
    check_operands_n(operands, 0)
    return []

def expand_dot_text(operands):
    check_operands_n(operands, 0)
    return []

def expand_dot_int(operands):
    check_operands_n(operands, 1, 2)
    if len(operands) == 1:
        return ['.int {}'.format(operands[0])]
    success, imm = parse_imm(operands[1])
    if not success:
        error('invalid syntax')
    check_imm_range(imm, 0, 1000)
    return ['.int {}'.format(operands[0])] * imm

def expand_dot_float(operands):
    check_operands_n(operands, 1, 2)
    if len(operands) == 1:
        return ['.float {}'.format(operands[0])]
    success, imm = parse_imm(operands[1])
    if not success:
        error('invalid syntax')
    check_imm_range(imm, 0, 1000)
    return ['.float {}'.format(operands[0])] * imm

macro_table = {
    'nop':      expand_nop,
    'mov':      expand_mov,
    'add':      expand_add,
    # 'sub':      expand_sub,
    # 'neg':      expand_neg,
    # 'shift':    expand_shift,
    # 'shl':      expand_shl,
    # 'shr':      expand_shr,
    # 'fsub':     expand_fsub,
    # 'br':       expand_br,
    # 'beq':      expand_beq,
    # 'ble':      expand_ble,
    # 'bge':      expand_bge,
    'push':     expand_push,
    'pop':      expand_pop,
    'call':     expand_call,
    'ret':      expand_ret,
    'halt':     expand_halt,
    '.data':    expand_dot_data,
    '.text':    expand_dot_text,
    '.int':     expand_dot_int,
    '.float':   expand_dot_float
}


# ----------------------------------------------------------------------
#       label resolution
# ----------------------------------------------------------------------

labels = {}
rev_labels = {}

def add_label(label, i):
    dic = labels.get(label, {})
    if filename in dic and dic[filename][0] >= 0:
        error('duplicate declaration of label \'{}\''.format(label))
    val = dic.get(filename, [-1, False, False])
    dic[filename] = [i, val[1], False]
    labels[label] = dic
    rev_labels[i] = rev_labels.get(i, []) + [label]

def add_global(label):
    dic = labels.get(label, {})
    val = dic.get(filename, [-1, False, False])
    dic[filename] = [val[0], True, False]
    labels[label] = dic

def check_global(label):
    if labels[label][filename][0] < 0:
        error('label \'{}\' is not declared'.format(label))

def subst(label, cur):
    if is_reg(label) or parse_imm(label)[0] or parse_float(label)[0]:
        return [label]
    if label not in labels:
        error('label \'{}\' is not declared'.format(label))
    if filename in labels[label]:
        labels[label][filename][2] = True
        return [str(labels[label][filename][0] - cur)]
    else:
        decl = ''
        for key in labels[label]:
            if labels[label][key][1]:
                if decl:
                    error('label \'{}\' is declared in multiple files ({}, {})'.format(label, decl, key))
                decl = key
        if not decl:
            error('label \'{}\' is not declared'.format(label))
        labels[label][decl][2] = True
        return [str(labels[label][decl][0] - cur)]

def warn_unused_label(label):
    if not labels[label][filename][2] and not (filename == library and labels[label][filename][1]):
        print >> sys.stderr, '{}:{}: warning: unused label \'{}\''.format(filename, pos, label)

def show_label(i):
    if i in rev_labels:
        return '# {}'.format(', '.join(rev_labels[i]))
    return ''


# ----------------------------------------------------------------------
#       main process
# ----------------------------------------------------------------------

# parse command line arguments
argparser = argparse.ArgumentParser(usage='%(prog)s [options] file...')
argparser.add_argument('inputs', nargs='*', help='input files', metavar='files...')
argparser.add_argument('-l', help='set library file to <file> (default: {})'.format(library), metavar='<file>')
argparser.add_argument('-o', help='set output file to <file>', metavar='<file>')
argparser.add_argument('-s', action='store_const', const=True, help='output primitive assembly')
argparser.add_argument('-k', action='store_const', const=True, help='output as array of std_logic_vector format')
argparser.add_argument('-a', action='store_const', const=True, help='output as rs232c send test format')
args = argparser.parse_args()
if args.inputs == []:
    prog = re.sub(r'.*[/\\]', '', sys.argv[0])
    print >> sys.stderr, 'usage: {} [options] file...'.format(prog)
    print >> sys.stderr, ''
    print >> sys.stderr, '  -l\tset library file to <file> (default: {})'.format(library)
    print >> sys.stderr, '  -o\tset output file to <file>'
    print >> sys.stderr, '  -s\toutput primitive assembly'
    print >> sys.stderr, ''
    print >> sys.stderr, '{}: error: no input files'.format(prog)
    sys.exit(1)
if args.l:
    library = args.l
if os.path.isfile(library) and library not in args.inputs:
    args.inputs.append(library)
library = re.sub(r'.*[/\\]', '', library)

# 0. preprocess
lines0 = [('jl r31, main', '_main', 0)]                     
for filename in args.inputs:
    with open(filename, 'r') as f:
        filename = re.sub(r'.*[/\\]', '', filename)
        srcs[filename] = {}
        for pos, line in enumerate(f):
            line = line.strip()
            srcs[filename][pos + 1] = line
            line = re.sub(r'[;#].*', '', line).strip()
            if line:
                lines0.append((line, filename, pos + 1))
lines0.extend([('halt', '', 0)] * 3)

# 1. macro expansion
lines1 = []
for line, filename, pos in lines0:
    mnemonic, operands = parse(line)
    if mnemonic in macro_table:
        lines = macro_table[mnemonic](operands)
        lines1.extend(map(lambda x: (x, filename, pos), lines))
    else:
        lines1.append((line, filename, pos))

# 2. label resolution (by 2-pass algorithm)
i = 0
lines2 = []
lines3 = []
for line, filename, pos in lines1:
    mnemonic, operands = parse(line)
    if mnemonic[-1] == ':':
        if len(operands) > 0:
            error('invalid syntax')
        add_label(line[:-1], i)
    elif mnemonic == '.global':
        check_operands_n(operands, 1)
        add_global(operands[0])
    else:
        lines2.append((line, filename, pos))
        i += 1
for i, (line, filename, pos) in enumerate(lines2):
    mnemonic, operands = parse(line)
    simple_mnemonic, suffix = parse_mnemonic(mnemonic)
    if simple_mnemonic not in all_table:
        error('unknown mnemonic \'{}\''.format(simple_mnemonic))
    if suffix not in all_suffix:
        error('unknown suffix \'{}\''.format(suffix))
    check_operands_n(operands, 1, 4)
    operands[-1:] = subst(operands[-1], i)
    lines3.append(('{} {}'.format(mnemonic, ', '.join(operands)), filename, pos))
for line, filename, pos in lines1:
    mnemonic, operands = parse(line)
    if mnemonic[-1] == ':':
        warn_unused_label(line[:-1])
    if mnemonic == '.global':
        check_global(operands[0])

# 3. assemble
if not args.o:
    m = re.match(r'(.*)\.', args.inputs[0])
    args.o = '{}.out'.format(m.group(1) if m else args.inputs[0])
with open(args.o, 'w') as f:
    for i, (line, filename, pos) in enumerate(lines3):
        mnemonic, operands = parse(line)
        mnemonic, suffix = parse_mnemonic(mnemonic)
        if mnemonic in alu_table:
            if suffix in alu_suffix:
                opcode = alu_suffix.get(suffix)
                byterepr = alu_table[mnemonic](opcode, operands)
            else:
                error('invalid syntax')
        elif mnemonic in fpu_table:
            if suffix in fpu_suffix:
                sign_mode = fpu_suffix.get(suffix)
                byterepr = fpu_table[mnemonic](sign_mode, operands)
            else:
                error('invalid syntax')
        elif mnemonic in mem_table:
            if suffix in mem_suffix:
                pred = mem_suffix.get(suffix)
                byterepr = mem_table[mnemonic](pred, operands)
            else:
                error('invalid syntax')
        if args.k:
            f.write("{} => x\"{}\",\n".format(i, ''.join('{:02x}'.format(ord(x)) for x in byterepr)))
        elif args.a:
            fmt = """
            wait for BR; RS_RX <= '0';
            wait for BR; RS_RX <= '{}';
            wait for BR; RS_RX <= '{}';
            wait for BR; RS_RX <= '{}';
            wait for BR; RS_RX <= '{}';
            wait for BR; RS_RX <= '{}';
            wait for BR; RS_RX <= '{}';
            wait for BR; RS_RX <= '{}';
            wait for BR; RS_RX <= '{}';
            wait for BR; RS_RX <= '1';

            wait for (2 * BR);

            """
            for b in byterepr:
                a = ord(b)
                ps = ['1' if a & (1 << j) else '0' for j in range(8)]
                f.write(fmt.format(*ps))
        else:
            f.write(byterepr)
    if args.k:
        f.write("others => (others => '0')\n")
if args.s:
    with open(args.o + '.s', 'w') as f:
        for i, (line, filename, pos) in enumerate(lines3):
            mnemonic, operands = parse(line)
            f.write('{:7} {:19} {}'.format(mnemonic, ', '.join(operands), show_label(i)).strip() + '\n')
