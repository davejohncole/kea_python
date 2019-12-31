import unittest

import kea
import utils


class TestSrvConfig_new(utils.BaseTestCase):

    def test_ok(self):
        s = kea.SrvConfig()
        self.assertEqual(3, s.use_count)

    def test_default_current(self):
        s = kea.SrvConfig()
        self.assertEqual(3, s.use_count)
        t = kea.SrvConfig('current')
        self.assertEqual(4, s.use_count)

    def test_staging(self):
        s = kea.SrvConfig('staging')
        self.assertEqual(2, s.use_count)
        t = kea.SrvConfig('staging')
        self.assertEqual(3, s.use_count)
        u = kea.SrvConfig()
        self.assertEqual(3, u.use_count)


class TestSrvConfig_toElement(utils.BaseTestCase):

    def test_ok(self):
        s = kea.SrvConfig('staging')
        c = s.toElement()
        self.assertIsInstance(c, dict)
