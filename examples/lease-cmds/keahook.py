from kea import *


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
        lease_mgr = LeaseMgr()
        if 'ip-address' in args:
            addr = args['ip-address']
            if not isinstance(addr, str):
                raise CommandError("'ip-address' is not a string.")
            return make_response(lease_mgr.getLease4(addr=addr))
            
        if 'subnet-id' not in args:
            raise CommandError("Mandatory 'subnet-id' parameter missing.")
        subnet_id = args['subnet-id']
        if not isinstance(subnet_id, int):
            raise CommandError("'subnet-id' parameter is not integer.")

        id_type = args.get('identifier-type')
        if not isinstance(id_type, str):
            raise CommandError("No 'ip-address' provided"
                               " and 'identifier-type' is either missing or not a string.")
        if id_type not in ('address', 'hw-address', 'client-id'):
            # duid not supported for IPv4
            raise CommandError('Identifier type %s is not supported.' % id_type)
        ident = args.get('identifier')
        if not isinstance(ident, str):
            raise CommandError("No 'ip-address' provided"
                               " and 'identifier' is either missing or not a string.")

        if id_type == 'hw-address':
            return make_response(lease_mgr.getLease4(hwaddr=ident, subnet_id=subnet_id))
        elif id_type == 'client-id':
            return make_response(lease_mgr.getLease4(client_id=ident, subnet_id=subnet_id))

    return wrap_handler(handle, get_response)


def lease4_get_all(handle):
    cmd = handle.getArgument('command')
    handle.setArgument('response', {'result': 0})
    return 0


def lease4_get_page(handle):
    cmd = handle.getArgument('command')
    handle.setArgument('response', {'result': 0})
    return 0


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
