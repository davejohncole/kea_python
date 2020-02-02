import kea
import utils


class TestLease4_new(utils.BaseTestCase):

    def test_badarg_count(self):
        self.assert_constructor_no_arguments(kea.Lease4)

    def test_ok(self):
        x = kea.Lease4()
        self.assertEqual(1, x.use_count)


class TestLease4_addr(utils.BaseTestCase):

    def test_getset(self):
        x = kea.Lease4()
        self.assertEqual('0.0.0.0', x.addr)
        x.addr = '192.168.0.1'
        self.assertEqual('192.168.0.1', x.addr)

    def test_bad_type(self):
        x = kea.Lease4()
        with self.assertRaises(TypeError) as cm:
            x.addr = 'bogus'
        self.assertEqual(("Failed to convert string to address 'bogus': Invalid argument",),
                         cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            x.addr = 0
        self.assertEqual(('The addr attribute value must be a str',), cm.exception.args)


class TestLease4_valid_lft(utils.BaseTestCase):

    def test_getset(self):
        x = kea.Lease4()
        self.assertEqual(0, x.valid_lft)
        x.valid_lft = 3600
        self.assertEqual(3600, x.valid_lft)

    def test_bad_type(self):
        x = kea.Lease4()
        with self.assertRaises(TypeError) as cm:
            x.valid_lft = 'bogus'
        self.assertEqual(('The valid_lft attribute value must be an int',), cm.exception.args)


class TestLease4_cltt(utils.BaseTestCase):

    def test_getset(self):
        x = kea.Lease4()
        self.assertEqual(0, x.cltt)
        x.cltt = 3600
        self.assertEqual(3600, x.cltt)

    def test_bad_type(self):
        x = kea.Lease4()
        with self.assertRaises(TypeError) as cm:
            x.cltt = 'bogus'
        self.assertEqual(('The cltt attribute value must be an int',), cm.exception.args)


class TestLease4_subnet_id(utils.BaseTestCase):

    def test_getset(self):
        x = kea.Lease4()
        self.assertEqual(0, x.subnet_id)
        x.subnet_id = 5
        self.assertEqual(5, x.subnet_id)

    def test_bad_type(self):
        x = kea.Lease4()
        with self.assertRaises(TypeError) as cm:
            x.subnet_id = 'bogus'
        self.assertEqual(('The subnet_id attribute value must be an int',), cm.exception.args)


class TestLease4_hostname(utils.BaseTestCase):

    def test_getset(self):
        x = kea.Lease4()
        self.assertIsNone(x.hostname)
        x.hostname = 'example.org'
        self.assertEqual('example.org', x.hostname)
        x.hostname = None
        self.assertIsNone(x.hostname)

    def test_bad_type(self):
        x = kea.Lease4()
        with self.assertRaises(TypeError) as cm:
            x.hostname = 3
        self.assertEqual(('The hostname attribute value must be a str',), cm.exception.args)


class TestLease4_fqdn_fwd(utils.BaseTestCase):

    def test_getset(self):
        x = kea.Lease4()
        self.assertEqual(False, x.fqdn_fwd)
        x.fqdn_fwd = True
        self.assertEqual(True, x.fqdn_fwd)
        x.fqdn_fwd = False
        self.assertEqual(False, x.fqdn_fwd)

    def test_bad_type(self):
        x = kea.Lease4()
        with self.assertRaises(TypeError) as cm:
            x.fqdn_fwd = 'bogus'
        self.assertEqual(('The fqdn_fwd attribute value must be a bool',), cm.exception.args)


class TestLease4_fqdn_rev(utils.BaseTestCase):

    def test_getset(self):
        x = kea.Lease4()
        self.assertEqual(False, x.fqdn_rev)
        x.fqdn_rev = True
        self.assertEqual(True, x.fqdn_rev)
        x.fqdn_rev = False
        self.assertEqual(False, x.fqdn_rev)

    def test_bad_type(self):
        x = kea.Lease4()
        with self.assertRaises(TypeError) as cm:
            x.fqdn_rev = 'bogus'
        self.assertEqual(('The fqdn_rev attribute value must be a bool',), cm.exception.args)


class TestLease4_hwaddr(utils.BaseTestCase):

    def test_getset(self):
        x = kea.Lease4()
        self.assertIsNone(x.hwaddr)
        x.hwaddr = '01:02:03:04:05:06'
        self.assertEqual('01:02:03:04:05:06', x.hwaddr)

    def test_bad_type(self):
        x = kea.Lease4()
        with self.assertRaises(TypeError) as cm:
            x.hwaddr = 'bogus'
        self.assertEqual(("invalid format of the decoded string 'bogus'",), cm.exception.args)


class TestLease4_client_id(utils.BaseTestCase):

    def test_getset(self):
        x = kea.Lease4()
        self.assertIsNone(x.client_id)
        x.client_id = '01:02:03:04:05:06'
        self.assertEqual('01:02:03:04:05:06', x.client_id)

    def test_bad_type(self):
        x = kea.Lease4()
        with self.assertRaises(TypeError) as cm:
            x.client_id = 'bogus'
        self.assertEqual(("'bogus' is not a valid string of hexadecimal digits",),
                         cm.exception.args)


class TestLease4_state(utils.BaseTestCase):

    def test_getset(self):
        x = kea.Lease4()
        self.assertEqual(0, x.state)
        x.state = 5
        self.assertEqual(5, x.state)

    def test_bad_type(self):
        x = kea.Lease4()
        with self.assertRaises(TypeError) as cm:
            x.state = 'bogus'
        self.assertEqual(('The state attribute value must be an int',), cm.exception.args)


class TestLease4_setContext(utils.BaseTestCase):

    def test_badarg_count(self):
        x = kea.Lease4()
        self.assert_method_one_arg_no_keywords(x.setContext)

    def test_ok(self):
        x = kea.Lease4()
        self.assertIs(x, x.setContext('foo'))
        self.assertIs(x, x.setContext(2))
        self.assertIs(x, x.setContext(True))
        self.assertIs(x, x.setContext([1, 'foo']))
        self.assertIs(x, x.setContext({'foo': 'bar'}))
        self.assertIs(x, x.setContext(None))


class TestLease4_getContext(utils.BaseTestCase):

    def test_badarg_count(self):
        x = kea.Lease4()
        self.assert_method_no_arguments(x.getContext)

    def test_ok(self):
        x = kea.Lease4()
        self.assertIsNone(x.getContext())
        x.setContext(None)
        self.assertIsNone(x.getContext())
        x.setContext('foo')
        self.assertEqual('foo', x.getContext())
        x.setContext(2)
        self.assertEqual(2, x.getContext())
        x.setContext(True)
        self.assertEqual(True, x.getContext())
        x.setContext([1, 'foo'])
        self.assertEqual([1, 'foo'], x.getContext())
        x.setContext({'foo': 'bar'})
        self.assertEqual({'foo': 'bar'}, x.getContext())


class TestLease4_toElement(utils.BaseTestCase):

    def test_badarg_count(self):
        x = kea.Lease4()
        self.assert_method_no_arguments(x.toElement)

    def test_empty(self):
        x = kea.Lease4()
        with self.assertRaises(RuntimeError) as cm:
            self.assertEqual({}, x.toElement())
        self.assertIsInstance(cm.exception, RuntimeError)
        self.assertEqual(("hwaddr must not be empty",), cm.exception.args)

    def test_ok(self):
        x = kea.Lease4()
        x.hwaddr = '01:02:03:04:05:06'
        self.assertEqual({'cltt': 0,
                          'fqdn-fwd': False,
                          'fqdn-rev': False,
                          'hostname': '',
                          'hw-address': '01:02:03:04:05:06',
                          'ip-address': '0.0.0.0',
                          'state': 0,
                          'subnet-id': 0,
                          'valid-lft': 0}, x.toElement())
        x.cltt = 3600
        x.fqdn_fwd = x.fqdn_rev = True
        x.hostname = 'example.com'
        x.addr = '192.168.0.1'
        x.state = 3
        x.subnet_id = 4
        x.valid_lft = 1800
        x.client_id = '02:03:04:05:06:07'
        self.assertEqual({'client-id': '02:03:04:05:06:07',
                          'cltt': 3600,
                          'fqdn-fwd': True,
                          'fqdn-rev': True,
                          'hostname': 'example.com',
                          'hw-address': '01:02:03:04:05:06',
                          'ip-address': '192.168.0.1',
                          'state': 3,
                          'subnet-id': 4,
                          'valid-lft': 1800}, x.toElement())
