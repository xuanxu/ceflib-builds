"""Display metadata information for a given CEF filename

Usage: python info.py <filename.cef>
"""
import ceflib as cef

SEP = "=" * 40

def	info (filename):

	cef.read_metadata (filename)

	print ("GLOBAL ATTRIBUTES")
	print (SEP)

	gattr = cef.gattrs()

	for k,v in gattr.items():
		print ("- %s = %s" % (k, v))

	print ()
	print ("METADATA")
	print (SEP)

	meta = cef.metadata()

	for k, v in meta.items():
		print ("- %s = %s" % (k, v))

	print ()
	print ("VARIABLE ATTRIBUTES")
	print (SEP)

	for varname in cef.varnames():

		print (varname)

		vattr = cef.vattrs (varname)

		for k,v in vattr.items():	
			print ("- %s = %s" % (k, v))
		print ()


if __name__ == "__main__":

	from sys import argv, exit

	for filename in argv [1:]:
		info (filename)

