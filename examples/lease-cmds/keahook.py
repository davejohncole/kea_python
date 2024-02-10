import kea
import time


class UNSPECIFIED:
    pass


class CommandError(Exception):

    def __init__(self, reason):
        self.reason = reason


def get_arg(args, name, default=UNSPECIFIED, error_msg=None):
    if args is None or name not in args:
        if default is not UNSPECIFIED:
            return default
        if error_msg:
            raise CommandError(error_msg)
        raise CommandError("'%s' parameter not specified" % name)
    return args[name]


def get_string_arg(args, name, default=UNSPECIFIED, error_msg=None):
    value = get_arg(args, name, default, error_msg)
    if value != default and not isinstance(value, str):
        if error_msg:
            raise CommandError(error_msg)
        raise CommandError("'%s' is not a string" % name)
    return value


def get_int_arg(args, name, default=UNSPECIFIED, error_msg=None):
    value = get_arg(args, name, default)
    if value != default and not isinstance(value, int):
        if error_msg:
            raise CommandError(error_msg)
        raise CommandError("'%s' is not an integer" % name)
    return value


def get_bool_arg(args, name, default=UNSPECIFIED, error_msg=None):
    value = get_arg(args, name, default)
    if value != default and not isinstance(value, bool):
        if error_msg:
            raise CommandError(error_msg)
        raise CommandError("'%s' is not a bool" % name)
    return value


def get_list_arg(args, name, default=UNSPECIFIED, error_msg=None):
    value = get_arg(args, name, default)
    if value != default and not isinstance(value, list):
        if error_msg:
            raise CommandError(error_msg)
        raise CommandError("'%s' is not a list" % name)
    return value


def get_map_arg(args, name, default=UNSPECIFIED, error_msg=None):
    value = get_arg(args, name, default)
    if value != default and not isinstance(value, dict):
        if error_msg:
            raise CommandError(error_msg)
        raise CommandError("'%s' is not a map" % name)
    return value


def parse_lease4(args):
    lease = kea.Lease4()
    if not isinstance(args, dict):
        raise CommandError("parameters missing or is not a map")
    lease.addr = get_string_arg(args, 'ip-address')
    lease.hwaddr = get_string_arg(args, 'hw-address')
    subnet_id = get_int_arg(args, 'subnet-id', 0)
    cfg = kea.CfgMgr.instance().getCurrentCfg()
    if subnet_id:
        subnet = cfg.getCfgSubnets4().getSubnet(subnet_id)
        if not subnet:
            raise CommandError('no IPv4 subnet with subnet-id=%d currently configured' % subnet_id)
        if not subnet.inRange(lease.addr):
            raise CommandError('address %s does not belong to subnet %s, subnet-id=%s'
                               % (lease.addr, subnet.toText(), subnet_id))
    else:
        subnet = cfg.getCfgSubnets4().selectSubnet(lease.addr)
        if not subnet:
            raise CommandError('subnet-id not specified and failed to find a subnet for address %s'
                               % lease.addr)
        subnet_id = subnet.getID()
    lease.subnet_id = subnet_id
    client_id = get_string_arg(args, 'client-id', None)
    if client_id is not None:
        lease.client_id = client_id
    valid_lft = get_int_arg(args, 'valid-lft', None)
    if valid_lft is None:
        valid_lft = subnet.getValid()
    lease.valid_lft = valid_lft
    expire = get_int_arg(args, 'expire', None)
    if expire is not None:
        if expire <= 0:
            raise CommandError('expiration time must be positive for address %s' % lease.addr)
        if expire < valid_lft:
            raise CommandError('expiration time must be greater than valid lifetime for address %s'
                               % lease.addr)
        cltt = expire - valid_lft
    else:
        cltt = int(time.time())
    lease.cltt = cltt
    lease.fqdn_fwd = get_bool_arg(args, 'fqdn-fwd', False)
    lease.fqdn_rev = get_bool_arg(args, 'fqdn-rev', False)
    lease.hostname = get_string_arg(args, 'hostname', None)
    if lease.hostname and (lease.fqdn_fwd or lease.fqdn_rev):
        raise CommandError('no hostname specified and either forward or reverse'
                           ' fqdn was set to true')
    lease.state = get_int_arg(args, 'state', 0)
    if lease.state < 0 or lease.state > kea.STATE_EXPIRED_RECLAIMED:
        raise CommandError('invalid state %s: must be 0 (default), 1 (declined),'
                           ' or 2 (expired-reclaimed)' % lease.state)
    ctx = get_map_arg(args, 'user-context', {})
    comment = get_string_arg(args, 'comment', None)
    if comment is not None:
        if 'comment' in ctx:
            raise CommandError("duplicated comment entry '%s' in user context '%s'"
                               % (comment, ctx))
        ctx['comment'] = comment
    if ctx:
        lease.setContext(ctx)
    return lease


def make_lease_response(lease):
    if lease:
        return {'result': 0,
                'text': 'IPv4 lease found',
                'arguments': lease.toElement()}
    else:
        return {'result': 3,
                'text': 'lease not found'}


def make_lease_list_response(leases, include_count=True):
    args = {'leases': [l.toElement() for l in leases]}
    if include_count:
        args['count'] = len(leases)
    return {'result': 0 if leases else 3,
            'text': '%d IPv4 lease(s) found' % len(leases),
            'arguments': args}


def get_lease4_kwargs(args):
    addr = get_string_arg(args, 'ip-address', None)
    if addr:
        return dict(addr=addr)

    subnet_id = get_int_arg(args, 'subnet-id')
    id_type = get_string_arg(args, 'identifier-type',
                             error_msg=("no 'ip-address' provided and 'identifier-type'"
                                        " is either missing or not a string"))
    if id_type not in ('hw-address', 'client-id'):
        # duid not supported for IPv4
        raise CommandError("'identifier-type' %s is not supported, the only supported"
                           " values are: address, hw-address, client-id" % id_type)
    ident = get_string_arg(args, 'identifier', None,
                           "no 'ip-address' provided"
                           " and 'identifier' is either missing or not a string")

    if id_type == 'hw-address':
        return dict(hwaddr=ident, subnet_id=subnet_id)
    elif id_type == 'client-id':
        return dict(client_id=ident, subnet_id=subnet_id)


def wrap_handler(handle, get_response):
    try:
        cmd = handle.getArgument('command')
        args = cmd.get('arguments')
        if args is not None and not isinstance(args, dict):
            raise CommandError('parameters missing or is not a map')
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


def lease4_add(handle):
    def get_response(args):
        lease = parse_lease4(args)
        kea.LeaseMgr().addLease(lease)
        return {'result': 0,
                'text': 'lease for address %s, subnet-id %s added' % (lease.addr, lease.subnet_id)}

    return wrap_handler(handle, get_response)


def lease4_get(handle):
    def get_response(args):
        kwargs = get_lease4_kwargs(args)
        lease = kea.LeaseMgr().getLease4(**kwargs)
        return make_lease_response(lease)

    return wrap_handler(handle, get_response)


def lease4_get_all(handle):
    def get_response(args):
        lease_mgr = kea.LeaseMgr()
        if args:
            subnets = get_list_arg(args, 'subnets')
            leases = []
            for subnet_id in subnets:
                if not isinstance(subnet_id, int):
                    raise CommandError("listed subnet identifiers must be numbers")
                leases.extend(lease_mgr.getLeases4(subnet_id=subnet_id))
        else:
            leases = lease_mgr.getLeases4()
        return make_lease_list_response(leases, include_count=False)

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
        lease_mgr = kea.LeaseMgr()
        kwargs = get_lease4_kwargs(args)
        lease = lease_mgr.getLease4(**kwargs)
        if not lease:
            raise CommandError("IPv4 lease not found")
        if kea.__version__ >= '1.7.3':
            arg = lease
        else:
            arg = lease.addr
        if lease_mgr.deleteLease(arg):
            return {'result': 0,
                    'text': 'IPv4 lease deleted'}
        else:
            return {'result': 3,
                    'text': 'IPv4 lease not found'}

    return wrap_handler(handle, get_response)


def lease4_update(handle):
    def get_response(args):
        lease = parse_lease4(args)
        force_create = get_bool_arg(args, 'force-create', False)
        lease_mgr = kea.LeaseMgr()
        if force_create and lease_mgr.getLease4(addr=lease.addr) is None:
            lease_mgr.addLease(lease)
            return {'result': 0,
                    'text': 'IPv4 lease added'}
        lease_mgr.updateLease4(lease)
        return {'result': 0,
                'text': 'IPv4 lease updated'}

    return wrap_handler(handle, get_response)


def lease4_wipe(handle):
    def get_response(args):
        subnet_id = get_arg(args, 'subnet-id', None)
        if subnet_id is None:
            config = kea.CfgMgr.instance().getCurrentCfg()
            subnet_ids = [s.getID() for s in config.getCfgSubnets4().getAll()]
        elif not isinstance(subnet_id, int):
            raise CommandError("'subnet-id' is not integer")
        else:
            subnet_ids = [subnet_id]
        num = 0
        lease_mgr = kea.LeaseMgr()
        for s in subnet_ids:
            num += lease_mgr.wipeLeases4(s)
        msg = ('deleted %d IPv4 lease(s) from subnet(s) %s'
               % (num, ' '.join([str(s) for s in subnet_ids])))
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
