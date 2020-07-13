import re
from distutils.core import setup, Extension


def calc_macros():
    config_h = open('/usr/local/include/kea/config.h').read()
    m = re.search(r'^#define VERSION "([^"]*)"\n', config_h, re.M)
    if not m:
        raise RuntimeError('could not determine kea version')
    version = m.group(1)
    macros = []
    if version >= '1.7.1':
        macros.append(('HAVE_GETLEASES4_HOSTNAME', None))
    return macros


kea = Extension('kea',
                define_macros=calc_macros(),
                sources=['kea.cc',
                         'host.cc',
                         'host_reservation_parser.cc',
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
                include_dirs=['/usr/local/include/kea'],
                libraries=['kea-exceptions',
                           'kea-log',
                           'kea-hooks',
                           'kea-dhcpsrv',
                           'ffi'],
                library_dirs=['/usr/local/lib'])

setup(name='kea',
      version='1.0',
      description='Extension module for Kea',
      ext_modules=[kea])
