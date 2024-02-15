import kea
import utils


class TestSubnet4_new(utils.BaseTestCase):

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

    def test_unspecified(self):
        s = kea.Subnet4('192.168.1.0', 24, None, None, None, 5)
        self.assertEqual({'id': 5,
                          '4o6-interface': '',
                          '4o6-interface-id': '',
                          '4o6-subnet': '',
                          'option-data': [],
                          'pools': [],
                          'relay': {'ip-addresses': []},
                          'subnet': '192.168.1.0/24'}, s.toElement())


class TestSubnet4_sharedNetworkName(utils.BaseTestCase):

    def test_ok(self):
        s = kea.Subnet4('192.168.1.0', 24, 900, 1800, 3600, 5)
        self.assertEqual('', s.getSharedNetworkName())
        s.setSharedNetworkName('level3')
        self.assertEqual('level3', s.getSharedNetworkName())
