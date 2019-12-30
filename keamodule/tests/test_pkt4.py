import unittest

import kea
import utils


class TestLease4_new(utils.BaseTestCase):

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
        self.assertEqual(("an integer is required (got type str)",), cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual(1, p.use_count)


class TestLease4_getType(utils.BaseTestCase):

    def test_toomanyargs(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.getType(1)
        self.assertEqual(("getType() takes no arguments (1 given)",), cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual(kea.DHCPREQUEST, p.getType())


class TestLease4_setType(utils.BaseTestCase):

    def test_noargs(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        with self.assertRaises(TypeError) as cm:
            p.setType()
        self.assertEqual(("function takes exactly 1 argument (0 given)",), cm.exception.args)

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        p.setType(kea.DHCPNAK)
        self.assertEqual(kea.DHCPNAK, p.getType())


class TestLease4_getRemoteAddr(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual('0.0.0.0', p.getRemoteAddr())


class TestLease4_setRemoteAddr(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertIsNone(p.setRemoteAddr('1.2.3.4'))
        self.assertEqual('1.2.3.4', p.getRemoteAddr())


class TestLease4_getYiaddr(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual('0.0.0.0', p.getYiaddr())


class TestLease4_setYiaddr(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertIsNone(p.setYiaddr('1.2.3.4'))
        self.assertEqual('1.2.3.4', p.getYiaddr())


class TestLease4_getHWAddr(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertEqual('', p.getHWAddr())


class TestLease4_setHWAddr(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertIsNone(p.setHWAddr('01:02:03:04:05:06'))
        self.assertEqual('01:02:03:04:05:06', p.getHWAddr())


class TestLease4_delOption(utils.BaseTestCase):

    def test_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        self.assertFalse(p.delOption(42))


class TestLease4_addOption(utils.BaseTestCase):

    def test_empty_option_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        o = kea.Option(25)
        self.assertIsNone(p.addOption(o))
        self.assertEqual(2, o.use_count)

    def test_bytes_option_ok(self):
        p = kea.Pkt4(kea.DHCPREQUEST, 42)
        o = kea.Option(25)
        self.assertIsNone(p.addOption(o))
        self.assertEqual(2, o.use_count)
