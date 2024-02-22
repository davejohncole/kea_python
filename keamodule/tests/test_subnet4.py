import kea
import utils


class TestSubnet4_new(utils.BaseTestCase):

    def test_badarg_count(self):
        with self.assertRaises(TypeError) as cm:
            kea.Subnet4()
        self.assertEqual(("function missing required argument 'prefix' (pos 1)",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.Subnet4('foo')
        self.assertEqual(("function missing required argument 'length' (pos 2)",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.Subnet4('foo', 3)
        self.assertEqual(("function missing required argument 't1' (pos 3)",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.Subnet4('foo', 3, 1)
        self.assertEqual(("function missing required argument 't2' (pos 4)",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.Subnet4('foo', 3, 1, 2)
        self.assertEqual(("function missing required argument 'valid_lifetime' (pos 5)",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.Subnet4('foo', 3, 1, 2, 3)
        self.assertEqual(("function missing required argument 'id' (pos 6)",), cm.exception.args)

    def test_badarg_type(self):
        with self.assertRaises(TypeError) as cm:
            kea.Subnet4(1, 1, 1, 1, 1, 1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.Subnet4('foo', 'foo', 1, 1, 1, 1)
        self.assertEqual(("'str' object cannot be interpreted as an integer",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.Subnet4('foo', 1, 'foo', 1, 1, 1)
        self.assertEqual(("'t1' must be int or None",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.Subnet4('foo', 1, None, 'foo', 1, 1)
        self.assertEqual(("'t2' must be int or None",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.Subnet4('foo', 1, None, None, 'foo', 1)
        self.assertEqual(("'valid_lifetime' must be int or None",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.Subnet4('foo', 1, None, None, None, 'foo')
        self.assertEqual(("'str' object cannot be interpreted as an integer",), cm.exception.args)

    def test_ok(self):
        s = kea.Subnet4('192.168.1.0', 24, 900, 1800, 3600, 5)
        self.assertEqual({'id': 5,
                          '4o6-interface': '',
                          '4o6-interface-id': '',
                          '4o6-subnet': '',
                          'max-valid-lifetime': 3600,
                          'min-valid-lifetime': 3600,
                          'option-data': [],
                          'pools': [],
                          'relay': {'ip-addresses': []},
                          'renew-timer': 900,
                          'rebind-timer': 1800,
                          'valid-lifetime': 3600,
                          'subnet': '192.168.1.0/24'}, s.toElement())

    def test_ok_unspecified(self):
        s = kea.Subnet4('192.168.1.0', 24, None, None, None, 5)
        self.assertEqual({'id': 5,
                          '4o6-interface': '',
                          '4o6-interface-id': '',
                          '4o6-subnet': '',
                          'option-data': [],
                          'pools': [],
                          'relay': {'ip-addresses': []},
                          'subnet': '192.168.1.0/24'}, s.toElement())


class TestSubnet4_delServerTag(utils.BaseTestCase):

    def test_badarg_count(self):
        s = kea.Subnet4('192.168.1.0', 24, 900, 1800, 3600, 5)
        self.assert_method_one_arg_no_keywords(s.delServerTag)

    def test_ok(self):
        s = kea.Subnet4('192.168.1.0', 24, 900, 1800, 3600, 5)
        s.setServerTag('level3')
        s.delServerTag('level3')
        self.assertEqual(set(), s.getServerTags())


class TestSubnet4_getServerTags(utils.BaseTestCase):

    def test_badarg_count(self):
        s = kea.Subnet4('192.168.1.0', 24, 900, 1800, 3600, 5)
        self.assert_method_no_arguments(s.getServerTags)

    def test_ok(self):
        s = kea.Subnet4('192.168.1.0', 24, 900, 1800, 3600, 5)
        self.assertEqual(set(), s.getServerTags())
        s.setServerTag('level3')
        self.assertEqual(set(['level3']), s.getServerTags())


class TestSubnet4_getMetadata(utils.BaseTestCase):

    def test_badarg_count(self):
        s = kea.Subnet4('192.168.1.0', 24, 900, 1800, 3600, 5)
        self.assert_method_no_arguments(s.getMetadata)

    def test_ok(self):
        s = kea.Subnet4('192.168.1.0', 24, 900, 1800, 3600, 5)
        self.assertEqual({'server-tags': []}, s.getMetadata())
        s.setServerTag('level3')
        self.assertEqual({'server-tags': ['level3']}, s.getMetadata())


class TestSubnet4_getSharedNetworkName(utils.BaseTestCase):

    def test_badarg_count(self):
        s = kea.Subnet4('192.168.1.0', 24, 900, 1800, 3600, 5)
        self.assert_method_no_arguments(s.getSharedNetworkName)

    def test_ok(self):
        s = kea.Subnet4('192.168.1.0', 24, 900, 1800, 3600, 5)
        self.assertEqual('', s.getSharedNetworkName())
        s.setSharedNetworkName('level3')
        self.assertEqual('level3', s.getSharedNetworkName())


class TestSubnet4_setServerTag(utils.BaseTestCase):

    def test_badarg_count(self):
        s = kea.Subnet4('192.168.1.0', 24, 900, 1800, 3600, 5)
        self.assert_method_one_arg_no_keywords(s.setServerTag)

    def test_ok(self):
        s = kea.Subnet4('192.168.1.0', 24, 900, 1800, 3600, 5)
        s.setServerTag('one')
        self.assertEqual(set(['one']), s.getServerTags())
        s.setServerTag('two')
        self.assertEqual(set(['one', 'two']), s.getServerTags())


class TestSubnet4_setSharedNetworkName(utils.BaseTestCase):

    def test_badarg_count(self):
        s = kea.Subnet4('192.168.1.0', 24, 900, 1800, 3600, 5)
        self.assert_method_one_arg_no_keywords(s.setSharedNetworkName)

    def test_badarg_type(self):
        s = kea.Subnet4('192.168.1.0', 24, 900, 1800, 3600, 5)
        with self.assertRaises(TypeError) as cm:
            s.setSharedNetworkName(1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)

    def test_ok(self):
        s = kea.Subnet4('192.168.1.0', 24, 900, 1800, 3600, 5)
        s.setSharedNetworkName('level3')
        self.assertEqual('level3', s.getSharedNetworkName())
