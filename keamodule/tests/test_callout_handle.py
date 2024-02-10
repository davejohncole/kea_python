import sys

import kea
import utils


class TestCalloutHandle_new(utils.BaseTestCase):

    def test_badarg_count(self):
        self.assert_constructor_one_arg_no_keywords(kea.CalloutHandle)

    def test_badarg_type(self):
        with self.assertRaises(TypeError) as cm:
            kea.CalloutHandle(1)
        self.assertEqual(("argument 1 must be kea.CalloutManager, not int",), cm.exception.args)

    def test_ok(self):
        m = kea.CalloutManager()
        h = kea.CalloutHandle(m)
        self.assertEqual(2, m.use_count)
        del h
        self.assertEqual(1, m.use_count)


class TestCalloutHandle_getArgument(utils.BaseTestCase):

    def test_badarg_count(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        self.assert_method_one_arg_no_keywords(h.getArgument)

    def test_badarg_type(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        with self.assertRaises(TypeError) as cm:
            h.getArgument(3)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)

    def test_badarg_value(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        with self.assertRaises(ValueError) as cm:
            h.getArgument('foo')
        self.assertEqual(("unknown argument",), cm.exception.args)

    def test_ok(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        with self.assertRaises(TypeError) as cm:
            h.getArgument('query4')
        self.assertEqual(("unable to find argument with name query4",), cm.exception.args)


class TestCalloutHandle_setArgument(utils.BaseTestCase):

    def test_badarg_count(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        self.assert_method_two_args_no_keywords(h.setArgument)

    def test_badarg_type(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        with self.assertRaises(TypeError) as cm:
            h.setArgument(3, 4)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)

    def test_badarg_value(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        with self.assertRaises(ValueError) as cm:
            h.setArgument('foo', 3)
        self.assertEqual(("unknown argument",), cm.exception.args)

    def test_lease4_ok(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        x = kea.Lease4()
        x.addr = '1.2.3.4'
        self.assertEqual(1, x.use_count)
        self.assertIsNone(h.setArgument('lease4', x))
        self.assertEqual(2, x.use_count)
        self.assertEqual('1.2.3.4', h.getArgument('lease4').addr)

    def test_lease4_bad_type(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        with self.assertRaises(TypeError) as cm:
            h.setArgument('lease4', 3)
        self.assertEqual(("expected Lease4 object",), cm.exception.args)

    def test_query4_ok(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        x = kea.Pkt4(kea.DHCPACK, 42)
        self.assertEqual(1, x.use_count)
        self.assertIsNone(h.setArgument('query4', x))
        self.assertEqual(2, x.use_count)
        self.assertEqual(kea.DHCPACK, h.getArgument('query4').getType())

    def test_query4_bad_type(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        with self.assertRaises(TypeError) as cm:
            h.setArgument('query4', 3)
        self.assertEqual(("expected Pkt4 object",), cm.exception.args)

    def test_response4_ok(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        x = kea.Pkt4(kea.DHCPACK, 42)
        self.assertEqual(1, x.use_count)
        self.assertIsNone(h.setArgument('response4', x))
        self.assertEqual(2, x.use_count)
        self.assertEqual(kea.DHCPACK, h.getArgument('response4').getType())

    def test_response4_bad_type(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        with self.assertRaises(TypeError) as cm:
            h.setArgument('response4', 3)
        self.assertEqual(("expected Pkt4 object",), cm.exception.args)

    def test_command_ok(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        self.assertIsNone(h.setArgument('command', {'foo': 42}))
        self.assertEqual({'foo': 42}, h.getArgument('command'))

    def test_command_bad_type(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        with self.assertRaises(TypeError) as cm:
            h.setArgument('command', self)
        self.assertEqual(("unhandled type TestCalloutHandle_setArgument",), cm.exception.args)

    def test_response_ok(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        self.assertIsNone(h.setArgument('response', [1, 'two', None]))
        self.assertEqual([1, 'two', None], h.getArgument('response'))

    def test_response_bad_type(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        with self.assertRaises(TypeError) as cm:
            h.setArgument('response', self)
        self.assertEqual(("unhandled type TestCalloutHandle_setArgument",), cm.exception.args)


class TestCalloutHandle_setContext(utils.BaseTestCase):

    def test_badarg_count(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        self.assert_method_two_args_no_keywords(h.setContext)

    def test_badarg_type(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        with self.assertRaises(TypeError) as cm:
            h.setContext(1, 2)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)

    def test_ok(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        foo = utils.Logger()
        self.assertIsNone(h.setContext('foo', foo))
        self.assertEqual(3, sys.getrefcount(foo))


class TestCalloutHandle_getContext(utils.BaseTestCase):

    def test_badarg_count(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        self.assert_method_one_arg_no_keywords(h.getContext)

    def test_badarg_type(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        with self.assertRaises(TypeError) as cm:
            h.getContext(1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)

    def test_ok(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        foo = utils.Logger()
        h.setContext('foo', foo)
        bar = h.getContext('foo')
        self.assertIs(foo, bar)

    def test_notset(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        with self.assertRaises(TypeError) as cm:
            h.getContext('foo')
        self.assertEqual(("unable to find callout context associated with the current"
                          " library index (-1)",), cm.exception.args)


class TestCalloutHandle_deleteContext(utils.BaseTestCase):

    def test_badarg_count(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        self.assert_method_one_arg_no_keywords(h.deleteContext)

    def test_badarg_type(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        with self.assertRaises(TypeError) as cm:
            h.deleteContext(1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)

    def test_ok(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        foo = utils.Logger()
        h.setContext('foo', foo)
        self.assertEqual(3, sys.getrefcount(foo))
        self.assertIsNone(h.deleteContext('foo'))
        self.assertEqual(2, sys.getrefcount(foo))

    def test_notset(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        self.assertIsNone(h.deleteContext('foo'))


class TestCalloutHandle_getStatus(utils.BaseTestCase):

    def test_badarg_count(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        self.assert_method_no_arguments(h.getStatus)

    def test_ok(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        self.assertEqual(kea.NEXT_STEP_CONTINUE, h.getStatus())


class TestCalloutHandle_setStatus(utils.BaseTestCase):

    def test_badarg_count(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        self.assert_method_one_arg_no_keywords(h.setStatus)

    def test_badarg_type(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        with self.assertRaises(TypeError) as cm:
            h.setStatus('foo')
        self.assertEqual((utils.EXPECT_INT_GOT_STR,), cm.exception.args)

    def test_ok(self):
        h = kea.CalloutHandle(kea.CalloutManager())
        self.assertIsNone(h.setStatus(kea.NEXT_STEP_SKIP))
        self.assertEqual(kea.NEXT_STEP_SKIP, h.getStatus())
