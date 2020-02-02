import kea
import utils


class TestLeaseMgr_new(utils.BaseTestCase):

    def test_badarg_count(self):
        self.assert_constructor_no_arguments(kea.LeaseMgr)

    def test_ok(self):
        with self.assertRaises(TypeError) as cm:
            kea.LeaseMgr()
        self.assertEqual(("no current lease manager is available",), cm.exception.args)
