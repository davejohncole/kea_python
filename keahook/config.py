import os
import sys


def find_pyinc():
    path = os.path.join(sys.prefix, 'include/python%d.%dm' % sys.version_info[:2])
    if os.path.isdir(path):
        return path
    raise RuntimeError('cannot find python include')


def find_keainc():
    for path in ['/usr/local/include/kea',
                 '/usr/include/kea']:
        if os.path.isdir(path):
            return path
    raise RuntimeError('cannot find kea include')


def find_keahooks():
    for path in ['/usr/local/lib/kea/hooks',
                 '/usr/lib64/kea/hooks']:
        if os.path.isdir(path):
            return path
    raise RuntimeError('cannot find kea hooks')


if os.path.exists('config.txt'):
    config = {}
    for line in open('config.txt'):
        if '=' in line:
            name, value = line.split('=')
            config[name.strip()] = value.strip()
else:
    config = dict(pyinc=find_pyinc(),
                  keainc=find_keainc(),
                  keahooks=find_keahooks())
    open('config.txt', 'w').write('''\
pyinc = {pyinc}
keainc = {keainc}
keahooks = {keahooks}
'''.format(**config))


print(config[sys.argv[1]])
