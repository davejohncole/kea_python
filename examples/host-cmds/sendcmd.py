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


if __name__ == '__main__':
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

    # send_command('reservation-get', **{'subnet-id': 5, 'ip-address': '172.28.5.191'})

    # send_command('reservation-get', **{'subnet-id': 5,
    #                                    'identifier-type': 'hw-address',
    #                                    'identifier': '1a:1b:1c:1d:1e:1f'})

    # send_command('reservation-del', **{'subnet-id': 5, 'ip-address': '172.28.5.190'})

    # send_command('reservation-del', **{'subnet-id': 5,
    #                                    'identifier-type': 'hw-address',
    #                                    'identifier': '1a:1b:1c:1d:1e:1f'})

    send_command('reservation-get-all', **{'subnet-id': 5})
