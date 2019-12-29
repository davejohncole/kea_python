import unittest

import kea
import utils


class TestOption_new(utils.BaseTestCase):

    def test_ok(self):
        m = kea.Option(42)
        self.assertEqual(1, m.use_count)
