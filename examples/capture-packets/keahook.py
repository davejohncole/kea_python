import os
from kea import *


def load(handle):
    global fp
    dirname = os.path.dirname(__file__)
    fp = open(os.path.join(dirname, 'packets.log'), 'w')
    return 0

def unload():
    fp.close()
    return 0

def pkt4_receive(handle):
    fp.write('>> query4\n')
    fp.write(handle.getArgument('query4').toText())
    fp.write('\n\n')
    return 0

def pkt4_send(handle):
    fp.write('<< response4\n')
    fp.write(handle.getArgument('response4').toText())
    fp.write('\n\n')
    return 0
