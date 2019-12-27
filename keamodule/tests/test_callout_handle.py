import unittest

import kea


class TestCalloutHandle(unittest.TestCase):

    def test_allocate(self):
        m = kea.CalloutManager()
        h = kea.CalloutHandle(m)
        self.assertEqual(2, m.use_count)
        h = None
        self.assertEqual(1, m.use_count)
