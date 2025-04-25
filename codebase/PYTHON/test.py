"""Test celib installation
"""
from	__future__ import print_function
from	ceflib import *
import	os
import	glob

test_dir = "../DATA"

verbosity(0)

os.putenv ("CEFPATH", "/net/rosina1/MSA2060a.CAA2/CAA/INCLUDES")

filenames = glob.glob ("%s/*.cef.gz" % test_dir)

for filename in filenames:

	print ("Reading", os.path.basename(filename))

	read (filename)

	for var in varnames():

		if vattr (var, "DEPEND_0") or var.startswith ("time"):
			print (">>", var, ":", vattr (var, "CATDESC"), ":", vattr (var, "VALUE_TYPE"), "x", vattr (var, "SIZES") or "1")
	print()
	
