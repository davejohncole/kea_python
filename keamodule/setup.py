import os
import re
from distutils.core import setup, Extension


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
