"""
Implementation of https://kea.readthedocs.io/en/kea-2.2.0/arm/hooks.html#cb-cmds-configuration-backend-commands

The behaviour is not likely to be identical to the official hook since it has been developed
by reading the hook documentation, and guessing how the hook would implement the function.
"""
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


def make_selector(server_tags):
    if isinstance(server_tags, list) and len(server_tags) == 1:
        return server_tags[0]
    return server_tags


# {
#     "command": "remote-server4-del",
#     "arguments": {
#         "servers": [{
#             "server-tag": "server1"
#         }],
#         "remote": {
#             "type": "mysql"
#         }
#     } 
# }
def remote_server4_del(handle):
    def get_response(args):
        remote = get_map_arg(args, 'remote')
        backend = remote['type']
        pool = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        count = 0
        for server in get_list_arg(args, 'servers'):
            server_tag = server['server_tag']
            count += pool.deleteServer4(backend, server_tag)
        return {'result': 0,
                'text': '%s DHCPv4 server(s) deleted.' % count,
                'arguments': {
                    'count': count
                }}

    return wrap_handler(handle, get_response)


# {
#     "command": "remote-server4-get-all",
#     "arguments": {
#         "remote": {
#             "type": "mysql"
#         }
#     }
# }
def remote_server4_get_all(handle):
    def get_response(args):
        remote = get_map_arg(args, 'remote')
        backend = remote['type']
        pool = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        servers = pool.getAllServers4(backend)
        return {'result': 0,
                'text': 'DHCPv4 servers found.',
                'arguments': {
                    'servers': [s.toElement() for s in servers],
                    'count': len(servers)
                }}

    return wrap_handler(handle, get_response)


# {
#     "command": "remote-server4-set",
#     "arguments": {
#         "servers": [{
#             "server-tag": "kea",
#             "description": "Default server"
#         }],
#         "remote": {
#             "type": "mysql"
#         }
#     }
# }
def remote_server4_set(handle):
    def get_response(args):
        remote = get_map_arg(args, 'remote')
        backend = remote['type']
        servers = get_list_arg(args, 'servers')
        pool = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        for desc in servers:
            server_tag = desc.get('server-tag', 'empty-server')
            description = desc.get('description', '')
            pool.createUpdateServer4(backend, kea.Server(server_tag, description))
        return {'result': 0,
                'text': 'DHCPv4 server successfully set.',
                'arguments': {
                    'servers': servers
                }}

    return wrap_handler(handle, get_response)


# NOTE: servers is not in the documented official hook, but I cannot get it to work without supplying
#       that argument
# {
#     "command": "remote-subnet4-del-by-id",
#     "arguments": {
#         "servers": [{
#             "server-tag": "server1"
#         }],
#         "subnets": [{
#             "id": 5
#         }],
#         "remote": {
#             "type": "mysql"
#         }
#     }
# }
def remote_subnet4_del_by_id(handle):
    def get_response(args):
        remote = get_map_arg(args, 'remote')
        backend = remote['type']
        server_tags = get_list_arg(args, 'server-tags')
        subnets = get_list_arg(args, 'subnets')
        pool = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        count = 0
        for subnet in subnets:
            subnet_id = subnet['id']
            count += pool.deleteSubnet4(backend, make_selector(server_tags), subnet_id)
        return {'result': 0,
                'text': '%s IPv4 subnet(s) deleted.' % count}

    return wrap_handler(handle, get_response)


# {
#     "command": "remote-subnet4-list",
#     "arguments": {
#         "server-tags": ["all"],
#         "remote": {
#             "type": "mysql"
#         }
#     }
# }
def remote_subnet4_list(handle):
    def get_response(args):
        remote = get_map_arg(args, 'remote')
        backend = remote['type']
        server_tags = get_list_arg(args, 'server-tags')
        pool = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        subnets = pool.getAllSubnets4(backend, make_selector(server_tags))
        return {'result': 0,
                'text': '%s IPv4 subnet(s) found.' % len(subnets),
                'arguments': {
                    'subnets': [s.toElement() for s in subnets],
                    'count': len(subnets)
                }}

    return wrap_handler(handle, get_response)


# {
#     "command": "remote-subnet4-set",
#     "arguments": {
#         "subnets": [{...}],
#         "server-tags": ["all"],
#         "remote": {
#             "type": "mysql"
#         }
#     }
# }
def remote_subnet4_set(handle):
    def get_response(args):
        remote = get_map_arg(args, 'remote')
        backend = remote['type']
        server_tags = get_list_arg(args, 'server-tags')
        parser = kea.Subnet4ConfigParser()
        subnets = [parser.parse(s) for s in get_list_arg(args, 'subnet')]
        pool = kea.ConfigBackendDHCPv4Mgr.instance().getPool()
        for subnet in subnets:
            pool.createUpdateSubnet4(backend, make_selector(server_tags), subnet)
        return {'result': 0,
                'text': 'Subnet(s) added.'}

    return wrap_handler(handle, get_response)


def load(handle):
    handle.registerCommandCallout('remote-server4-get-all', remote_server4_get_all)
    handle.registerCommandCallout('remote-server4-set', remote_server4_set)
    handle.registerCommandCallout('remote-subnet4-del-by-id', remote_subnet4_del_by_id)
    handle.registerCommandCallout('remote-subnet4-list', remote_subnet4_list)
    handle.registerCommandCallout('remote-subnet4-set', remote_subnet4_set)
    return 0
