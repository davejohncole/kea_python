import kea
import utils


class TestCfgSubnets4_getAll(utils.BaseTestCase):

    def test_ok(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        self.assertEqual([], n.getAll())

    def test_bad_args(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        with self.assertRaises(TypeError) as cm:
            n.getAll(1)
        self.assertEqual(('getAll() takes no arguments (1 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            n.getAll(x=1)
        self.assertEqual(('getAll() takes no keyword arguments',), cm.exception.args)


class TestCfgSubnets4_getSubnet(utils.BaseTestCase):

    def test_ok(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        self.assertIsNone(n.getSubnet(1))

    def test_bad_args(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        with self.assertRaises(TypeError) as cm:
            n.getSubnet()
        self.assertEqual(('function takes exactly 1 argument (0 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            n.getSubnet(x=1)
        self.assertEqual(('getSubnet() takes no keyword arguments',), cm.exception.args)


class TestCfgSubnets4_selectSubnet(utils.BaseTestCase):

    def test_ok(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        self.assertIsNone(n.selectSubnet('192.168.0.1'))

    def test_bad_args(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        with self.assertRaises(TypeError) as cm:
            n.selectSubnet()
        self.assertEqual(('function takes exactly 1 argument (0 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            n.selectSubnet(x=1)
        self.assertEqual(('selectSubnet() takes no keyword arguments',), cm.exception.args)


class TestCfgSubnets4_toElement(utils.BaseTestCase):

    def test_ok(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        self.assertEqual([], n.toElement())

    def test_bad_args(self):
        n = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        with self.assertRaises(TypeError) as cm:
            n.toElement(1)
        self.assertEqual(('toElement() takes no arguments (1 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            n.toElement(x=1)
        self.assertEqual(('toElement() takes no keyword arguments',), cm.exception.args)
