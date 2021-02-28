import kea
import utils


class TestCfgSubnets4_new(utils.BaseTestCase):

    def test_cannot_construct(self):
        self.assert_cannot_construct(kea.CfgSubnets4)


class TestCfgSubnets4_getAll(utils.BaseTestCase):

    def test_badarg_count(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        self.assert_method_no_arguments(n.getAll)

    def test_ok(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        self.assertEqual([], n.getAll())
        self.assertEqual(2, n.use_count)
        o = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        self.assertEqual(3, o.use_count)
        o = None
        self.assertEqual(2, n.use_count)


class TestCfgSubnets4_getSubnet(utils.BaseTestCase):

    def test_badarg_count(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        self.assert_method_one_arg_no_keywords(n.getSubnet)

    def test_badarg_type(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        with self.assertRaises(TypeError) as cm:
            n.getSubnet('foo')
        self.assertEqual(("argument 1 must be int, not str",), cm.exception.args)

    def test_ok(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        self.assertIsNone(n.getSubnet(1))


class TestCfgSubnets4_selectSubnet(utils.BaseTestCase):

    def test_badarg_count(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        self.assert_method_one_arg_no_keywords(n.selectSubnet)

    def test_badarg_type(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        with self.assertRaises(TypeError) as cm:
            n.selectSubnet(1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)

    def test_ok(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        self.assertIsNone(n.selectSubnet('192.168.0.1'))


class TestCfgSubnets4_toElement(utils.BaseTestCase):

    def test_badarg_type(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        self.assert_method_no_arguments(n.toElement)

    def test_ok(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        self.assertEqual([], n.toElement())
