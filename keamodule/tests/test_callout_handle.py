import sys

import kea
import utils


class TestCalloutHandle(utils.BaseTestCase):

    def test_allocate_noargs(self):
        with self.assertRaises(TypeError) as cm:
            kea.CalloutHandle()
        self.assertEqual(("Required argument 'manager' (pos 1) not found",), cm.exception.args)

    def test_allocate_toomanyargs(self):
        with self.assertRaises(TypeError) as cm:
            kea.CalloutHandle(1, 2)
        self.assertEqual(("function takes at most 1 argument (2 given)",), cm.exception.args)

    def test_allocate_badarg(self):
        with self.assertRaises(TypeError) as cm:
            kea.CalloutHandle(1)
        self.assertEqual(("manager must be instance of CalloutManager",), cm.exception.args)

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

    def test_set_context(self):
        m = kea.CalloutManager()
        h = kea.CalloutHandle(m)
        foo = utils.Logger()
        self.assertIsNone(h.setContext('foo', foo))
        self.assertEqual(3, sys.getrefcount(foo))
    
    def test_get_context(self):
        m = kea.CalloutManager()
        h = kea.CalloutHandle(m)
        foo = utils.Logger()
        h.setContext('foo', foo)
        bar = h.getContext('foo')
        self.assertIs(foo, bar)
    
    def test_delete_context(self):
        m = kea.CalloutManager()
        h = kea.CalloutHandle(m)
        foo = utils.Logger()
        h.setContext('foo', foo)
        self.assertEqual(3, sys.getrefcount(foo))
        h.deleteContext('foo')
        self.assertEqual(2, sys.getrefcount(foo))
