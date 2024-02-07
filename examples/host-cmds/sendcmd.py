import argparse
import json
import socket


def send_command(name, **args):
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


num_resv = 10


def add_reservations():
    for i in range(num_resv):
        send_command('reservation-add',
                     reservation={
                         'subnet-id': 5,
                         'hw-address': '1a:1b:1c:1d:1e:%02x' % i,
                         'ip-address': '172.28.5.%d' % (i + 1)})


def get_reservations():
    for i in range(num_resv):
        send_command('reservation-get', **{'subnet-id': 5, 'ip-address': '172.28.5.%d' % (i + 1)})


def list_reservations():
    res = send_command('reservation-get-page', **{'subnet-id': 5,
                                                  'limit': 5})
    while res['result'] == 0:
        next = res['arguments']['next']
        lower_host_id = next['from']
        source_index = next['source-index']
        res = send_command('reservation-get-page', **{'subnet-id': 5,
                                                      'limit': 5,
                                                      'from': lower_host_id,
                                                      'source-index': source_index})


def delete_reservations():
    for i in range(num_resv):
        send_command('reservation-del',
                     **{'subnet-id': 5,
                        'ip-address': '172.28.5.%d' % (i + 1)})


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--add', action='store_true')
    parser.add_argument('--get', action='store_true')
    parser.add_argument('--list', action='store_true')
    parser.add_argument('--delete', action='store_true')

    args = parser.parse_args()
    if args.add:
        add_reservations()
    if args.get:
        get_reservations()
    if args.list:
        list_reservations()
    if args.delete:
        delete_reservations()
    if not args.add and not args.get and not args.list and not args.delete:
        add_reservations()
        get_reservations()
        list_reservations()
        delete_reservations()



if __name__ == '__main__':
    main()
    # send_command('reservation-add',
    #              reservation={
    #                  'subnet-id': 5,
    #                  'hw-address': '1a:1b:1c:1d:1e:1f',
    #                  'ip-address': '172.28.5.190',
    #                  'next-server': '192.0.2.1',
    #                  'server-hostname': 'hal9000',
    #                  'boot-file-name': '/dev/null',
    #                  'option-data': [{
    #                      'name': 'domain-name-servers',
    #                      'data': '10.1.1.202,10.1.1.203'
    #                  }],
    #                  'client-classes': ['special_snowflake', 'office']
    #              })

    # send_command('reservation-get', **{'subnet-id': 5, 'ip-address': '172.28.5.190'})

    # send_command('reservation-get', **{'subnet-id': 5, 'ip-address': '172.28.5.191'})

    # send_command('reservation-get', **{'subnet-id': 5,
    #                                    'identifier-type': 'hw-address',
    #                                    'identifier': '1a:1b:1c:1d:1e:1f'})

    # send_command('reservation-del', **{'subnet-id': 5, 'ip-address': '172.28.5.190'})

    # send_command('reservation-del', **{'subnet-id': 5,
    #                                    'identifier-type': 'hw-address',
    #                                    'identifier': '1a:1b:1c:1d:1e:1f'})

    # send_command('reservation-get-all', **{'subnet-id': 5})
