import os
import re
from distutils.core import setup, Extension


def find_kea_include():
    for path in ['/usr/local/include/kea',
                 '/usr/include/kea']:
        if os.path.isdir(path):
            return path
    raise RuntimeError('cannot locate kea include directory')


def find_kea_libs():
    for path in ['/usr/local/lib',
                 '/usr/lib64']:
        if os.path.exists(os.path.join(path, 'libkea-hooks.so')):
            return path
    raise RuntimeError('cannot locate kea library directory')


kea_include = find_kea_include()
kea_libs = find_kea_libs()


def calc_macros():
    config_h = open(os.path.join(kea_include, 'config.h')).read()
    m = re.search(r'^#define VERSION "([^"]*)"\n', config_h, re.M)
    if not m:
        raise RuntimeError('could not determine kea version')
    version = tuple(int(v) for v in m.group(1).split('.'))
    macros = []
    if version >= (1, 7, 1):
        macros.append(('HAVE_GETLEASES4_HOSTNAME', None))
    if version >= (1, 7, 4):
        macros.append(('HAVE_DELETELEASE_LEASE', None))
    return macros


kea = Extension('kea',
                define_macros=calc_macros(),
                sources=['kea.cc',
                         'host.cc',
                         'host_reservation_parser4.cc',
                         'host_mgr.cc',
                         'cfg_mgr.cc',
                         'cfg_subnets4.cc',
                         'lease_mgr.cc',
                         'errors.cc',
                         'capsule.cc',
                         'constants.cc',
                         'utils.cc',
                         'callouts.cc',
                         'callout_closure.cc',
                         'callout_manager.cc',
                         'library_handle.cc',
                         'callout_handle.cc',
                         'lease4.cc',
                         'pkt4.cc',
                         'subnet4.cc',
                         'option.cc',
                         'srv_config.cc'],
                include_dirs=[kea_include],
                libraries=['kea-exceptions',
                           'kea-log',
                           'kea-hooks',
                           'kea-dhcpsrv',
                           'ffi'],
                library_dirs=[kea_libs],
                extra_compile_args=['-std=c++11'])

setup(name='kea',
      version='1.0',
      description='Extension module for Kea',
      ext_modules=[kea])
