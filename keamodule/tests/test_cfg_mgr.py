import kea
import utils


class TestCfgMgr_new(utils.BaseTestCase):

    def test_ok(self):
        self.assert_cannot_construct(kea.CfgMgr)


class TestCfgMgr_instance(utils.BaseTestCase):

    def test_badarg_count(self):
        self.assert_method_no_arguments(kea.CfgMgr.instance)

    def test_ok(self):
        m = kea.CfgMgr.instance()
        self.assertIsInstance(m, kea.CfgMgr)


class TestCfgMgr_getCurrentCfg(utils.BaseTestCase):

    def test_badarg_count(self):
        m = kea.CfgMgr.instance()
        self.assert_method_no_arguments(m.getCurrentCfg)

    def test_ok(self):
        m = kea.CfgMgr.instance()
        c = m.getCurrentCfg()
        self.assertIsInstance(c, kea.SrvConfig)


class TestCfgMgr_getStagingCfg(utils.BaseTestCase):

    def test_badarg_count(self):
        m = kea.CfgMgr.instance()
        self.assert_method_no_arguments(m.getStagingCfg)

    def test_ok(self):
        m = kea.CfgMgr.instance()
        c = m.getStagingCfg()
        self.assertIsInstance(c, kea.SrvConfig)
