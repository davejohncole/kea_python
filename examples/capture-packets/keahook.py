from kea import *
from capture import Capture


def load(handle):
    global log
    log = Capture()
    return 0

def unload():
    log.unload()
    return 0

def pkt4_receive(handle):
    log.pkt4_receive(handle)
    return 0

def pkt4_send(handle):
    log.pkt4_send(handle)
    return 0
