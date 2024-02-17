import kea
import utils


class TestServer_new(utils.BaseTestCase):

    def test_badarg_count(self):
        self.assert_constructor_two_args_no_keywords(kea.Server)

    def test_badarg_type(self):
        with self.assertRaises(TypeError) as cm:
            kea.Server(1, 1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.Server('foo', 1)
        self.assertEqual(("argument 2 must be str, not int",), cm.exception.args)

    def test_ok(self):
        s = kea.Server('kea', 'kea server')
        self.assertEqual('kea', s.getServerTagAsText())
        self.assertEqual('kea server', s.getDescription())


class TestServer_getServerTagAsText(utils.BaseTestCase):

    def test_badarg_count(self):
        s = kea.Server('kea', 'kea server')
        self.assert_method_no_arguments(s.getServerTagAsText)

    def test_ok(self):
        s = kea.Server('kea', 'kea server')
        self.assertEqual('kea', s.getServerTagAsText())


class TestServer_getDescription(utils.BaseTestCase):

    def test_badarg_count(self):
        s = kea.Server('kea', 'kea server')
        self.assert_method_no_arguments(s.getDescription)

    def test_ok(self):
        s = kea.Server('kea', 'kea server')
        self.assertEqual('kea server', s.getDescription())


class TestServer_toElement(utils.BaseTestCase):

    def test_badarg_count(self):
        s = kea.Server('kea', 'kea server')
        self.assert_method_no_arguments(s.toElement)

    def test_ok(self):
        s = kea.Server('kea', 'kea server')
        self.assertEqual({'server-tag': 'kea',
                          'description': 'kea server'}, s.toElement())
