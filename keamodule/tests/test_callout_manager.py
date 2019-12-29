import unittest

import kea
import utils


class TestCalloutManager_new(utils.BaseTestCase):

    def test_toomanyargs(self):
        with self.assertRaises(TypeError) as cm:
            kea.CalloutManager(1)
        self.assertEqual(('function takes exactly 0 arguments (1 given)',), cm.exception.args)

    def test_ok(self):
        m = kea.CalloutManager()
        self.assertEqual(1, m.use_count)
