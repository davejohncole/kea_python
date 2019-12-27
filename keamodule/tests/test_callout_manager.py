import unittest

import kea


class TestCalloutManager(unittest.TestCase):

    def test_allocate(self):
        m = kea.CalloutManager()
        self.assertEqual(1, m.use_count)
