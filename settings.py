import os
import sys


def find_pyinc():
    path = os.path.join(sys.prefix, 'include/python%d.%d' % sys.version_info[:2])
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


def find_kealibs():
    for path in ['/usr/local/lib',
                 '/usr/lib64']:
        if os.path.exists(os.path.join(path, 'libkea-hooks.so')):
            return path
    raise RuntimeError('cannot locate kea library directory')


with open('settings.mk', 'w') as fp:
    fp.write('PYTHON_INC = %s\n' % find_pyinc())
    fp.write('KEA_INC = %s\n' % find_keainc())
    fp.write('KEA_HOOKS = %s\n' % find_keahooks())
    fp.write('KEA_LIBS = %s\n' % find_kealibs())
