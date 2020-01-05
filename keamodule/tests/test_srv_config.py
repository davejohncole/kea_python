import kea
import utils


class TestSrvConfig_new(utils.BaseTestCase):

    def test_ok(self):
        s = kea.CfgMgr().getCurrentCfg()
        self.assertEqual(3, s.use_count)

    def test_get_current_cfg(self):
        m = kea.CfgMgr()
        c = m.getCurrentCfg()
        self.assertEqual(3, c.use_count)
        d = m.getCurrentCfg()
        self.assertEqual(4, c.use_count)
        self.assertEqual(4, d.use_count)

    def test_staging(self):
        m = kea.CfgMgr()
        c = m.getStagingCfg()
        self.assertEqual(2, c.use_count)
        d = m.getStagingCfg()
        self.assertEqual(3, c.use_count)
        self.assertEqual(3, d.use_count)


class TestSrvConfig_toElement(utils.BaseTestCase):

    def test_ok(self):
        m = kea.CfgMgr()
        c = m.getStagingCfg()
        e = c.toElement()
        self.assertIsInstance(e, dict)
