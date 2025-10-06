import kea
import utils


class TestClientClassDictionary_new(utils.BaseTestCase):

    def test_badarg_count(self):
        self.assert_constructor_no_arguments(kea.ClientClassDictionary)


class TestClientClassDictionary_getClasses(utils.BaseTestCase):

    def test_badarg_count(self):
        cc_dict = kea.ClientClassDictionary()
        self.assert_method_no_arguments(cc_dict.getClasses)

    def test_ok(self):
        cc_dict = kea.ClientClassDictionary()
        classes = cc_dict.getClasses()
        self.assertEqual([], classes)
