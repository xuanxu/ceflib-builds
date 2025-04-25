all:	ceflib.so
clean:
	rm -fr build/

raz:	clean
	rm -f ceflib.so

ceflib.so:	src/ceflib.c
	python setup.py build_ext --inplace
	#chcon -t textrel_shlib_t ceflib.so
