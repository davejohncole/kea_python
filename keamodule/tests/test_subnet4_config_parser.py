import kea
import utils


class TestSubnet4ConfigParser_new(utils.BaseTestCase):

    def test_badarg_count(self):
        self.assert_constructor_no_arguments(kea.Subnet4ConfigParser)


class TestSubnet4ConfigParser_parse(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Subnet4ConfigParser()
        self.assert_method_one_arg_no_keywords(p.parse)

    def test_badarg_type(self):
        p = kea.Subnet4ConfigParser()
        with self.assertRaises(TypeError) as cm:
            p.parse(None)
        self.assertEqual(("mapValue() called on non-map Element",), cm.exception.args)

    def test_ok(self):
        p = kea.Subnet4ConfigParser()
        s = p.parse({'subnet': '192.168.1.0/24',
                      'id': 1})
        self.assertEqual({'4o6-interface': '',
                          '4o6-interface-id': '',
                          '4o6-subnet': '',
                          'id': 1,
                          'option-data': [],
                          'pools': [],
                          'relay': {'ip-addresses': []},
                          'subnet': '192.168.1.0/24'}, s.toElement())
