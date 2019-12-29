import unittest

import kea
import utils


class TestLease4_new(utils.BaseTestCase):

    def test_ok(self):
        m = kea.Lease4()
        self.assertEqual(1, m.use_count)


class TestLease4_addr(utils.BaseTestCase):

    def test_getset(self):
        m = kea.Lease4()
        self.assertEqual('0.0.0.0', m.addr)
        m.addr = '192.168.0.1'
        self.assertEqual('192.168.0.1', m.addr)
