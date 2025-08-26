import argparse
import json
import socket


def send_command(name, args=None):
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.connect('/usr/local/var/run/kea/kea4.sock')
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


def remote_server4_get_all():
    send_command('remote-server4-get-all',
                 {'remote': {'type': 'mysql'}})


def remote_server4_set():
    send_command('remote-server4-set',
                 {'servers': [{'server-tag': 'kea',
                               'description': 'Default server.'}],
                  'remote': {'type': 'mysql'}})


def remote_subnet4_list():
    send_command('remote-subnet4-list',
                 {'remote': {'type': 'mysql'},
                  'server-tags': ['kea']
                 })


def remote_subnet4_del_by_id():
    send_command('remote-subnet4-del-by-id',
                 {'subnets': [{
                     'id': 5,
                  }],
                  'remote': {'type': 'mysql'}
                 })


def remote_subnet4_get_by_id():
    send_command('remote-subnet4-get-by-id',
                 {'subnets': [{
                     'id': 5,
                  }],
                  'remote': {'type': 'mysql'}
                 })


def remote_subnet4_set():
    send_command('remote-subnet4-set',
                 {'subnets': [{
                     'subnet': '172.28.5.0/24',
                     'id': 5,
                     'shared-network-name': None,
                     'pools': [{'pool': '172.28.5.100 - 172.28.5.200'}]
                  }],
                  'remote': {'type': 'mysql'},
                  'server-tags': ['kea']
                 })


def server_tag_get():
    send_command('server-tag-get')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--server-tag-get', action='store_true')
    parser.add_argument('--server-set', action='store_true')
    parser.add_argument('--server-get-all', action='store_true')
    parser.add_argument('--subnet-set', action='store_true')
    parser.add_argument('--subnet-del-by-id', action='store_true')
    parser.add_argument('--subnet-get-by-id', action='store_true')
    parser.add_argument('--subnet-list', action='store_true')

    args = parser.parse_args()
    do_all = True
    if args.server_tag_get:
        server_tag_get()
        do_all = False
    if args.subnet_set:
        remote_subnet4_set()
        do_all = False
    if args.subnet_del_by_id:
        remote_subnet4_del_by_id()
        do_all = False
    if args.subnet_get_by_id:
        remote_subnet4_get_by_id()
        do_all = False
    if args.subnet_list:
        remote_subnet4_list()
        do_all = False
    if args.server_set:
        remote_server4_set()
        do_all = False
    if args.server_get_all:
        remote_server4_get_all()
        do_all = False
    if do_all:
        remote_server4_set()
        remote_server4_get_all()
        remote_subnet4_set()
        remote_subnet4_list()
        remote_subnet4_get_by_id()
        remote_subnet4_del_by_id()
        remote_subnet4_list()


if __name__ == '__main__':
    main()
