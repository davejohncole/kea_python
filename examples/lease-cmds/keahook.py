import kea


class UNSPECIFIED: pass


class CommandError(Exception):

    def __init__(self, reason):
        self.reason = reason


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
        kea.logger.exception('')
        handle.setArgument('response', {'result': 1,
                                        'text': str(e)})
        return 1
    return 0


def make_lease_response(lease):
    if lease:
        return {'result': 0,
                'text': 'IPv4 lease found.',
                'arguments': lease.toElement()}
    else:
        return {'result': 3,
                'text': 'Lease not found.'}


def make_lease_list_response(leases):
    return {'result': 0 if leases else 3,
            'text': '%d IPv4 lease(s) found.' % len(leases),
            'arguments': {'leases': [l.toElement() for l in leases],
                            'count': len(leases)}}


def get_lease4_kwargs(args):
    addr = get_string_arg(args, 'ip-address', None)
    if addr:
        return dict(addr=addr)

    subnet_id = get_int_arg(args, 'subnet-id')
    id_type = get_string_arg(args, 'identifier-type', None,
                             "No 'ip-address' provided"
                             " and 'identifier-type' is either missing or not a string.")
    if id_type not in ('hw-address', 'client-id'):
        # duid not supported for IPv4
        raise CommandError('Identifier type %s is not supported.' % id_type)
    ident = get_string_arg(args, 'identifier', None,
                           "No 'ip-address' provided"
                           " and 'identifier' is either missing or not a string.")

    if id_type == 'hw-address':
        return dict(hwaddr=ident, subnet_id=subnet_id)
    elif id_type == 'client-id':
        return dict(client_id=ident, subnet_id=subnet_id)


def lease4_add(handle):
    def get_response(args):
        return {'result': 0}

    return wrap_handler(handle, get_response)


def lease4_get(handle):
    def get_response(args):
        kwargs = get_lease_kwargs(args)
        lease = kea.LeaseMgr().getLease4(**kwargs)
        return make_lease_response(lease)

    return wrap_handler(handle, get_response)


def lease4_get_all(handle):
    def get_response(args):
        subnets = get_list_arg(args, 'subnets')
        lease_mgr = kea.LeaseMgr()
        leases = []
        for subnet_id in subnets:
            if not isinstance(subnet_id, int):
                raise CommandError("listed subnet identifiers must be numbers")
            leases.extend(lease_mgr.getLeases4(subnet_id=subnet_id))
        return make_lease_list_response(leases)

    return wrap_handler(handle, get_response)


def lease4_get_page(handle):
    def get_response(args):
        lower = get_string_arg(args, 'from')
        if lower == 'start':
            lower = '0.0.0.0'
        limit = get_int_arg(args, 'limit')
        leases = kea.LeaseMgr().getLeases4(lower_bound_address=lower, page_size=limit)
        return make_lease_list_response(leases)

    return wrap_handler(handle, get_response)


def lease4_get_by_hw_address(handle):
    def get_response(args):
        hwaddr = get_string_arg(args, 'hw-address')
        leases = kea.LeaseMgr().getLease4(hwaddr=hwaddr)
        return make_lease_list_response(leases)

    return wrap_handler(handle, get_response)


def lease4_get_by_client_id(handle):
    def get_response(args):
        client_id = get_string_arg(args, 'client-id')
        leases = kea.LeaseMgr().getLease4(client_id=client_id)
        return make_lease_list_response(leases)

    return wrap_handler(handle, get_response)


def lease4_get_by_hostname(handle):
    def get_response(args):
        hostname = get_string_arg('hostname').lower()
        if not hostname:
            raise CommandError("'hostname' parameter is empty")
        leases = kea.LeaseMgr().getLeases4(hostname=hostname)
        return make_lease_list_response(leases)

    return wrap_handler(handle, get_response)


def lease4_del(handle):
    def get_response(args):
        kwargs = get_lease_kwargs(args)
        lease_mgr = kea.LeaseMgr()
        addr = kwargs.get('addr')
        if addr is None:
            lease = lease_mgr.getLease4(**kwargs)
            if not lease:
                raise CommandError("IPv4 lease not found.")
            addr = lease.addr
        if lease_mgr.deleteLease(addr):
            return {'result': 0,
                    'text': 'IPv4 lease deleted.'}
        else:
            return {'result': 3,
                    'text': 'IPv4 lease not found.'}

    return wrap_handler(handle, get_response)


def lease4_update(handle):
    cmd = handle.getArgument('command')
    handle.setArgument('response', {'result': 0})
    return 0


def lease4_wipe(handle):
    def get_response(args):
        subnet_id = get_arg(args, 'subnet-id', None)
        if subnet_id is None:
            config = kea.CfgMgr().getCurrentCfg()
            subnet_ids = [s.getID() for s in config.getCfgSubnets4().getAll()]
        elif not isinstance(subnet_id, int):
            raise CommandError("'subnet-id' is not integer.")
        else:
            subnet_ids = [subnet_id]
        num = 0
        lease_mgr = kea.LeaseMgr()
        for s in subnet_ids:
            num += lease_mgr.wipeLeases4(s)
        msg = 'Deleted %d IPv4 lease(s) from subnet(s)' % num
        if num:
            msg += ' ' + ' '.join([str(s) for s in subnet_ids])
        return {'result': 0 if num else 3,
                'text': msg}

    return wrap_handler(handle, get_response)


def load(handle):
    handle.registerCommandCallout('lease4-add', lease4_add)
    handle.registerCommandCallout('lease4-get', lease4_get)
    handle.registerCommandCallout('lease4-get-all', lease4_get_all)
    handle.registerCommandCallout('lease4-get-page', lease4_get_page)
    handle.registerCommandCallout('lease4-get-by-hw-address', lease4_get_by_hw_address)
    handle.registerCommandCallout('lease4-get-by-client-id', lease4_get_by_client_id)
    if kea.__version__ >= '1.7.1':
        handle.registerCommandCallout('lease4-get-by-hostname', lease4_get_by_hostname)
    handle.registerCommandCallout('lease4-del', lease4_del)
    handle.registerCommandCallout('lease4-update', lease4_update)
    handle.registerCommandCallout('lease4-wipe', lease4_wipe)
    return 0
