import copy
import difflib
import json
import socket
import textwrap
import time
import unittest


lease4_args = {'addr': 'ip-address',
               'valid_lft': 'valid-lft',
               'subnet_id': 'subnet-id',
               'fqdn_fwd': 'fqdn-fwd',
               'fqdn_rev': 'fqdn-rev',
               'hwaddr': 'hw-address',
               'client_id': 'client-id',
               'from_': 'from',
               'ident': 'identifier',
               'ident_type': 'identifier-type'}


def send_command(name, **kwargs):
    args = {}
    for k, v in kwargs.items():
        args[lease4_args.get(k, k)] = v
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.connect('/var/run/kea4.sock')
    cmd = {'command': name}
    if args:
        cmd['arguments'] = args
    request = json.dumps(cmd, sort_keys=True)
    print(request)
    s.send(request.encode('utf-8'))
    bits = []
    while True:
        buff = s.recv(4096)
        if not buff:
            break
        bits.append(buff)
    response = b''.join(bits).decode('utf-8')
    print(response)
    return json.loads(response)


def fix_cltt(eargs, rargs):
    if 'cltt' in eargs and 'cltt' in rargs:
        cltt_1 = eargs['cltt']
        cltt_2 = rargs['cltt']
        if isinstance(cltt_1, int) and isinstance(cltt_2, int):
            if abs(cltt_1 - cltt_2) <= 1:
                rargs['cltt'] = eargs['cltt']


class ResponseTestCase(unittest.TestCase):

    def setUp(self):
        self.maxDiff = None

    def assertResponse(self, expected, response, alt_text=None):
        if 'arguments' in expected and 'arguments' in response:
            # single lease
            eargs = expected['arguments']
            rargs = response['arguments']
            if isinstance(eargs, dict) and isinstance(rargs, dict):
                if 'leases' in eargs and 'leases' in rargs:
                    eleases = eargs['leases']
                    rleases = rargs['leases']
                    if isinstance(eleases, list) and isinstance(rleases, list):
                        for elease, rlease in zip(eleases, rleases):
                            fix_cltt(elease, rlease)
                else:
                    fix_cltt(eargs, rargs)
        ejson = json.dumps(expected, sort_keys=True, indent=2)
        rjson = json.dumps(response, sort_keys=True, indent=2)
        if alt_text is None:
            self.assertMultiLineEqual(ejson, rjson)
            return
        if response == expected:
            return
        alt_expected = copy.deepcopy(expected)
        alt_expected['text'] = alt_text
        if response == alt_expected:
            return
        aejson = json.dumps(alt_expected, sort_keys=True, indent=2)
        diff = '\n'.join(difflib.ndiff(ejson.splitlines(),
                                       rjson.splitlines()))
        alt_diff = '\n'.join(difflib.ndiff(aejson.splitlines(),
                                           rjson.splitlines()))
        raise self.failureException('%s != response options:\n%s\nand\n%s'
                                    % (response,
                                       textwrap.indent(diff, '  ').rstrip(),
                                       textwrap.indent(alt_diff, '  ').rstrip()))


class TestLease4Add(ResponseTestCase):

    def test_ok(self):
        response = send_command('lease4-add',
                                addr='172.28.5.12',
                                hwaddr='1a:1b:1c:1d:1e:1f')
        self.assertResponse({
                "text": "Lease for address 172.28.5.12, subnet-id 5 added.",
                "result": 0
            },
            response,
            "lease for address 172.28.5.12, subnet-id 5 added")

    def test_bad_network(self):
        response = send_command('lease4-add',
                                addr='192.0.2.202',
                                hwaddr='1a:1b:1c:1d:1e:1f')
        self.assertResponse({
                "text": ("subnet-id not specified and failed to find a subnet"
                         " for address 192.0.2.202"),
                "result": 1
            },
            response)

    def test_bad_addr_value(self):
        response = send_command('lease4-add',
                                addr='oops')
        self.assertResponse({
                "text": ("Failed to convert 'oops' to address: Failed to convert string to "
                         "address 'oops': Invalid argument(<wire>:0:29)"),
                "result": 1
            },
            response,
            "Failed to convert string to address 'oops': Invalid argument")

    def test_bad_addr_type(self):
        response = send_command('lease4-add',
                                addr=42)
        self.assertResponse({
                "text": "invalid type specified for parameter 'ip-address' (<wire>:0:29)",
                "result": 1
            },
            response,
            "'ip-address' is not a string")

    def test_no_args(self):
        response = send_command('lease4-add')
        self.assertResponse({
                "text": "no parameters specified for the command",
                "result": 1
            },
            response,
            "parameters missing or is not a map")

    def test_missing_addr(self):
        response = send_command('lease4-add',
                                hwaddr='1a:1b:1c:1d:1e:1f')
        self.assertResponse({
                "text": "missing parameter 'ip-address' (<wire>:0:15)",
                "result": 1
            },
            response,
            "'ip-address' parameter not specified")


class TestLeaseGet(ResponseTestCase):

    def test_ok(self):
        send_command('lease4-wipe')
        response = send_command('lease4-get',
                                addr='172.28.6.12')
        self.assertResponse({
                "text": "Lease not found.",
                "result": 3
            },
            response,
            "lease not found")
        response = send_command('lease4-get',
                                ident='1a:1b:1c:1d:1e:1f',
                                ident_type='hw-address',
                                subnet_id=5)
        self.assertResponse({
                "text": "Lease not found.",
                "result": 3
            },
            response,
            "lease not found")
        send_command('lease4-add',
                     addr='172.28.5.12',
                     hwaddr='1a:1b:1c:1d:1e:1f')
        response = send_command('lease4-get',
                                addr='172.28.5.12')
        self.assertResponse({
                'arguments': {'cltt': 0,                # TODO int(time.time()),
                              'fqdn-fwd': False,
                              'fqdn-rev': False,
                              'hostname': '',
                              'hw-address': '1a:1b:1c:1d:1e:1f',
                              'ip-address': '172.28.5.12',
                              'state': 0,
                              'subnet-id': 5,
                              'valid-lft': 4000},
                'result': 0,
                'text': 'IPv4 lease found.'
            },
            response,
            "IPv4 lease found")
        response = send_command('lease4-get',
                                ident='1a:1b:1c:1d:1e:1f',
                                ident_type='hw-address',
                                subnet_id=5)
        self.assertResponse({
                'arguments': {'cltt': 0,                # TODO int(time.time()),
                              'fqdn-fwd': False,
                              'fqdn-rev': False,
                              'hostname': '',
                              'hw-address': '1a:1b:1c:1d:1e:1f',
                              'ip-address': '172.28.5.12',
                              'state': 0,
                              'subnet-id': 5,
                              'valid-lft': 4000},
                'result': 0,
                'text': 'IPv4 lease found.'
            },
            response,
            "IPv4 lease found")
        send_command('lease4-add',
                     addr='172.28.6.12',
                     hwaddr='2a:2b:2c:2d:2e:2f',
                     client_id='01:02:03:04:05:06')
        response = send_command('lease4-get',
                                ident='01:02:03:04:05:06',
                                ident_type='client-id',
                                subnet_id=6)
        self.assertResponse({
                'arguments': {'client-id': '01:02:03:04:05:06',
                              'cltt': 0,                # TODO int(time.time()),
                              'fqdn-fwd': False,
                              'fqdn-rev': False,
                              'hostname': '',
                              'hw-address': '2a:2b:2c:2d:2e:2f',
                              'ip-address': '172.28.6.12',
                              'state': 0,
                              'subnet-id': 6,
                              'valid-lft': 4000},
                'result': 0,
                'text': 'IPv4 lease found.'
            },
            response,
            "IPv4 lease found")

    def test_bad_addr(self):
        response = send_command('lease4-get',
                                addr='bogus')
        self.assertResponse({
                "text": "Failed to convert string to address 'bogus': Invalid argument",
                "result": 1
            },
            response)

    def test_bad_hwaddr(self):
        send_command('lease4-wipe')
        response = send_command('lease4-get',
                                ident='bogus',
                                ident_type='hw-address',
                                subnet_id=5)
        self.assertResponse({
                "text": "invalid format of the decoded string 'bogus'",
                "result": 1
            },
            response)

    def test_not_found_by_client_id(self):
        send_command('lease4-wipe')
        response = send_command('lease4-get',
                                ident='01:02:03:04:05:06',
                                ident_type='client-id',
                                subnet_id=5)
        self.assertResponse({
                "text": "Lease not found.",
                "result": 3
            },
            response,
            "lease not found")

    def test_missing_args(self):
        response = send_command('lease4-get')
        self.assertResponse({
                'result': 1,
                'text': 'Parameters missing or are not a map.'
            },
            response,
            "'subnet-id' parameter not specified")

    def test_missing_subnet_id(self):
        response = send_command('lease4-get',
                                hwaddr='01:02:03:04:05:06')
        self.assertResponse({
                'result': 1,
                'text': "Mandatory 'subnet-id' parameter missing."
            },
            response,
            "'subnet-id' parameter not specified")

    def test_missing_ip_address(self):
        response = send_command('lease4-get',
                                subnet_id=5)
        self.assertResponse({
                'result': 1,
                'text': ("No 'ip-address' provided and 'identifier-type' is either missing or "
                         "not a string.")
            },
            response,
            "no 'ip-address' provided and 'identifier-type' is either missing or not a string")

    def test_bad_id_type(self):
        response = send_command('lease4-get',
                                ident='01:02:03:04:05:06',
                                ident_type='bogus',
                                subnet_id=5)
        self.assertResponse({
                'result': 1,
                'text': ('Incorrect identifier type: bogus, the only supported values are: '
                         'address, hw-address, duid')
            },
            response,
            ("'identifier-type' bogus is not supported, the only supported values are:"
             " address, hw-address, client-id"))


class TestLeaseGetAll(ResponseTestCase):

    def test_ok(self):
        send_command('lease4-wipe')
        response = send_command('lease4-get-all')
        self.assertResponse({
                "text": "0 IPv4 lease(s) found.",
                "arguments": {
                    "leases": [],
                },
                "result": 3
            },
            response,
            "0 IPv4 lease(s) found")
        send_command('lease4-add',
                     addr='172.28.5.12',
                     hwaddr='1a:1b:1c:1d:1e:1f')
        send_command('lease4-add',
                     addr='172.28.6.12',
                     hwaddr='2a:2b:2c:2d:2e:2f')
        response = send_command('lease4-get-all')
        self.assertResponse({
                "text": "2 IPv4 lease(s) found.",
                "arguments": {
                    "leases": [{'cltt': 0,              # TODO int(time.time()),
                                'fqdn-fwd': False,
                                'fqdn-rev': False,
                                'hostname': '',
                                'hw-address': '1a:1b:1c:1d:1e:1f',
                                'ip-address': '172.28.5.12',
                                'state': 0,
                                'subnet-id': 5,
                                'valid-lft': 4000},
                               {'cltt': 0,              # TODO int(time.time()),
                                'fqdn-fwd': False,
                                'fqdn-rev': False,
                                'hostname': '',
                                'hw-address': '2a:2b:2c:2d:2e:2f',
                                'ip-address': '172.28.6.12',
                                'state': 0,
                                'subnet-id': 6,
                                'valid-lft': 4000}]
                },
                "result": 0
            },
            response,
            "2 IPv4 lease(s) found")
        response = send_command('lease4-get-all',
                                subnets=[5, 6])
        self.assertResponse({
                "text": "2 IPv4 lease(s) found.",
                "arguments": {
                    "leases": [{'cltt': 0,              # TODO int(time.time()),
                                'fqdn-fwd': False,
                                'fqdn-rev': False,
                                'hostname': '',
                                'hw-address': '1a:1b:1c:1d:1e:1f',
                                'ip-address': '172.28.5.12',
                                'state': 0,
                                'subnet-id': 5,
                                'valid-lft': 4000},
                               {'cltt': 0,              # TODO int(time.time()),
                                'fqdn-fwd': False,
                                'fqdn-rev': False,
                                'hostname': '',
                                'hw-address': '2a:2b:2c:2d:2e:2f',
                                'ip-address': '172.28.6.12',
                                'state': 0,
                                'subnet-id': 6,
                                'valid-lft': 4000}]
                },
                "result": 0
            },
            response,
            "2 IPv4 lease(s) found")
        response = send_command('lease4-get-all',
                                subnets=[5])
        self.assertResponse({
                "text": "1 IPv4 lease(s) found.",
                "arguments": {
                    "leases": [{'cltt': 0,              # TODO int(time.time()),
                                'fqdn-fwd': False,
                                'fqdn-rev': False,
                                'hostname': '',
                                'hw-address': '1a:1b:1c:1d:1e:1f',
                                'ip-address': '172.28.5.12',
                                'state': 0,
                                'subnet-id': 5,
                                'valid-lft': 4000}]
                },
                "result": 0
            },
            response,
            "1 IPv4 lease(s) found")

    def test_bad_subnets(self):
        response = send_command('lease4-get-all',
                                subnets=['bogus'])
        self.assertResponse({
                "text": "listed subnet identifiers must be numbers",
                "result": 1
            },
            response)


class TestLease4GetPage(ResponseTestCase):

    def test_ok(self):
        send_command('lease4-wipe')
        send_command('lease4-add',
                     addr='172.28.5.12',
                     hwaddr='1a:1b:1c:1d:1e:1f')
        send_command('lease4-add',
                     addr='172.28.6.12',
                     hwaddr='2a:2b:2c:2d:2e:2f')
        response = send_command('lease4-get-page',
                                from_='start',
                                limit=1024)
        self.assertResponse({
                "text": "2 IPv4 lease(s) found.",
                "arguments": {
                    "count": 2,
                    "leases": [{'cltt': 0,              # TODO int(time.time()),
                                'fqdn-fwd': False,
                                'fqdn-rev': False,
                                'hostname': '',
                                'hw-address': '1a:1b:1c:1d:1e:1f',
                                'ip-address': '172.28.5.12',
                                'state': 0,
                                'subnet-id': 5,
                                'valid-lft': 4000},
                               {'cltt': 0,              # TODO int(time.time()),
                                'fqdn-fwd': False,
                                'fqdn-rev': False,
                                'hostname': '',
                                'hw-address': '2a:2b:2c:2d:2e:2f',
                                'ip-address': '172.28.6.12',
                                'state': 0,
                                'subnet-id': 6,
                                'valid-lft': 4000}]
                },
                "result": 0
            },
            response,
            "2 IPv4 lease(s) found")
        response = send_command('lease4-get-page',
                                from_='start',
                                limit=1)
        self.assertResponse({
                "text": "1 IPv4 lease(s) found.",
                "arguments": {
                    "count": 1,
                    "leases": [{'cltt': 0,              # TODO int(time.time()),
                                'fqdn-fwd': False,
                                'fqdn-rev': False,
                                'hostname': '',
                                'hw-address': '1a:1b:1c:1d:1e:1f',
                                'ip-address': '172.28.5.12',
                                'state': 0,
                                'subnet-id': 5,
                                'valid-lft': 4000}]
                },
                "result": 0
            },
            response,
            "1 IPv4 lease(s) found")
        response = send_command('lease4-get-page',
                                from_='172.28.5.12',
                                limit=1)
        self.assertResponse({
                "text": "1 IPv4 lease(s) found.",
                "arguments": {
                    "count": 1,
                    "leases": [{'cltt': 0,              # TODO int(time.time()),
                                'fqdn-fwd': False,
                                'fqdn-rev': False,
                                'hostname': '',
                                'hw-address': '2a:2b:2c:2d:2e:2f',
                                'ip-address': '172.28.6.12',
                                'state': 0,
                                'subnet-id': 6,
                                'valid-lft': 4000}]
                },
                "result": 0
            },
            response,
            "1 IPv4 lease(s) found")
        response = send_command('lease4-get-page',
                                from_='172.28.6.12',
                                limit=1)
        self.assertResponse({
                "text": "0 IPv4 lease(s) found.",
                "arguments": {
                    "count": 0,
                    "leases": []
                },
                "result": 3
            },
            response,
            "0 IPv4 lease(s) found")

    def test_bad_addr(self):
        response = send_command('lease4-get-page',
                                from_='bogus',
                                limit=1)
        self.assertResponse({
                "text": ("'from' parameter value is neither 'start' keyword nor a valid IPv4 "
                         "address"),
                "result": 1
            },
            response,
            "Failed to convert string to address 'bogus': Invalid argument")

    def test_bad_limit(self):
        response = send_command('lease4-get-page',
                                from_='0.0.0.0',
                                limit=0)
        self.assertResponse({
                "text": "page size of retrieved leases must not be 0",
                "result": 1
            },
            response,
            "'limit' is not an integer")

    def test_bad_limit_type(self):
        response = send_command('lease4-get-page',
                                from_='0.0.0.0',
                                limit='bogus')
        self.assertResponse({
                "text": "'limit' parameter must be a number",
                "result": 1
            },
            response,
            "'limit' is not an integer")

    def test_missing_limit(self):
        response = send_command('lease4-get-page',
                                from_='0.0.0.0')
        self.assertResponse({
                "text": "'limit' parameter not specified",
                "result": 1
            },
            response)


class TestGetLease4ByHwAddr(ResponseTestCase):

    def test_ok(self):
        response = send_command('list-commands')
        if 'lease4-get-by-hw-address' not in response['arguments']:
            return
        send_command('lease4-wipe')
        send_command('lease4-add',
                     addr='172.28.5.12',
                     hwaddr='1a:1b:1c:1d:1e:1f')
        send_command('lease4-add',
                     addr='172.28.6.12',
                     hwaddr='2a:2b:2c:2d:2e:2f')
        response = send_command('lease4-get-by-hw-address',
                                hwaddr='1a:1b:1c:1d:1e:1f')
        self.assertResponse({
                'arguments': {
                    'count': 1,
                    'leases': [{'cltt': 0,      # TODO int(time.time()),
                                'fqdn-fwd': False,
                                'fqdn-rev': False,
                                'hostname': '',
                                'hw-address': '1a:1b:1c:1d:1e:1f',
                                'ip-address': '172.28.5.12',
                                'state': 0,
                                'subnet-id': 5,
                                'valid-lft': 4000}]
                },
                'result': 0,
                'text': '1 IPv4 lease(s) found'
            },
            response)


class TestGetLease4ByClientId(ResponseTestCase):

    def test_ok(self):
        response = send_command('list-commands')
        if 'lease4-get-by-client-id' not in response['arguments']:
            return
        send_command('lease4-wipe')
        send_command('lease4-add',
                     addr='172.28.5.12',
                     hwaddr='1a:1b:1c:1d:1e:1f',
                     client_id='01:02:03:04:05:06')
        send_command('lease4-add',
                     addr='172.28.6.12',
                     hwaddr='2a:2b:2c:2d:2e:2f')
        response = send_command('lease4-get-by-client-id',
                                client_id='01:02:03:04:05:06')
        self.assertResponse({
                'arguments': {
                    'count': 1,
                    'leases': [{'cltt': 0,    # TODO int(time.time()),
                                'fqdn-fwd': False,
                                'fqdn-rev': False,
                                'hostname': '',
                                'hw-address': '1a:1b:1c:1d:1e:1f',
                                'ip-address': '172.28.5.12',
                                'client-id': '01:02:03:04:05:06',
                                'state': 0,
                                'subnet-id': 5,
                                'valid-lft': 4000}]
                },
                'result': 0,
                'text': '1 IPv4 lease(s) found'
            },
            response)


class TestGetLease4ByHostname(ResponseTestCase):

    def test_ok(self):
        response = send_command('list-commands')
        if 'lease4-get-by-hostname' not in response['arguments']:
            return
        send_command('lease4-wipe')
        send_command('lease4-add',
                     addr='172.28.5.12',
                     hwaddr='1a:1b:1c:1d:1e:1f',
                     hostname='foo.example.com')
        send_command('lease4-add',
                     addr='172.28.6.12',
                     hwaddr='2a:2b:2c:2d:2e:2f',
                     hostname='bar.example.com')
        response = send_command('lease4-get-by-hostname',
                                hostname='foo.example.com')
        self.assertResponse({
                'arguments': {'cltt': int(time.time()),
                              'fqdn-fwd': False,
                              'fqdn-rev': False,
                              'hostname': '',
                              'hw-address': '1a:1b:1c:1d:1e:1f',
                              'ip-address': '172.28.5.12',
                              'state': 0,
                              'subnet-id': 5,
                              'valid-lft': 4000},
                'result': 0,
                'text': 'IPv4 lease found.'
            },
            response)


class TestLease4Del(ResponseTestCase):

    def test_ok(self):
        send_command('lease4-wipe')
        send_command('lease4-add',
                     addr='172.28.5.12',
                     hwaddr='1a:1b:1c:1d:1e:1f',
                     hostname='foo.example.com')
        send_command('lease4-add',
                     addr='172.28.6.12',
                     hwaddr='2a:2b:2c:2d:2e:2f',
                     hostname='bar.example.com')
        response = send_command('lease4-del',
                                addr='172.28.5.12')
        self.assertResponse({
                'result': 0,
                'text': 'IPv4 lease deleted.'
            },
            response,
            'IPv4 lease deleted')
        response = send_command('lease4-del',
                                addr='172.28.5.12')
        self.assertResponse({
                'result': 3,
                'text': 'IPv4 lease not found.'
            },
            response,
            'IPv4 lease not found')


class TestLease4Update(ResponseTestCase):

    def test_ok(self):
        send_command('lease4-wipe')
        send_command('lease4-add',
                     addr='172.28.5.12',
                     hwaddr='11:11:11:11:11:11')
        response = send_command('lease4-update',
                                addr='172.28.5.12',
                                hwaddr='11:11:11:11:11:11',
                                hostname='foo.example.com')
        self.assertResponse({
                'result': 0,
                'text': 'IPv4 lease updated.'
            },
            response,
            'IPv4 lease updated')
        response = send_command('lease4-update',
                                addr='172.28.5.13',
                                hwaddr='12:12:12:12:12:12',
                                hostname='bar.example.com')
        self.assertResponse({
                'result': 1,
                'text': 'failed to update the lease with address 172.28.5.13 - no such lease'
            },
            response)


class TestLeaseWipe(ResponseTestCase):

    def test_wipe_0(self):
        send_command('lease4-wipe')
        response = send_command('lease4-wipe')
        self.assertResponse({
                "text": "Deleted 0 IPv4 lease(s) from subnet(s) 5 6",
                "result": 3
            },
            response,
            'deleted 0 IPv4 lease(s) from subnet(s) 5 6')

    def test_wipe_1_from_any(self):
        send_command('lease4-wipe')
        send_command('lease4-add',
                     addr='172.28.5.12',
                     hwaddr='1a:1b:1c:1d:1e:1f')
        response = send_command('lease4-wipe')
        self.assertResponse({
                "text": "Deleted 1 IPv4 lease(s) from subnet(s) 5 6",
                "result": 0
            },
            response,
            'deleted 1 IPv4 lease(s) from subnet(s) 5 6')

    def test_wipe_1_from_6(self):
        send_command('lease4-wipe')
        send_command('lease4-add',
                     addr='172.28.6.12',
                     hwaddr='1a:1b:1c:1d:1e:1f')
        response = send_command('lease4-wipe',
                                subnet_id=6)
        self.assertResponse({
                "text": "Deleted 1 IPv4 lease(s) from subnet(s) 6",
                "result": 0
            },
            response,
            'deleted 1 IPv4 lease(s) from subnet(s) 6')
