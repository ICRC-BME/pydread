#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from setuptools import setup, Extension
import numpy

# the c extension module
DREAD_EXT = Extension("pydread",
                      ["pydread/pydread.c"],
                      include_dirs=["pydread/dlib"])

setup(name = "pydread",
      version='0.0a1',
      description='Wrapper of C library for .d file reading',
      url='https://github.com/ICRC-BME/pydread',
      author='Jan Cimbalnik',
      author_email='jan.cimbalnik@fnusa.cz, jan.cimbalnik@mayo.edu',
      license='Apache 2.0',
      platforms=['Linux', 'MacOSX'],
      keywords='electrophysiology',
      install_requires=['numpy'],
      zip_safe=False,
      classifiers=['License :: OSI Approved :: Apache Software License',
                   'Operating System :: MacOS :: MacOS X',
                   'Operating System :: POSIX :: Linux',
                   'Operating System :: Microsoft :: Windows',
                   'Programming Language :: Python :: 3',
                   'Development Status :: 4 - Beta',
                   'Topic :: Scientific/Engineering'],
      packages = ["pydread"],
      ext_modules=[DREAD_EXT],
      include_dirs=[numpy.get_include()])
