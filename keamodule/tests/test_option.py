import codecs

import kea
import utils


class TestOption_new(utils.BaseTestCase):

    def test_ok(self):
        o = kea.Option(42)
        self.assertEqual(1, o.use_count)


class TestOption_getType(utils.BaseTestCase):

    def test_ok(self):
        o = kea.Option(42)
        self.assertIs(42, o.getType())


class TestOption_setBytes(utils.BaseTestCase):

    def test_ok(self):
        o = kea.Option(42)
        self.assertIs(o, o.setBytes(b'123'))


class TestOption_getBytes(utils.BaseTestCase):

    def test_ok(self):
        o = kea.Option(42)
        o.setBytes(b'123')
        self.assertEqual(b'123', o.getBytes())


class TestOption_setString(utils.BaseTestCase):

    def test_ok(self):
        o = kea.Option(42)
        self.assertIs(o, o.setString('Pokémon'))


class TestOption_getString(utils.BaseTestCase):

    def test_ok(self):
        o = kea.Option(42)
        o.setString('Pokémon')
        self.assertEqual('Pokémon', o.getString())


class TestOption_setUint8(utils.BaseTestCase):

    def test_ok(self):
        o = kea.Option(42)
        self.assertIs(o, o.setUint8(0xfe))


class TestOption_getUint8(utils.BaseTestCase):

    def test_ok(self):
        o = kea.Option(42)
        o.setUint8(0xfe)
        self.assertEqual(0xfe, o.getUint8())
        self.assertEqual(b'\xfe', o.getBytes())


class TestOption_setUint16(utils.BaseTestCase):

    def test_ok(self):
        o = kea.Option(42)
        self.assertIs(o, o.setUint16(0xfeed))


class TestOption_getUint16(utils.BaseTestCase):

    def test_ok(self):
        o = kea.Option(42)
        o.setUint16(0xfeed)
        self.assertEqual(0xfeed, o.getUint16())
        self.assertEqual(b'\xfe\xed', o.getBytes())


class TestOption_setUint32(utils.BaseTestCase):

    def test_ok(self):
        o = kea.Option(42)
        self.assertIs(o, o.setUint32(0x01020304))


class TestOption_getUint32(utils.BaseTestCase):

    def test_ok(self):
        o = kea.Option(42)
        o.setUint32(0x01020304)
        self.assertEqual(0x01020304, o.getUint32())
        self.assertEqual(b'\x01\x02\x03\x04', o.getBytes())


class TestOption_addOption(utils.BaseTestCase):

    def test_ok(self):
        o = kea.Option(42)
        p = kea.Option(2).setUint8(0xef)
        self.assertIs(o, o.addOption(p))


class TestOption_getOption(utils.BaseTestCase):

    def test_ok(self):
        o = kea.Option(42).addOption(kea.Option(2).setUint8(0xef))
        p = o.getOption(2)
        self.assertIsInstance(p, kea.Option)
        self.assertEqual(2, p.getType())
        self.assertEqual(0xef, p.getUint8())


class TestOption_pack(utils.BaseTestCase):

    def test_ok(self):
        o = kea.Option(42).addOption(kea.Option(2).setUint8(0xef))
        wire = o.pack()
        self.assertIsInstance(wire, bytes)
        self.assertEqual(b'2a030201ef', codecs.encode(wire, 'hex'))


class TestOption_toText(utils.BaseTestCase):

    def test_empty(self):
        o = kea.Option(42)
        self.assertEqual('type=042, len=000: ', o.toText())

    def test_uint8(self):
        o = kea.Option(42).setUint8(5)
        self.assertEqual('type=042, len=001: 05', o.toText())

    def test_nested(self):
        o = kea.Option(42).addOption(kea.Option(4)
                                     .setUint16(5)).addOption(kea.Option(6)
                                                              .setString('hello'))
        self.assertEqual("""\
type=042, len=011: ,
options:
  type=004, len=002: 00:05
  type=006, len=005: 68:65:6c:6c:6f""", o.toText())
