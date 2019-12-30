import sys

import kea
import utils


class TestCalloutHandle_new(utils.BaseTestCase):

    def test_noargs(self):
        with self.assertRaises(TypeError) as cm:
            kea.CalloutHandle()
        self.assertEqual(("function takes exactly 1 argument (0 given)",), cm.exception.args)

    def test_toomanyargs(self):
        with self.assertRaises(TypeError) as cm:
            kea.CalloutHandle(1, 2)
        self.assertEqual(("function takes exactly 1 argument (2 given)",), cm.exception.args)

    def test_badarg(self):
        with self.assertRaises(TypeError) as cm:
            kea.CalloutHandle(1)
        self.assertEqual(("manager must be instance of CalloutManager",), cm.exception.args)

    def test_ok(self):
        m = kea.CalloutManager()
        h = kea.CalloutHandle(m)
        self.assertEqual(2, m.use_count)
        h = None
        self.assertEqual(1, m.use_count)


class TestCalloutHandle_setArgument(utils.BaseTestCase):

    def test_lease4(self):
        m = kea.CalloutManager()
        h = kea.CalloutHandle(m)
        l = kea.Lease4()
        l.addr = '1.2.3.4'
        self.assertEqual(1, l.use_count)
        self.assertIsNone(h.setArgument('lease4', l))
        self.assertEqual(2, l.use_count)
        self.assertEqual('1.2.3.4', h.getArgument('lease4').addr)


class TestCalloutHandle_setContext(utils.BaseTestCase):

    def test_ok(self):
        m = kea.CalloutManager()
        h = kea.CalloutHandle(m)
        foo = utils.Logger()
        self.assertIsNone(h.setContext('foo', foo))
        self.assertEqual(3, sys.getrefcount(foo))


class TestCalloutHandle_getContext(utils.BaseTestCase):
    
    def test_ok(self):
        m = kea.CalloutManager()
        h = kea.CalloutHandle(m)
        foo = utils.Logger()
        h.setContext('foo', foo)
        bar = h.getContext('foo')
        self.assertIs(foo, bar)

    def test_notset(self):
        m = kea.CalloutManager()
        h = kea.CalloutHandle(m)
        with self.assertRaises(TypeError) as cm:
            h.getContext('foo')
        self.assertEqual(("unable to find callout context associated with the current library index (-1)",), cm.exception.args)


class TestCalloutHandle_deleteContext(utils.BaseTestCase):
    
    def test_ok(self):
        m = kea.CalloutManager()
        h = kea.CalloutHandle(m)
        foo = utils.Logger()
        h.setContext('foo', foo)
        self.assertEqual(3, sys.getrefcount(foo))
        self.assertIsNone(h.deleteContext('foo'))
        self.assertEqual(2, sys.getrefcount(foo))


class TestCalloutHandle_getStatus(utils.BaseTestCase):
    
    def test_ok(self):
        m = kea.CalloutManager()
        h = kea.CalloutHandle(m)
        self.assertEqual(kea.NEXT_STEP_CONTINUE, h.getStatus())


class TestCalloutHandle_setStatus(utils.BaseTestCase):
    
    def test_ok(self):
        m = kea.CalloutManager()
        h = kea.CalloutHandle(m)
        self.assertIsNone(h.setStatus(kea.NEXT_STEP_SKIP))
        self.assertEqual(kea.NEXT_STEP_SKIP, h.getStatus())
