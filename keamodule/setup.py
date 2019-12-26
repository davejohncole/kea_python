from distutils.core import setup, Extension


kea = Extension('kea',
                sources=['kea.cc',
                         'capsule.cc'],
                include_dirs=['/usr/local/include/kea'])

setup(name='kea',
      version='1.0',
      description='Extension module for Kea',
      ext_modules=[kea])
