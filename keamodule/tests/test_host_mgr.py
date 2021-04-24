import kea
import utils


class TestHostMgr_new(utils.BaseTestCase):

    def test_badarg_count(self):
        self.assert_cannot_construct(kea.HostMgr)


class TestHostMgr_instance(utils.BaseTestCase):

    def test_badarg_count(self):
        self.assert_method_no_arguments(kea.HostMgr.instance)

    def test_ok(self):
        m = kea.HostMgr.instance()
        self.assertIsInstance(m, kea.HostMgr)


class TestHostMgr_add(utils.BaseTestCase):

    def test_badarg_count(self):
        m = kea.HostMgr.instance()
        self.assert_method_one_arg_no_keywords(m.add)

    def test_badarg_type(self):
        m = kea.HostMgr.instance()
        with self.assertRaises(TypeError) as cm:
            m.add('foo')
        self.assertEqual(("argument 1 must be kea.Host, not str",), cm.exception.args)
