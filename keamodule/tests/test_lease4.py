import unittest

import kea
import utils


class TestLease4(utils.BaseTestCase):

    def test_allocate(self):
        m = kea.Lease4()
        self.assertEqual(1, m.use_count)

    def test_addr(self):
        m = kea.Lease4()
        self.assertEqual('0.0.0.0', m.addr)
        m.addr = '192.168.0.1'
        self.assertEqual('192.168.0.1', m.addr)
