import unittest

import kea
import utils


class TestCalloutManager(utils.BaseTestCase):

    def test_allocate_toomanyargs(self):
        with self.assertRaises(TypeError) as cm:
            kea.CalloutManager(1)
        self.assertEqual(('function takes at most 0 arguments (1 given)',), cm.exception.args)

    def test_allocate(self):
        m = kea.CalloutManager()
        self.assertEqual(1, m.use_count)
