import textwrap
from ipaddress import IPv4Network

import kea
import utils


class TestPkt4_new(utils.BaseTestCase):

    def test_noargs(self):
        with self.assertRaises(TypeError) as cm:
            kea.Pkt4()
        self.assertEqual(("function takes exactly 2 arguments (0 given)",), cm.exception.args)

    def test_toomanyargs(self):
        with self.assertRaises(TypeError) as cm:
            kea.Pkt4(1, 2, 3)
        self.assertEqual(("function takes exactly 2 arguments (3 given)",), cm.exception.args)

    def test_badarg(self):
        with self.assertRaises(TypeError) as cm:
            kea.Pkt4('1', 42)
        self.assertEqual(("an integer is required (got type str)",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.Pkt4(1, '42')
        self.assertEqual(("argument 2 must be int, not str",), cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual(1, p.use_count)


class TestPkt4_getType(utils.BaseTestCase):

    def test_toomanyargs(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.getType(1)
        self.assertEqual(("getType() takes no arguments (1 given)",), cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual(kea.DHCPREQUEST, p.getType())


class TestPkt4_setType(utils.BaseTestCase):

    def test_noargs(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.setType()
        self.assertEqual(("function takes exactly 1 argument (0 given)",), cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        p.setType(kea.DHCPNAK)
        self.assertEqual(kea.DHCPNAK, p.getType())


class TestPkt4_getRemoteAddr(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual('0.0.0.0', p.getRemoteAddr())


class TestPkt4_setRemoteAddr(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertIs(p, p.setRemoteAddr('1.2.3.4'))
        self.assertEqual('1.2.3.4', p.getRemoteAddr())


class TestPkt4_getLocalAddr(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual('0.0.0.0', p.getLocalAddr())


class TestPkt4_setLocalAddr(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertIs(p, p.setLocalAddr('1.2.3.4'))
        self.assertEqual('1.2.3.4', p.getLocalAddr())


class TestPkt4_getYiaddr(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual('0.0.0.0', p.getYiaddr())


class TestPkt4_setYiaddr(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertIs(p, p.setYiaddr('1.2.3.4'))
        self.assertEqual('1.2.3.4', p.getYiaddr())


class TestPkt4_getHWAddr(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual('', p.getHWAddr())


class TestPkt4_setHWAddr(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertIs(p, p.setHWAddr('01:02:03:04:05:06'))
        self.assertEqual('01:02:03:04:05:06', p.getHWAddr())


class TestPkt4_delOption(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertFalse(p.delOption(42))


class TestPkt4_addOption(utils.BaseTestCase):

    def test_empty_option_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        o = kea.Option(25)
        self.assertIs(p, p.addOption(o))
        self.assertEqual(2, o.use_count)

    def test_bytes_option_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        o = kea.Option(25)
        self.assertIs(p, p.addOption(o))
        self.assertEqual(2, o.use_count)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        o = kea.Option(kea.DHO_DHCP_AGENT_OPTIONS)
        self.assertEqual(1, o.use_count)
        self.assertIs(p, p.addOption(o))
        self.assertEqual(2, o.use_count)
        n = p.getOption(kea.DHO_DHCP_AGENT_OPTIONS)
        self.assertEqual(kea.DHO_DHCP_AGENT_OPTIONS, n.getType())


class TestPkt4_getOption(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        o = p.getOption(kea.DHO_DHCP_MESSAGE_TYPE)
        self.assertIsInstance(o, kea.Option)
        self.assertEqual(bytes([kea.DHCPREQUEST]), o.getBytes())


class TestPkt4_toText(utils.BaseTestCase):

    def test_empty(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual(textwrap.dedent("""\
            local_address=0.0.0.0:67, remote_address=0.0.0.0:68, msg_type=DHCPREQUEST (3), transid=0x2a,
            options:
              type=053, len=001: 3 (uint8)"""), p.toText())  # noqa: E501

    def test_filled(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)                                               # 53
        p.setLocalAddr('1.2.3.4')
        p.setRemoteAddr('2.3.4.5')
        subnet = IPv4Network('10.0.0.0/20')
        p.addOption(kea.Option(kea.DHO_SUBNET_MASK).setBytes(subnet.netmask.packed))    # 1
        p.addOption(kea.Option(kea.DHO_ROUTERS).setBytes(subnet[1].packed))             # 3
        p.addOption(kea.Option(kea.DHO_DOMAIN_NAME).setString('test.org'))              # 15
        p.addOption(kea.Option(kea.DHO_DHCP_LEASE_TIME).setUint32(7200))                # 51
        p.addOption(kea.Option(kea.DHO_DHCP_RENEWAL_TIME).setUint32(1800))              # 58
        p.addOption(kea.Option(kea.DHO_DHCP_REBINDING_TIME).setUint32(3600))            # 59
        self.assertEqual(textwrap.dedent("""\
            local_address=1.2.3.4:67, remote_address=2.3.4.5:68, msg_type=DHCPREQUEST (3), transid=0x2a,
            options:
              type=001, len=004: ff:ff:f0:00
              type=003, len=004: 0a:00:00:01
              type=015, len=008: 74:65:73:74:2e:6f:72:67
              type=051, len=004: 00:00:1c:20
              type=053, len=001: 3 (uint8)
              type=058, len=004: 00:00:07:08
              type=059, len=004: 00:00:0e:10"""), p.toText())  # noqa: E501
