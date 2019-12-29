import sys
import unittest

import kea


class Logger:

    def error(self, msg):
        sys.stderr.write(msg + '\n')


class BaseTestCase(unittest.TestCase):

    def setUp(self):
        kea.logger = Logger()
