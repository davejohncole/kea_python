import os
import re
from setuptools import setup, Extension


settings = {}
for line in open('../settings.mk'):
    if '=' in line:
        name, value = line.split('=')
        settings[name.strip()] = value.strip()


def calc_macros():
    config_h = open(os.path.join(settings['KEA_INC'], 'config.h')).read()
    m = re.search(r'^#define VERSION "([^"]*)"\n', config_h, re.M)
    if not m:
        raise RuntimeError('could not determine kea version')
    ver = re.sub('-git', '', m.group(1))
    version = tuple(int(v) for v in ver.split('.'))
    macros = []
    if version < (1, 7, 1):
        macros.append(('MISSING_GETLEASES4_HOSTNAME', None))
    if version < (1, 7, 4):
        macros.append(('HAVE_DELETELEASE_ADDR', None))
    if version < (1, 7, 5):
        macros.append(('HAVE_LIBRARYHANDLE_MANAGER_PTR', None))
    if version < (1, 9, 4):
        macros.append(('HAVE_GETLEASE4_METHOD', None))
    if version < (2, 2, 0):
        macros.append(('HAVE_BACKEND_PGSQL', None))
    return macros


kea = Extension('kea',
                define_macros=calc_macros(),
                sources=['callout_closure.cc',
                         'callout_handle.cc',
                         'callout_manager.cc',
                         'callouts.cc',
                         'capsule.cc',
                         'cfg_mgr.cc',
                         'cfg_subnets4.cc',
                         'config_backend_dhcp4_mgr.cc',
                         'config_backend_pool_dhcp4.cc',
                         'constants.cc',
                         'errors.cc',
                         'host.cc',
                         'host_mgr.cc',
                         'host_reservation_parser4.cc',
                         'kea.cc',
                         'lease4.cc',
                         'lease_mgr.cc',
                         'library_handle.cc',
                         'logger_manager.cc',
                         'option.cc',
                         'pkt4.cc',
                         'server.cc',
                         'srv_config.cc',
                         'subnet4.cc',
                         'subnet4_config_parser.cc',
                         'utils.cc'],
                include_dirs=[settings['KEA_INC']],
                libraries=['kea-exceptions',
                           'kea-log',
                           'kea-hooks',
                           'kea-dhcpsrv',
                           'ffi'],
                library_dirs=[settings['KEA_LIBS']],
                extra_compile_args=['-std=c++11'])

setup(name='kea',
      version='1.0',
      description='Extension module for Kea',
      ext_modules=[kea])
