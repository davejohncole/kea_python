import kea
import utils


class TestCfgMgr_new(utils.BaseTestCase):

    def test_ok(self):
        kea.CfgMgr()

    def test_bad_args(self):
        with self.assertRaises(TypeError) as cm:
            kea.CfgMgr(1)
        self.assertEqual(('function takes exactly 0 arguments (1 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.CfgMgr(x=1)
        self.assertEqual(('keyword arguments are not supported',), cm.exception.args)
