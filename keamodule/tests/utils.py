import sys
import unittest

import kea


class Logger:

    def error(self, msg):
        sys.stderr.write(msg + '\n')


class BaseTestCase(unittest.TestCase):

    def setUp(self):
        self.maxDiff = None
        kea.logger = Logger()

    def assert_cannot_construct(self, cls):
        with self.assertRaises(RuntimeError) as cm:
            cls()
        self.assertEqual(("cannot directly construct",), cm.exception.args)

    def assert_constructor_no_arguments(self, cls):
        with self.assertRaises(TypeError) as cm:
            cls(1)
        self.assertEqual(('function takes exactly 0 arguments (1 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            cls(x=1)
        self.assertEqual(('keyword arguments are not supported',), cm.exception.args)

    def assert_constructor_one_arg_no_keywords(self, cls):
        with self.assertRaises(TypeError) as cm:
            cls()
        self.assertEqual(('function takes exactly 1 argument (0 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            cls(1, 2)
        self.assertEqual(('function takes exactly 1 argument (2 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            cls(x=1)
        self.assertEqual(('keyword arguments are not supported',), cm.exception.args)

    def assert_constructor_two_args_no_keywords(self, cls):
        with self.assertRaises(TypeError) as cm:
            cls()
        self.assertEqual(('function takes exactly 2 arguments (0 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            cls(1)
        self.assertEqual(('function takes exactly 2 arguments (1 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            cls(1, 2, 3)
        self.assertEqual(('function takes exactly 2 arguments (3 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            cls(x=1)
        self.assertEqual(('keyword arguments are not supported',), cm.exception.args)

    def assert_method_no_arguments(self, method):
        with self.assertRaises(TypeError) as cm:
            method(1)
        msg = '%s() takes no arguments (1 given)' % method.__qualname__
        self.assertEqual((msg,), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            method(x=1)
        msg = '%s() takes no keyword arguments' % method.__qualname__
        self.assertEqual((msg,), cm.exception.args)

    def assert_method_one_arg_no_keywords(self, method):
        with self.assertRaises(TypeError) as cm:
            method()
        self.assertEqual(('function takes exactly 1 argument (0 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            method(1, 2)
        self.assertEqual(('function takes exactly 1 argument (2 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            method(x=1)
        msg = '%s() takes no keyword arguments' % method.__name__
        self.assertEqual((msg,), cm.exception.args)

    def assert_method_two_args_no_keywords(self, method):
        with self.assertRaises(TypeError) as cm:
            method()
        self.assertEqual(('function takes exactly 2 arguments (0 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            method(1)
        self.assertEqual(('function takes exactly 2 arguments (1 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            method(1, 2, 3)
        self.assertEqual(('function takes exactly 2 arguments (3 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            method(x=1)
        msg = '%s() takes no keyword arguments' % method.__name__
        self.assertEqual((msg,), cm.exception.args)

