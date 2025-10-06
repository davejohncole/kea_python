import kea
import utils


class TestClientClassDefParser_new(utils.BaseTestCase):

    def test_badarg_count(self):
        self.assert_constructor_no_arguments(kea.ClientClassDefParser)


class TestClientClassDefParser_parse(utils.BaseTestCase):

    def test_badarg_count(self):
        p = kea.ClientClassDefParser()
        self.assert_method_two_args_no_keywords(p.parse)

    def test_badarg_type(self):
        p = kea.ClientClassDefParser()
        cc_dict = kea.ClientClassDictionary()
        with self.assertRaises(TypeError) as cm:
            p.parse(None, {})
        self.assertEqual(("argument 1 must be kea.ClientClassDictionary, not NoneType",), cm.exception.args)
        with self.assertRaises(TypeError) as cm:
            p.parse(cc_dict, None)
        self.assertEqual(("mapValue() called on non-map Element",), cm.exception.args)

    def test_ok(self):
        p = kea.ClientClassDefParser()
        cc_dict = kea.ClientClassDictionary()
        result = p.parse(cc_dict, {'name': 'foo',
                                   'option-def': [{
                                       'name': 'configfile',
                                       'code': 224,
                                       'type': 'string'
                                    }],
                                   'option-data': [{
                                       'name': 'configfile',
                                       'data': '1APC'
                                    }]
                                  })
        self.assertIsNone(result)