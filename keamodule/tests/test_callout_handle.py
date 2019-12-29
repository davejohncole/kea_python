import sys

import kea
import utils


class TestCalloutHandle(utils.BaseTestCase):

    def test_allocate(self):
        m = kea.CalloutManager()
        h = kea.CalloutHandle(m)
        self.assertEqual(2, m.use_count)
        h = None
        self.assertEqual(1, m.use_count)

    def test_set_lease4(self):
        m = kea.CalloutManager()
        h = kea.CalloutHandle(m)
        l = kea.Lease4()
        l.addr = '1.2.3.4'
        self.assertEqual(1, l.use_count)
        self.assertIsNone(h.setArgument('lease4', l))
        self.assertEqual(2, l.use_count)
        self.assertEqual('1.2.3.4', h.getArgument('lease4').addr)
