import ceflib as cef

cef.verbosity (0)

def	test_datetime64 (filename):

	cef.read (filename)

	t = cef.datetime64 ("time_tags")

	print ("datetime64('time_tags')")
	print (t)

	print ("FIRST =", t[0])
	print ("LAST  =", t[-1])

	t = cef.datetime ("time_tags")

	print ("datetime('time_tags')")
	print (t)
	print ("FIRST =", t[0].isoformat())
	print ("LAST  =", t[-1].isoformat())

	cef.close()


if __name__ == "__main__":

	from sys import argv, exit

	test_datetime64 (argv [1])
	
