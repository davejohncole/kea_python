import kea
import utils


class TestConfigBackendDHCPv4Mgr_new(utils.BaseTestCase):

    def test_ok(self):
        self.assert_cannot_construct(kea.ConfigBackendDHCPv4Mgr)


class TestConfigBackendDHCPv4Mgr_instance(utils.BaseTestCase):

    def test_badarg_count(self):
        self.assert_method_no_arguments(kea.ConfigBackendDHCPv4Mgr.instance)

    def test_ok(self):
        m = kea.ConfigBackendDHCPv4Mgr.instance()
        self.assertIsInstance(m, kea.ConfigBackendDHCPv4Mgr)


class TestConfigBackendDHCPv4Mgr_addBackend(utils.BaseTestCase):

    def test_badarg_count(self):
        m = kea.ConfigBackendDHCPv4Mgr.instance()
        self.assert_method_one_arg_no_keywords(m.addBackend)

    def test_ok(self):
        m = kea.ConfigBackendDHCPv4Mgr.instance()
        # have not added the mysql hook, so this will fail
        with self.assertRaises(TypeError) as cm:
            m.addBackend('type=mysql')
        self.assertEqual(("The type of the configuration backend: 'mysql' is not supported",), cm.exception.args)


class TestConfigBackendDHCPv4Mgr_getPool(utils.BaseTestCase):

    def test_badarg_count(self):
        m = kea.ConfigBackendDHCPv4Mgr.instance()
        self.assert_method_no_arguments(m.getPool)

    def test_ok(self):
        m = kea.ConfigBackendDHCPv4Mgr.instance()
        self.assertIsInstance(m.getPool(), kea.ConfigBackendPoolDHCPv4)
