import kea
import utils


class TestGetCurrentConfig(utils.BaseTestCase):

    def test_ok(self):
        m = kea.CfgMgr()
        c = m.getCurrentCfg()
        self.assertEqual(3, c.use_count)
        d = m.getCurrentCfg()
        self.assertEqual(4, c.use_count)
        self.assertEqual(4, d.use_count)

    def test_bad_args(self):
        m = kea.CfgMgr()
        with self.assertRaises(TypeError) as cm:
            m.getCurrentCfg(1)
        self.assertEqual(('getCurrentCfg() takes no arguments (1 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            m.getCurrentCfg(x=1)
        self.assertEqual(('getCurrentCfg() takes no keyword arguments',), cm.exception.args)


class TestGetStagingConfig(utils.BaseTestCase):

    def test_ok(self):
        m = kea.CfgMgr()
        c = m.getStagingCfg()
        self.assertEqual(2, c.use_count)
        d = m.getStagingCfg()
        self.assertEqual(3, c.use_count)
        self.assertEqual(3, d.use_count)

    def test_bad_args(self):
        m = kea.CfgMgr()
        with self.assertRaises(TypeError) as cm:
            m.getStagingCfg(1)
        self.assertEqual(('getStagingCfg() takes no arguments (1 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            m.getStagingCfg(x=1)
        self.assertEqual(('getStagingCfg() takes no keyword arguments',), cm.exception.args)


class TestSrvConfig_getCfgSubnets4(utils.BaseTestCase):

    def test_ok(self):
        c = kea.CfgMgr().getStagingCfg()
        n = c.getCfgSubnets4()
        self.assertIsInstance(n, kea.CfgSubnets4)
        self.assertEqual(2, n.use_count)

    def test_bad_args(self):
        c = kea.CfgMgr().getStagingCfg()
        with self.assertRaises(TypeError) as cm:
            c.getCfgSubnets4(1)
        self.assertEqual(('getCfgSubnets4() takes no arguments (1 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            c.getCfgSubnets4(x=1)
        self.assertEqual(('getCfgSubnets4() takes no keyword arguments',), cm.exception.args)


class TestSrvConfig_toElement(utils.BaseTestCase):

    def test_ok(self):
        c = kea.CfgMgr().getStagingCfg()
        e = c.toElement()
        self.assertIsInstance(e, dict)

    def test_bad_args(self):
        c = kea.CfgMgr().getStagingCfg()
        with self.assertRaises(TypeError) as cm:
            c.toElement(1)
        self.assertEqual(('toElement() takes no arguments (1 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            c.toElement(x=1)
        self.assertEqual(('toElement() takes no keyword arguments',), cm.exception.args)
