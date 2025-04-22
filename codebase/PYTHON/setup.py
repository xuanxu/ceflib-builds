#! /usr/bin/env python

VERSION="1.8.4"

from setuptools import setup
from setuptools import Extension
from os import getenv
import numpy as np

ceflib_dir = getenv ("CEFLIB_DIR")
cislib_dir = getenv ("CISLIB_DIR")

module = Extension ('ceflib',
	sources = [ "src/ceflib.c" ],
	include_dirs = [ 
		"%s/inc" % cislib_dir,
		"%s/inc" % ceflib_dir,
		np.get_include(),
		# "%s/core/include" % numpy_dir,
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
