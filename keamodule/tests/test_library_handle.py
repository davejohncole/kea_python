import kea
import utils


class TestLibraryHandle_new(utils.BaseTestCase):

    def test_badarg_count(self):
        self.assert_constructor_one_arg_no_keywords(kea.LibraryHandle)

    def test_ok(self):
        m = kea.CalloutManager()
        h = kea.LibraryHandle(m)
        # LibraryHandle has pointer to CalloutManager, not shared_ptr
        self.assertEqual(1, m.use_count)
        self.assertIsInstance(h, kea.LibraryHandle)


class TestLibraryHandle_registerCommandCallout(utils.BaseTestCase):

    def test_badarg_count(self):
        h = kea.LibraryHandle(kea.CalloutManager())
        self.assert_method_two_args_no_keywords(h.registerCommandCallout)

    def test_nadarg_type(self):
        h = kea.LibraryHandle(kea.CalloutManager())
        with self.assertRaises(TypeError) as cm:
            h.registerCommandCallout(1, 42)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            h.registerCommandCallout('foo', 42)
        self.assertEqual(("callout must be callable",), cm.exception.args)

    def test_ok(self):
        def foo():
            pass
        h = kea.LibraryHandle(kea.CalloutManager())
        with self.assertRaises(RuntimeError) as cm:
            h.registerCommandCallout('foo', foo)
        self.assertEqual(("only supported in embedded mode",), cm.exception.args)
