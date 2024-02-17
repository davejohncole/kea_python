import kea
import utils


class TestConfigBackendPoolDHCPv4_new(utils.BaseTestCase):

    def test_ok(self):
        self.assert_cannot_construct(kea.ConfigBackendPoolDHCPv4)


class TestConfigBackendPoolDHCPv4_createUpdateServer4(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        self.assert_method_two_args_no_keywords(p.createUpdateServer4)

    def test_badarg_type(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        with self.assertRaises(TypeError) as cm:
            p.createUpdateServer4(1, kea.Server('kea', 'kea server'))
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.createUpdateServer4('all', 'foo')
        self.assertEqual(("argument 2 must be kea.Server, not str",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.createUpdateServer4('all', kea.Server('kea', 'kea server'))
        self.assertEqual(("unrecognised selector: all",), cm.exception.args)

    def test_ok(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        with self.assertRaises(TypeError) as cm:
            p.createUpdateServer4('unspec', kea.Server('kea', 'kea server'))
        self.assertEqual(("no such database found for selector: unspecified",), cm.exception.args)
        # have not added the mysql hook, so this will fail
        with self.assertRaises(TypeError) as cm:
            p.createUpdateServer4('mysql', kea.Server('kea', 'kea server'))
        self.assertEqual(("no such database found for selector: type=mysql",), cm.exception.args)


class TestConfigBackendPoolDHCPv4_createUpdateSubnet4(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        self.assert_method_three_args_no_keywords(p.createUpdateSubnet4)

    def test_badarg_type(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        subnet = kea.Subnet4('192.168.0.0', 24, None, None, None, 1)
        with self.assertRaises(TypeError) as cm:
            p.createUpdateSubnet4(1, 1, 1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.createUpdateSubnet4('unspec', 1, subnet)
        self.assertEqual(("server selector must be string or list of strings",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.createUpdateSubnet4('unspec', 'all', 1)
        self.assertEqual(("argument 3 must be kea.Subnet4, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.createUpdateSubnet4('unspec', [], subnet)
        self.assertEqual(("ServerSelector: expecting at least one server tag",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.createUpdateSubnet4('unspec', [1], subnet)
        self.assertEqual(("expected string for server selector element 0",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.createUpdateSubnet4('all', 'one', subnet)
        self.assertEqual(("unrecognised selector: all",), cm.exception.args)

    def test_ok(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        subnet = kea.Subnet4('192.168.0.0', 24, None, None, None, 1)
        with self.assertRaises(TypeError) as cm:
            p.createUpdateSubnet4('unspec', 'one', subnet)
        self.assertEqual(("no such database found for selector: unspecified",), cm.exception.args)
        # have not added the mysql hook, so this will fail
        with self.assertRaises(TypeError) as cm:
            p.createUpdateSubnet4('mysql', 'one', subnet)
        self.assertEqual(("no such database found for selector: type=mysql",), cm.exception.args)


class TestConfigBackendPoolDHCPv4_deleteSubnet4(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        self.assert_method_three_args_no_keywords(p.deleteSubnet4)

    def test_badarg_type(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        with self.assertRaises(TypeError) as cm:
            p.deleteSubnet4(1, 1, 1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.deleteSubnet4('unspec', 1, 1)
        self.assertEqual(("server selector must be string or list of strings",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.deleteSubnet4('unspec', 'all', 'foo')
        self.assertEqual(("'str' object cannot be interpreted as an integer",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.deleteSubnet4('unspec', [], 1)
        self.assertEqual(("ServerSelector: expecting at least one server tag",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.deleteSubnet4('unspec', [1], 1)
        self.assertEqual(("expected string for server selector element 0",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.deleteSubnet4('all', 'one', 1)
        self.assertEqual(("unrecognised selector: all",), cm.exception.args)

    def test_ok(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        with self.assertRaises(TypeError) as cm:
            p.deleteSubnet4('unspec', 'one', 1)
        self.assertEqual(("no such database found for selector: unspecified",), cm.exception.args)
        # have not added the mysql hook, so this will fail
        with self.assertRaises(TypeError) as cm:
            p.deleteSubnet4('mysql', 'one', 1)
        self.assertEqual(("no such database found for selector: type=mysql",), cm.exception.args)


class TestConfigBackendPoolDHCPv4_deleteServer4(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        self.assert_method_two_args_no_keywords(p.deleteServer4)

    def test_badarg_type(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        with self.assertRaises(TypeError) as cm:
            p.deleteServer4(1, 1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.deleteServer4('unspec', 1)
        self.assertEqual(("argument 2 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.deleteServer4('all', 'one')
        self.assertEqual(("unrecognised selector: all",), cm.exception.args)

    def test_ok(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        with self.assertRaises(TypeError) as cm:
            p.deleteServer4('unspec', 'one')
        self.assertEqual(("no such database found for selector: unspecified",), cm.exception.args)
        # have not added the mysql hook, so this will fail
        with self.assertRaises(TypeError) as cm:
            p.deleteServer4('mysql', 'one')
        self.assertEqual(("no such database found for selector: type=mysql",), cm.exception.args)


class TestConfigBackendPoolDHCPv4_getAllServers4(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        self.assert_method_one_arg_no_keywords(p.getAllServers4)

    def test_badarg_type(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        with self.assertRaises(TypeError) as cm:
            p.getAllServers4(1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.getAllServers4('all')
        self.assertEqual(("unrecognised selector: all",), cm.exception.args)

    def test_ok(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        with self.assertRaises(TypeError) as cm:
            p.getAllServers4('unspec')
        self.assertEqual(("no such database found for selector: unspecified",), cm.exception.args)
        # have not added the mysql hook, so this will fail
        with self.assertRaises(TypeError) as cm:
            p.getAllServers4('mysql')
        self.assertEqual(("no such database found for selector: type=mysql",), cm.exception.args)


class TestConfigBackendPoolDHCPv4_getSubnet4(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        self.assert_method_three_args_no_keywords(p.getSubnet4)

    def test_badarg_type(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        with self.assertRaises(TypeError) as cm:
            p.getSubnet4(1, 1, 'foo')
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.getSubnet4('unspec', 1, 'foo')
        self.assertEqual(("'str' object cannot be interpreted as an integer",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.getSubnet4('unspec', 'all', 'foo')
        self.assertEqual(("'str' object cannot be interpreted as an integer",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.getSubnet4('unspec', [], 1)
        self.assertEqual(("ServerSelector: expecting at least one server tag",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.getSubnet4('unspec', [1], 1)
        self.assertEqual(("expected string for server selector element 0",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.getSubnet4('all', 'one', 1)
        self.assertEqual(("unrecognised selector: all",), cm.exception.args)

    def test_ok(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        self.assertIsNone(p.getSubnet4('unspec', 'one', 1))


class TestConfigBackendPoolDHCPv4_getAllSubnets4(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        self.assert_method_two_args_no_keywords(p.getAllSubnets4)

    def test_badarg_type(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        with self.assertRaises(TypeError) as cm:
            p.getAllSubnets4(1, 1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.getAllSubnets4('unspec', 1)
        self.assertEqual(("server selector must be string or list of strings",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.getAllSubnets4('unspec', [])
        self.assertEqual(("ServerSelector: expecting at least one server tag",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.getAllSubnets4('unspec', [1])
        self.assertEqual(("expected string for server selector element 0",), cm.exception.args)

    def test_ok(self):
        p = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        self.assertEqual([], p.getAllSubnets4('unspec', 'one'))
