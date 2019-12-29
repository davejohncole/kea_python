import unittest

import kea
import utils


class TestCalloutManager(utils.BaseTestCase):

    def test_allocate(self):
        m = kea.CalloutManager()
        self.assertEqual(1, m.use_count)
