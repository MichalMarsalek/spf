from distutils.core import setup, Extension
setup(name = 'spf', version = '1.0',  \
   ext_modules = [Extension('spf', ['spf.c'])])