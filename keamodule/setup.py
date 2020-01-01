from distutils.core import setup, Extension


kea = Extension('kea',
                sources=['kea.cc',
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
