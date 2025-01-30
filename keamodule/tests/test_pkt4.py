import textwrap
from ipaddress import IPv4Network

import kea
import utils


# https://fossies.org/dox/kea-1.7.4/classisc_1_1dhcp_1_1Pkt4.html
class TestPkt4_new(utils.BaseTestCase):

    def test_badarg_count(self):
        # 1 or 2 args
        with self.assertRaises(TypeError) as cm:
            kea.Pkt4()
        self.assertEqual(('function takes exactly 2 arguments (0 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.Pkt4(1, 2, 3)
        self.assertEqual(('function takes exactly 2 arguments (3 given)',), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.Pkt4(x=1)
        self.assertEqual(('keyword arguments are not supported',), cm.exception.args)

    def test_badarg_type(self):
        # 1 arg - bytes
        with self.assertRaises(TypeError) as cm:
            kea.Pkt4(42)
        self.assertEqual(("argument 1 must be bytes, not int",), cm.exception.args)
        # 2 args - type, trans_id
        with self.assertRaises(TypeError) as cm:
            kea.Pkt4('1', 42)
        self.assertEqual((utils.EXPECT_INT_GOT_STR,), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            kea.Pkt4(1, '42')
        self.assertEqual(("argument 2 must be int, not str",), cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual(1, p.use_count)


class TestPkt4_getType(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_no_arguments(p.getType)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual(kea.DHCPREQUEST, p.getType())


class TestPkt4_setType(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_one_arg_no_keywords(p.setType)

    def test_badarg_type(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.setType('foo')
        self.assertEqual((utils.EXPECT_INT_GOT_STR,), cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        p.setType(kea.DHCPNAK)
        self.assertEqual(kea.DHCPNAK, p.getType())


class TestPkt4_getFlags(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_no_arguments(p.getFlags)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual(0, p.getFlags())


class TestPkt4_setFlags(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_one_arg_no_keywords(p.setFlags)

    def test_badarg_type(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.setFlags('foo')
        self.assertEqual((utils.EXPECT_INT_GOT_STR,), cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        p.setFlags(0xbeef)
        self.assertEqual(0xbeef, p.getFlags())


class TestPkt4_getLocalAddr(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_no_arguments(p.getLocalAddr)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual('0.0.0.0', p.getLocalAddr())


class TestPkt4_setLocalAddr(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_one_arg_no_keywords(p.setLocalAddr)

    def test_badarg_type(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.setLocalAddr(1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.setLocalAddr('foo')
        self.assertEqual(("Failed to convert string to address 'foo': Invalid argument",),
                         cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertIs(p, p.setLocalAddr('1.2.3.4'))
        self.assertEqual('1.2.3.4', p.getLocalAddr())


class TestPkt4_getRemoteAddr(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_no_arguments(p.getRemoteAddr)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual('0.0.0.0', p.getRemoteAddr())


class TestPkt4_setRemoteAddr(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_one_arg_no_keywords(p.setRemoteAddr)

    def test_badarg_type(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.setRemoteAddr(1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.setRemoteAddr('foo')
        self.assertEqual(("Failed to convert string to address 'foo': Invalid argument",),
                         cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertIs(p, p.setRemoteAddr('1.2.3.4'))
        self.assertEqual('1.2.3.4', p.getRemoteAddr())


class TestPkt4_getCiaddr(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_no_arguments(p.getCiaddr)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual('0.0.0.0', p.getCiaddr())


class TestPkt4_setCiaddr(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_one_arg_no_keywords(p.setCiaddr)

    def test_badarg_type(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.setCiaddr(1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.setCiaddr('foo')
        self.assertEqual(("Failed to convert string to address 'foo': Invalid argument",),
                         cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertIs(p, p.setCiaddr('1.2.3.4'))
        self.assertEqual('1.2.3.4', p.getCiaddr())


class TestPkt4_getGiaddr(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_no_arguments(p.getGiaddr)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual('0.0.0.0', p.getGiaddr())


class TestPkt4_setGiaddr(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_one_arg_no_keywords(p.setGiaddr)

    def test_badarg_type(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.setGiaddr(1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.setGiaddr('foo')
        self.assertEqual(("Failed to convert string to address 'foo': Invalid argument",),
                         cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertIs(p, p.setGiaddr('1.2.3.4'))
        self.assertEqual('1.2.3.4', p.getGiaddr())


class TestPkt4_getSiaddr(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_no_arguments(p.getSiaddr)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual('0.0.0.0', p.getSiaddr())


class TestPkt4_setSiaddr(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_one_arg_no_keywords(p.setSiaddr)

    def test_badarg_type(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.setSiaddr(1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.setSiaddr('foo')
        self.assertEqual(("Failed to convert string to address 'foo': Invalid argument",),
                         cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertIs(p, p.setSiaddr('1.2.3.4'))
        self.assertEqual('1.2.3.4', p.getSiaddr())


class TestPkt4_getYiaddr(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_no_arguments(p.getYiaddr)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual('0.0.0.0', p.getYiaddr())


class TestPkt4_setYiaddr(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_one_arg_no_keywords(p.setYiaddr)

    def test_badarg_type(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.setYiaddr(1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.setYiaddr('foo')
        self.assertEqual(("Failed to convert string to address 'foo': Invalid argument",),
                         cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertIs(p, p.setYiaddr('1.2.3.4'))
        self.assertEqual('1.2.3.4', p.getYiaddr())


class TestPkt4_getHWAddr(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_no_arguments(p.getHWAddr)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual('', p.getHWAddr())


class TestPkt4_setHWAddr(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_one_arg_no_keywords(p.setHWAddr)

    def test_badarg_type(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.setHWAddr(1)
        self.assertEqual(("argument 1 must be str, not int",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.setHWAddr('foo')
        self.assertEqual(("invalid format of the decoded string 'foo'",),
                         cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertIs(p, p.setHWAddr('01:02:03:04:05:06'))
        self.assertEqual('01:02:03:04:05:06', p.getHWAddr())


class TestPkt4_delOption(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_one_arg_no_keywords(p.delOption)

    def test_badarg_type(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.delOption('foo')
        self.assertEqual((utils.EXPECT_INT_GOT_STR,), cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertFalse(p.delOption(42))


class TestPkt4_addOption(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_one_arg_no_keywords(p.addOption)

    def test_badarg_type(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.addOption('foo')
        self.assertEqual(("argument 1 must be kea.Option, not str",), cm.exception.args)

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

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_one_arg_no_keywords(p.getOption)

    def test_badarg_type(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.getOption('foo')
        self.assertEqual((utils.EXPECT_INT_GOT_STR,), cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        o = p.getOption(kea.DHO_DHCP_MESSAGE_TYPE)
        self.assertIsInstance(o, kea.Option)
        self.assertEqual(bytes([kea.DHCPREQUEST]), o.getBytes())


class TestPkt4_pack(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_no_arguments(p.pack)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        wire = p.pack()
        self.assertIsInstance(wire, bytes)


class TestPkt4_unpack(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_no_arguments(p.unpack)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        wire = p.pack()
        p = kea.Pkt4(wire)
        self.assertIsNone(p.unpack())


class TestPkt4_toText(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assert_method_no_arguments(p.toText)

    def test_empty(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        if kea.__version__ >= '2.6.1':
            self.assertEqual(textwrap.dedent("""\
                local_address=0.0.0.0:67, remote_address=0.0.0.0:68,
                msg_type=DHCPREQUEST (3), trans_id=0x2a,
                options:
                  type=053, len=001: 3 (uint8)"""), p.toText())
        else:
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
        if kea.__version__ >= '2.6.1':
            self.assertEqual(textwrap.dedent("""\
                local_address=1.2.3.4:67, remote_address=2.3.4.5:68,
                msg_type=DHCPREQUEST (3), trans_id=0x2a,
                options:
                  type=001, len=004: ff:ff:f0:00
                  type=003, len=004: 0a:00:00:01
                  type=015, len=008: 74:65:73:74:2e:6f:72:67
                  type=051, len=004: 00:00:1c:20
                  type=053, len=001: 3 (uint8)
                  type=058, len=004: 00:00:07:08
                  type=059, len=004: 00:00:0e:10"""), p.toText())
        else:
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
