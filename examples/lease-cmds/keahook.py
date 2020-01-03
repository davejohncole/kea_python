from kea import *


class UNSPECIFIED: pass


class CommandError(Exception):

    def __init__(self, reason):
        self.reason = reason


def wrap_handler(handle, get_response):
    try:
        cmd = handle.getArgument('command')
        args = cmd.get('arguments')
        if not isinstance(args, dict):
            raise CommandError('Parameters missing or are not a map.')
        handle.setArgument('response', get_response(args))
    except CommandError as e:
        handle.setArgument('response', {'result': 1,
                                        'text': e.reason})
        return 1
    except Exception as e:
        logger.exception('')
        handle.setArgument('response', {'result': 1,
                                        'text': str(e)})
        return 1
    return 0


def get_arg(args, name, default=UNSPECIFIED):
    if name not in args:
        if default is not UNSPECIFIED:
            return default
        raise CommandError("'%s' parameter not specified" % name)
    return args[name]


def get_string_arg(args, name, default=UNSPECIFIED, error_msg=None):
    value = get_arg(args, name, default)
    if not isinstance(value, str):
        if error_msg:
            raise CommandError(error_msg)
        raise CommandError("'%s' is not a string." % name)
    return value


def get_int_arg(args, name, default=UNSPECIFIED, error_msg=None):
    value = get_arg(args, name, default)
    if not isinstance(value, int):
        if error_msg:
            raise CommandError(error_msg)
        raise CommandError("'%s' is not integer." % name)
    return value


def get_list_arg(args, name, default=UNSPECIFIED, error_msg=None):
    value = get_arg(args, name, default)
    if not isinstance(value, list):
        if error_msg:
            raise CommandError(error_msg)
        raise CommandError("'%s' is not list." % name)
    return value


def lease4_add(handle):
    cmd = handle.getArgument('command')
    handle.setArgument('response', {'result': 0})
    return 0


def lease4_get(handle):
    def make_response(lease4):
        if lease4:
            return {'result': 0,
                    'text': 'IPv4 lease found.',
                    'arguments': lease4.toElement()}
        else:
            return {'result': 3,
                    'text': 'Lease not found.'}

    def get_response(args):
        addr = get_string_arg(args, 'ip-address', None)
        if addr:
            return make_response(LeaseMgr().getLease4(addr=addr))

        subnet_id = get_int_arg(args, 'subnet-id')
        id_type = get_string_arg(args, 'identifier-type', None,
                                 "No 'ip-address' provided"
                                 " and 'identifier-type' is either missing or not a string.")
        if id_type not in ('address', 'hw-address', 'client-id'):
            # duid not supported for IPv4
            raise CommandError('Identifier type %s is not supported.' % id_type)
        ident = get_string_arg(args, 'identifier', None,
                               "No 'ip-address' provided"
                               " and 'identifier' is either missing or not a string.")

        if id_type == 'hw-address':
            return make_response(LeaseMgr().getLease4(hwaddr=ident, subnet_id=subnet_id))
        elif id_type == 'client-id':
            return make_response(LeaseMgr().getLease4(client_id=ident, subnet_id=subnet_id))

    return wrap_handler(handle, get_response)


def lease4_get_all(handle):
    def get_response(args):
        subnets = get_list_arg(args, 'subnets')

        lease_mgr = LeaseMgr()
        leases = []
        for subnet_id in subnets:
            if not isinstance(subnet_id, int):
                raise CommandError("listed subnet identifiers must be numbers")
            leases.extend(lease_mgr.getLeases4(subnet_id=subnet_id))
        return {'result': 0,
                'text': '%d IPv4 lease(s) found.' % len(leases),
                'arguments': {'leases': [l.toElement() for l in leases]}}

    return wrap_handler(handle, get_response)


def lease4_get_page(handle):
    def get_response(args):
        lower = get_string_arg(args, 'from')
        if lower == 'start':
            lower = '0.0.0.0'
        limit = get_int_arg(args, 'limit')

        lease_mgr = LeaseMgr()
        leases = lease_mgr.getLeases4(lower_bound_address=lower, page_size=limit)
        return {'result': 0 if leases else 3,
                'text': '%d IPv4 lease(s) found.' % len(leases),
                'arguments': {'leases': [l.toElement() for l in leases],
                              'count': len(leases)}}

    return wrap_handler(handle, get_response)


def lease4_get_by_hw_address(handle):
    cmd = handle.getArgument('command')
    handle.setArgument('response', {'result': 0})
    return 0


def lease4_get_by_client_id(handle):
    cmd = handle.getArgument('command')
    handle.setArgument('response', {'result': 0})
    return 0


def lease4_get_by_hostname(handle):
    cmd = handle.getArgument('command')
    handle.setArgument('response', {'result': 0})
    return 0


def lease4_del(handle):
    cmd = handle.getArgument('command')
    handle.setArgument('response', {'result': 0})
    return 0


def lease4_update(handle):
    cmd = handle.getArgument('command')
    handle.setArgument('response', {'result': 0})
    return 0


def lease4_wipe(handle):
    cmd = handle.getArgument('command')
    handle.setArgument('response', {'result': 0})
    return 0


def load(handle):
    handle.registerCommandCallout('lease4-add', lease4_add)
    handle.registerCommandCallout('lease4-get', lease4_get)
    handle.registerCommandCallout('lease4-get-all', lease4_get_all)
    handle.registerCommandCallout('lease4-get-page', lease4_get_page)
    handle.registerCommandCallout('lease4-get-by-hw-address', lease4_get_by_hw_address)
    handle.registerCommandCallout('lease4-get-by-client-id', lease4_get_by_client_id)
    handle.registerCommandCallout('lease4-get-by-hostname', lease4_get_by_hostname)
    handle.registerCommandCallout('lease4-del', lease4_del)
    handle.registerCommandCallout('lease4-update', lease4_update)
    handle.registerCommandCallout('lease4-wipe', lease4_wipe)
    return 0
