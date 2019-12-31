import os


class Capture:

    def __init__(self, fname=None):
        if fname is None:
            dirname = os.path.dirname(__file__)
            fname = os.path.join(dirname, 'packets.log')
        self.fp = open(fname, 'w')

    def unload(self):
        self.fp.close()
        return 0

    def pkt4_receive(self, handle):
        self.fp.write('>> query4\n')
        self.fp.write(handle.getArgument('query4').toText())
        self.fp.write('\n\n')
        return 0

    def pkt4_send(self, handle):
        self.fp.write('<< response4\n')
        self.fp.write(handle.getArgument('response4').toText())
        self.fp.write('\n\n')
        return 0
