import kea
import utils


class TestCfgSubnets4_usecount(utils.BaseTestCase):

    def test_usecount(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        self.assertEqual(2, n.use_count)
        o = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        self.assertEqual(3, o.use_count)
        o = None
        self.assertEqual(2, n.use_count)


class TestCfgSubnets4_new(utils.BaseTestCase):

    def test_cannot_construct(self):
        self.assert_cannot_construct(kea.CfgSubnets4)


class TestCfgSubnets4_add(utils.BaseTestCase):

    def test_badarg_count(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        self.assert_method_one_arg_no_keywords(n.add)

    def test_badarg_type(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        with self.assertRaises(TypeError) as cm:
            n.add('foo')
        self.assertEqual(("argument 1 must be kea.Subnet4, not str",), cm.exception.args)

    def test_ok(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        n.add(kea.Subnet4('192.168.0.0', 24, None, None, None, 1))
        try:
            subnets = n.getAll()
            self.assertEqual(1, len(subnets))
            self.assertEqual('192.168.0.0/24', subnets[0].toText())
        finally:
            n.clear()


class TestCfgSubnets4_clear(utils.BaseTestCase):

    def test_badarg_count(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        self.assert_method_no_arguments(n.clear)

    def test_ok(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        n.add(kea.Subnet4('192.168.0.0', 24, None, None, None, 1))
        n.clear()
        self.assertEqual([], n.getAll())


class TestCfgSubnets4_delSubnetID(utils.BaseTestCase):

    def test_badarg_count(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        self.assert_method_one_arg_no_keywords(n.delSubnetID)

    def test_badarg_type(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        with self.assertRaises(TypeError) as cm:
            n.delSubnetID('foo')
        self.assertEqual((utils.EXPECT_INT_GOT_STR,), cm.exception.args)

    def test_ok(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        n.add(kea.Subnet4('192.168.0.0', 24, None, None, None, 1))
        subnets = n.getAll()
        self.assertEqual(1, len(subnets))
        self.assertIsNone(n.delSubnetID(1))
        self.assertEqual([], n.getAll())


class TestCfgSubnets4_getAll(utils.BaseTestCase):

    def test_badarg_count(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        self.assert_method_no_arguments(n.getAll)

    def test_ok(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        self.assertEqual([], n.getAll())


class TestCfgSubnets4_getSubnet(utils.BaseTestCase):

    def test_badarg_count(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        self.assert_method_one_arg_no_keywords(n.getSubnet)

    def test_badarg_type(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        with self.assertRaises(TypeError) as cm:
            n.getSubnet('foo')
        self.assertEqual(("argument 1 must be int, not str",), cm.exception.args)

    def test_ok(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        self.assertIsNone(n.getSubnet(1))
        n.add(kea.Subnet4('192.168.0.0', 24, None, None, None, 1))
        try:
            self.assertEqual('192.168.0.0/24', n.getSubnet(1).toText())
        finally:
            n.clear()


class TestCfgSubnets4_replace(utils.BaseTestCase):

    def test_badarg_count(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        self.assert_method_one_arg_no_keywords(n.replace)

    def test_badarg_type(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        with self.assertRaises(TypeError) as cm:
            n.replace(1)
        self.assertEqual(("argument 1 must be kea.Subnet4, not int",), cm.exception.args)

    def test_ok(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        with self.assertRaises(TypeError) as cm:
            n.replace(kea.Subnet4('192.168.1.0', 24, None, None, None, 1))
        self.assertEqual(('There is no IPv4 subnet with ID 1',), cm.exception.args)
        n.add(kea.Subnet4('192.168.0.0', 24, None, None, None, 1))
        try:
            n.replace(kea.Subnet4('192.168.1.0', 24, None, None, None, 1))
            self.assertEqual('192.168.1.0/24', n.getSubnet(1).toText())
        finally:
            n.clear()


class TestCfgSubnets4_selectSubnet(utils.BaseTestCase):

    def test_badarg_count(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        self.assert_method_one_arg_no_keywords(n.selectSubnet)

    def test_badarg_type(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        with self.assertRaises(TypeError) as cm:
            n.selectSubnet(1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)

    def test_ok(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        self.assertIsNone(n.selectSubnet('192.168.0.1'))


class TestCfgSubnets4_toElement(utils.BaseTestCase):

    def test_badarg_type(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        self.assert_method_no_arguments(n.toElement)

    def test_ok(self):
        n = kea.CfgMgr.instance().getCurrentCfg().getCfgSubnets4()
        self.assertEqual([], n.toElement())
        n.add(kea.Subnet4('192.168.0.0', 24, None, None, None, 1))
        try:
            self.assertEqual([{'4o6-interface': '',
                            '4o6-interface-id': '',
                            '4o6-subnet': '',
                            'id': 1,'option-data': [],
                            'pools': [],
                            'relay': {'ip-addresses': []},
                            'subnet': '192.168.0.0/24'}], n.toElement())
        finally:
            n.clear()
