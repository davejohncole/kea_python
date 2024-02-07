import json
import kea


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


def get_map_arg(args, name, default=UNSPECIFIED, error_msg=None):
    value = get_arg(args, name, default)
    if value != default and not isinstance(value, dict):
        if error_msg:
            raise CommandError(error_msg)
        raise CommandError("'%s' is not a map" % name)
    return value


def get_list_arg(args, name, default=UNSPECIFIED, error_msg=None):
    value = get_arg(args, name, default)
    if value != default and not isinstance(value, list):
        if error_msg:
            raise CommandError(error_msg)
        raise CommandError("'%s' is not a list" % name)
    return value


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


def debug_subnets():
    subnets = kea.CfgMgr().getCurrentCfg().getCfgSubnets4().getAll()
    if subnets:
        for s in kea.CfgMgr().getCurrentCfg().getCfgSubnets4().getAll():
            kea.logger.debug(json.dumps(s.toElement()))
    else:
        kea.logger.debug('no subnets')


# {"command": "remote-server4-get-all",
#  "arguments": {"remote": "mysql"}}
def remote_server4_get_all(handle):
    def get_response(args):
        backend = get_string_arg(args, 'remote')
        servers = kea.ConfigBackendDHCPv4Mgr.instance().getPool().getAllServers4(backend)
        return {'result': 0,
                'text': 'DHCPv4 servers found.',
                'arguments': {
                    'servers': servers,
                    'count': len(servers)
                }}

    return wrap_handler(handle, get_response)


# {"command": "remote-server4-set",
#  "arguments": {"servers": [{"server-tag": "kea",
#                             "description": "Default server"}],
#                "remote": "mysql"}}
def remote_server4_set(handle):
    def get_response(args):
        backend = get_string_arg(args, 'remote')
        servers = get_list_arg(args, 'servers')
        for desc in servers:
            server_tag = desc.get('server-tag', 'empty-server')
            description = desc.get('description', '')
            pool = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
            pool.createUpdateServer4(backend, server_tag, description)
        return {'result': 0,
                'text': 'DHCPv4 server successfully added.'}

    return wrap_handler(handle, get_response)


# {"command": "remote-subnet4-del-by-id",
#  "arguments": {"remote": "mysql",
#                "subnets": [{"id": 5}]}}
def remote_subnet4_del_by_id(handle):
    def get_response(args):
        backend = get_string_arg(args, 'remote')
        subnets = get_list_arg(args, 'subnets')
        pool = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        cfg_subnets = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        for subnet in subnets:
            subnet_id = subnet['id']
            pool.deleteSubnet4(backend, 'all', subnet_id)
            cfg_subnets.delSubnetID(subnet_id)
        return {'result': 0,
                'text': '%s IPv4 subnet(s) deleted.' % len(subnets)}

    return wrap_handler(handle, get_response)


# {"command": "remote-subnet4-list",
#  "arguments": {"remote": "mysql",
#                "server-tags": ["all"]}}
def remote_subnet4_list(handle):
    def get_response(args):
        backend = get_string_arg(args, 'remote')
        server = get_string_arg(args, 'server-tags')
        pool = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        subnets = pool.getAllSubnets4(backend, server)
        return {'result': 0,
                'text': '%s IPv4 subnet(s) found.' % len(subnets),
                'arguments': {
                    'subnets': [s.toElement() for s in subnets],
                    'count': len(subnets)
                }}

    return wrap_handler(handle, get_response)


# {"command": "remote-subnet4-set",
#  "arguments": {"subnets": [{...}],
#                "remote": "mysql",
#                "server-tags": ["all"]}}
def remote_subnet4_set(handle):
    def get_response(args):
        subnet = get_map_arg(args, 'subnet')
        backend = get_string_arg(args, 'remote')
        server = get_string_arg(args, 'server-tags')
        subnet4 = kea.Subnet4ConfigParser().parse(subnet)
        pool = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        pool.createUpdateSubnet4(subnet4, backend, server)
        cfg_subnets = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
        if cfg_subnets.getSubnet(subnet4.getID()):
            cfg_subnets.replace(subnet4)
        else:
            cfg_subnets.add(subnet4)
        return {'result': 0,
                'text': 'Subnet added.'}

    return wrap_handler(handle, get_response)


def dhcp4_srv_configured(handle):
    debug_subnets()
    mgr = kea.ConfigBackendDHCPv4Mgr.instance()
    mgr.addBackend('type=mysql host=mysql name=kea user=kea password=kea')
    pool = mgr.getPool()
    config = kea.CfgMgr().getCurrentCfg().getCfgSubnets4()
    for subnet in pool.getAllSubnets4('mysql', 'kea'):
        config.add(subnet)
    debug_subnets()
    return 0


def load(handle):
    handle.registerCommandCallout('remote-server4-get-all', remote_server4_get_all)
    handle.registerCommandCallout('remote-server4-set', remote_server4_set)
    handle.registerCommandCallout('remote-subnet4-del-by-id', remote_subnet4_del_by_id)
    handle.registerCommandCallout('remote-subnet4-list', remote_subnet4_list)
    handle.registerCommandCallout('remote-subnet4-set', remote_subnet4_set)
    return 0
