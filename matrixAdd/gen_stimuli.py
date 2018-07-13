#!/usr/bin/env python

import sys
import random

def write_header(f, name, type, height, width):
    f.write('RT_LOCAL_DATA %s %s[%d*%d];\n' % (type, name, height, width))

def write_arr(f, name, arr, type, height, width):
    f.write('RT_L2_DATA %s %s[%d*%d] = {\n' % (type, name, height, width))
    for i in range(0, height):
        for j in range(0, width):
            v = arr[i * height + j]
            f.write('%d, ' % (v))
        f.write('\n')
    f.write('};\n\n')
    return

################################################################################

def gen_stimuli(name, type, min, max, size):
    f = open(name, 'w')
    
    NR_ADD = 1
    HEIGHT = size
    WIDTH  = size
    MAX_SIZE = 16*16
    
    m_a   = []
    m_b   = []
    m_exp = []
    
    for i in range(0,HEIGHT):
        for j in range(0,WIDTH):
            a = random.randint(min, max)
            b = random.randint(min, max)
    
            r = (a + b)
    
            m_a.append(a)
            m_b.append(b)
            m_exp.append(r)
    
    f.write('typedef %s mattype;\n' % type)
    f.write('int matrixsizes = 16;\n')
    f.write('#define NR_ADD %d\n\n' % NR_ADD)

    write_header(f, 'm_a',   type, HEIGHT, WIDTH)
    write_header(f, 'm_b',   type, HEIGHT, WIDTH)
    write_header(f, 'm_c',   type, 1, MAX_SIZE)

    write_arr(f, 'm_a_l2',   m_a,   type, HEIGHT, WIDTH)
    write_arr(f, 'm_b_l2',   m_b,   type, HEIGHT, WIDTH)
    write_arr(f, 'm_exp_l2', m_exp, type, HEIGHT, WIDTH)
    
    f.write('#define WIDTH %d\n' % HEIGHT)
    f.write('#define HEIGHT %d\n'% WIDTH)

gen_stimuli('matrixAdd32.h', "int32_t", -2**30, 2**30-1,16)
gen_stimuli('matrixAdd16.h', "int16_t", -2**14, 2**14-1,16)
gen_stimuli('matrixAdd8.h',  "int8_t",  -2**6, 2**6-1,16)
