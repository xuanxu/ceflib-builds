#! /usr/bin/env python

from setuptools import setup
from setuptools import Extension
import numpy as np

from get_version import get_version

VERSION = get_version()

ceflib_dir = "../LIB"
cislib_dir = "../C"

module = Extension ('ceflib',
	sources = [ "src/ceflib.c" ],
	include_dirs = [ 
		"%s/inc" % cislib_dir,
		"%s/inc" % ceflib_dir,
		np.get_include(),
	],
	library_dirs = [ 
		"%s/bin" % cislib_dir,
		"%s/bin" % ceflib_dir,
	],
	libraries = [
		"CEF_gcc", 
		"CIS_gcc", 
	"z"]
)
	
setup (	name = "ceflib",
	version = VERSION,
	description = 'Python CEFLIB interface',
	author = "Alain BARTHE",
	author_email = "alain.barthe@irap.omp.eu",
	long_description = '''Python interface to the Cluster Exchange Format (CEF) files

This module will allow to read CEF files
''',
	ext_modules = [module]
)
