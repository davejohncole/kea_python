from distutils.core import setup, Extension


kea = Extension('kea',
                sources=['kea.cc'])

setup(name='kea',
      version='1.0',
      description='Extension module for Kea',
      ext_modules=[kea])
