import kea
import utils


class TestSrvConfig_new(utils.BaseTestCase):

    def test_cannot_construct(self):
        with self.assertRaises(RuntimeError) as cm:
            kea.SrvConfig()
        self.assertEqual(("cannot directly construct",), cm.exception.args)


class TestGetCurrentConfig(utils.BaseTestCase):

    def test_badarg_count(self):
        m = kea.CfgMgr()
        self.assert_method_no_arguments(m.getCurrentCfg)

    def test_ok(self):
        m = kea.CfgMgr()
        c = m.getCurrentCfg()
        self.assertEqual(3, c.use_count)
        d = m.getCurrentCfg()
        self.assertEqual(4, c.use_count)
        self.assertEqual(4, d.use_count)


class TestGetStagingConfig(utils.BaseTestCase):

    def test_badarg_count(self):
        m = kea.CfgMgr()
        self.assert_method_no_arguments(m.getStagingCfg)

    def test_ok(self):
        m = kea.CfgMgr()
        c = m.getStagingCfg()
        self.assertEqual(2, c.use_count)
        d = m.getStagingCfg()
        self.assertEqual(3, c.use_count)
        self.assertEqual(3, d.use_count)
        d = None
        self.assertEqual(2, c.use_count)


class TestSrvConfig_getCfgSubnets4(utils.BaseTestCase):

    def test_badarg_count(self):
        c = kea.CfgMgr().getStagingCfg()
        self.assert_method_no_arguments(c.getCfgSubnets4)

    def test_ok(self):
        c = kea.CfgMgr().getStagingCfg()
        n = c.getCfgSubnets4()
        self.assertEqual(2, n.use_count)


class TestSrvConfig_toElement(utils.BaseTestCase):

    def test_badarg_count(self):
        c = kea.CfgMgr().getStagingCfg()
        self.assert_method_no_arguments(c.toElement)

    def test_ok(self):
        c = kea.CfgMgr().getStagingCfg()
        e = c.toElement()
        self.assertIsInstance(e, dict)
