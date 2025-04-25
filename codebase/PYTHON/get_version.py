#! /bin/env python

def	get_version ():
	"""
	Extract ceflib version from CEFLIB.h
	"""
	for line in open ("../C/inc/CEFLIB.h"):
		if "CEFLIB_RELEASE_VERSION" in line:
			str = line.strip().split()[-1]
			return  str.replace('"',"")

if __name__ == "__main__":

	print ("ceflib version =", get_version())

