import kea
import utils


class TestClientClassDef_new(utils.BaseTestCase):

    def test_badarg_count(self):
        self.assert_constructor_no_arguments(kea.ClientClassDef)


class TestClientClassDef_toElement(utils.BaseTestCase):

    def test_badarg_count(self):
        cc = kea.ClientClassDef()
        self.assert_method_no_arguments(cc.toElement)

    def test_uninitialized(self):
        cc = kea.ClientClassDef()
        with self.assertRaises(RuntimeError) as cm:
            cc.toElement()
        self.assertEqual(("ClientClassDef object is not initialized",), cm.exception.args)

    def test_ok(self):
        p = kea.ClientClassDefParser()
        cc_dict = kea.ClientClassDictionary()
        p.parse(cc_dict, {'name': 'foo',
                          'option-def': [{'name': 'configfile',
                                          'code': 224,
                                          'type': 'string'}],
                          'option-data': [{'name': 'configfile',
                                           'data': '1APC'}]})
        cc = cc_dict.getClasses()[0]
        self.assertEqual({'boot-file-name': '',
                          'name': 'foo',
                          'next-server': '0.0.0.0',
                          'option-data': [{'always-send': False,
                                           'code': 224,
                                           'csv-format': True,
                                           'data': '1APC',
                                           'name': 'configfile',
                                           'never-send': False,
                                           'space': 'dhcp4'}],
                          'option-def': [{'array': False,
                                          'code': 224,
                                          'encapsulate': '',
                                          'name': 'configfile',
                                          'record-types': '',
                                          'space': 'dhcp4',
                                          'type': 'string'}],
                          'server-hostname': ''}, cc.toElement())
