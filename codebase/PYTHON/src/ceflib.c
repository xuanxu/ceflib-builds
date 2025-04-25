/*******************************************************************************
 *
 *	Fichier:	ceflib.c
 *
 *	Auteur:		Alain BARTHE
 *
 *	Fonction:	Librairie dynamique de lecture du format CEF
 *
 *	$Id: ceflib.c,v 1.28 2025/04/24 16:12:29 barthe Exp $
 **/

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <Python.h>
#include "numpy/arrayobject.h"

#include <stdlib.h>

#include "CEFLIB.h"

#define	IS_SCALAR(var) ((var->nb_size == 1) && (var->nb_elem == 1))


/*******************************************************************************
 *
 *	Set CEFLIB verbosity
 */
static	PyObject * cef_verbosity (PyObject * self, PyObject * args)
{
	int	level = 0;

	if (! PyArg_ParseTuple (args, "i", & level)) 
		return NULL;

	Set_trace_level (level);

	return	Py_BuildValue ("i", level);
}


/*******************************************************************************
 *
 * 	Display CEFLIB version
 */
static	PyObject * cef_version (PyObject * self, PyObject * args)
{
	if (Get_trace_level () > 0)
		printf ("ceflib %s - %s\n", CEFLIB_RELEASE_VERSION, CEFLIB_RELEASE_DATE);

	return Py_BuildValue ("s", CEFLIB_RELEASE_VERSION);
}


/*******************************************************************************
 *
 *	Read a CEF file and load data in memory
 */
static	PyObject * cef_read (PyObject * self, PyObject * args)
{
	char * 		str;

	if (! PyArg_ParseTuple (args, "s", & str)) 
		return NULL;

	return Py_BuildValue ("i", Read_CEF_file (str));
}


/*******************************************************************************
 *
 *	Read CEF metadata only
 */
static	PyObject * cef_read_metadata (PyObject * self, PyObject * args)
{
	char *		str;

	if (! PyArg_ParseTuple (args, "s", & str)) 
		return NULL;

	return Py_BuildValue ("i", Read_CEF_metadata (str));
}


/*******************************************************************************
 *
 *	Close file and release ressources
 */
static	PyObject * cef_close ()
{
	return Py_BuildValue ("i", Close_CEF_file());
}


/*******************************************************************************
 *
 *	Return number of records in the CEF file
 */
static	PyObject * cef_records (PyObject * self, PyObject * args)
{
	return Py_BuildValue ("i", Records_count ());
}


/*******************************************************************************
 *
 *	Return list of CEF variables's name
 */
static	PyObject * cef_varnames (PyObject * self, PyObject * args)
{
	PyObject *	res = NULL;
	int		i, nb_var;

	nb_var = Variables_count ();

	res = PyList_New (nb_var);

	for (i = 0; i < nb_var; i++)
		PyList_SetItem (res, i, Py_BuildValue ("s", Get_variable_number (i)->name)); 

	return res;
}


/*******************************************************************************
 *
 *	Return list of metadata section names
 */
static	PyObject * cef_metanames (PyObject * self, PyObject * args)
{
	PyObject *	res = NULL;
	int		nb_meta = Metadata_count ();
	int		i;

	res = PyList_New (nb_meta);

	for (i = 0; i < nb_meta; i++) 
		PyList_SetItem (res, i,  Py_BuildValue ("s", Get_meta_number (i)->name));

	return res;
}


/*******************************************************************************
 *
 *	Return list of metadata entries
 */
static	PyObject * cef_meta (PyObject * self, PyObject * args)
{
	char * 		str;
	t_meta *	meta = NULL;
	PyObject *	res = NULL;
	int		i, count = 0;

	if (! PyArg_ParseTuple (args, "s", & str)) 
		return NULL;

	if ((meta = Get_meta (str)) == NULL) 
		return NULL;

	count = Get_size (meta->entry);	
	
	if ((res = PyList_New (count)) == NULL) 
		return NULL;
	
	for (i = 0; i < count; i++)
		PyList_SetItem (res, i, Py_BuildValue ("s", Get_item (meta->entry, i)));

	return res;
}


/*******************************************************************************
 *
 *	Return a list of attributes names
 */
static	PyObject * make_attributes_list (t_attr * attr)
{
	PyObject *	res = PyList_New (attr->count);
	int		i;

	for (i = 0; i < attr->count; i++)
		PyList_SetItem (res, i, Py_BuildValue ("s", attr->data [i].key));
	
	return res;
}


/*******************************************************************************
 *
 *	Return global attributes names
 */
static	PyObject * cef_gattributes (PyObject * self, PyObject * args)
{
	if (! PyArg_ParseTuple (args, "")) 
		return NULL;

	return make_attributes_list (Get_gattr ());
}


/*******************************************************************************
 *
 *	Return attributes names for a given variable
 */
static	PyObject * cef_vattributes (PyObject * self, PyObject * args)
{
	char *		str = NULL;
	t_variable *	var = NULL;

	if (! PyArg_ParseTuple (args, "s", & str)) 
		return NULL;

	if ((var = Get_variable (str)) == NULL) {

		PyErr_Format (PyExc_KeyError, "Unknown variable %s", str);
		return NULL;
	}

	return make_attributes_list (var->attr);
}
	

/*******************************************************************************
 *
 *	Return value of a given attribute :
 *	- a string if value is unique
 *	- a list otherwise
 */
static	PyObject * make_attribute_value (t_list * values)
{
	PyObject *	res = NULL;
	int		size = Get_size (values);
	int		i;

	if (size > 1) {

		res = PyList_New (size);

		for (i = 0; i < size; i++)
			PyList_SetItem (res, i, Py_BuildValue ("s", Get_item (values, i)));
	}
	else		
		res = Py_BuildValue ("s", Get_item (values, 0));

	return res;
}


/*******************************************************************************
 *
 *	Return attribute value for a given global attribute
 */
static	PyObject * cef_gattr (PyObject * self, PyObject * args)
{
	char *		str;
	t_list *	attr = NULL;

	if (! PyArg_ParseTuple (args, "s", & str)) 
		return NULL;

	if ((attr = Get_attr (Get_gattr (), str)) == NULL) {

		PyErr_Format (PyExc_KeyError, "Unknown global attribute %s", str);
		return NULL;
	}

	return make_attribute_value (attr);
}


/*******************************************************************************
 *
 *	Return a variable attribute value for given attribute and variable
 */
static	PyObject * cef_vattr (PyObject * self, PyObject * args)
{
	char *		str;
	char *		key;
	t_variable *	var = NULL;
	t_list *	attr = NULL;

	if (! PyArg_ParseTuple (args, "ss", & str, & key)) 
		return NULL;

	if ((var = Get_variable (str)) == NULL) {

		PyErr_Format (PyExc_KeyError, "Unknown variable %s", str);
		return NULL;
	}

	if ((attr = Get_attr (var->attr, key)) == NULL) {

		/* Return None as attribute doesn't exists */
		Py_RETURN_NONE;
	}

	return make_attribute_value (attr);
}


/*******************************************************************************
 *
 *	Return dict (attribute, value) for global attributes
 */
static	PyObject * cef_gattrs (PyObject * self, PyObject * args)
{
	PyObject *	dict = NULL;
	t_attr *	attr = Get_gattr();
	int		i;

	if (attr == NULL) return NULL;
	if (! PyArg_ParseTuple (args, ""))
		return NULL;

	if ((dict = PyDict_New ()) == NULL)
		return NULL;

	for (i = 0; i < attr->count; i++) {

		PyDict_SetItemString (dict, 
			attr->data[i].key,
			make_attribute_value (attr->data[i].values));
	}
	return dict;
}


/*******************************************************************************
 *
 *	Return dict (attribute, value) for a given variable
 */
static	PyObject * cef_vattrs (PyObject * self, PyObject * args)
{
	char *		str;
	t_variable *	var = NULL;
	t_attr	*	attr = NULL;
	PyObject *	dict = NULL;
	int		i;

	if (! PyArg_ParseTuple (args, "s", & str))
		return NULL;

	if ((var = Get_variable (str)) == NULL) {

		PyErr_Format (PyExc_KeyError, "Unknown variable %s", str);
		return NULL;
	}

	if ((attr = var->attr) == NULL) {

		PyErr_Format (PyExc_KeyError, "No attributes for variable %s", str);
		return NULL;
	}

	if ((dict = PyDict_New ()) == NULL)
		return NULL;

	for (i = 0; i < attr->count; i++) {

		PyDict_SetItemString (dict, 
			attr->data[i].key,
			make_attribute_value (attr->data[i].values));
	}
	return dict;
}


/*******************************************************************************
 *
 *	Return dict (metadata, value) for all metadata sections
 */
static	PyObject * cef_metadata (PyObject * self, PyObject * args)
{
	PyObject *	dict = NULL;
	int		count = Metadata_count();
	int		i;

	if (! PyArg_ParseTuple (args, ""))
		return NULL;

	if ((dict = PyDict_New ()) == NULL)
		return NULL;

	for (i = 0; i < count; i++) {

		t_meta *	meta = Get_meta_number (i);

		PyDict_SetItemString (dict,
			meta->name,
			make_attribute_value (meta->entry));
	}

	return dict;
}


/*******************************************************************************
 *
 *	Converts internal time-tags to ISOTIME strings
 */
static	PyObject * milli_to_isotime (PyObject * self, PyObject * args)
{
	double	milli;
	int	res;

	if (! PyArg_ParseTuple (args, "di", & milli, & res)) 
		return NULL;

	return Py_BuildValue ("s", Milli_to_ISOTIME (milli, res));
}


/*******************************************************************************
 *
 *	Converts internal time-tags to Unix time stamp
 *
 *	internal : (double) milli-seconds from 1958-01-01T00:000
 *
 *	Unix     : (double) fractional seconds from 1970-01-01T00:00
 */
static	PyObject * milli_to_timestamp (PyObject * self, PyObject * args)
{
	double milli;

	if (! PyArg_ParseTuple (args, "d", & milli))
		return NULL;

	milli = milli / 1000.0 - 4383 * 86400.0;

	return Py_BuildValue ("d", milli);
}


/*******************************************************************************
 *
 *	Convert ISOTIME strings to internal time-tags
 */
static	PyObject * isotime_to_milli (PyObject * self, PyObject * args)
{
	char *	str;

	if (! PyArg_ParseTuple (args, "s", & str)) 
		return NULL;

	return Py_BuildValue ("d", ISOTIME_to_milli (str));
}


/*******************************************************************************
 *
 *	Compute sizes and dimensions of numpy array
 */
static	void	Compute_numpy_sizes (t_variable * var, int * nb_elem, int * nb_dim, npy_intp * dims)
{
	int	nrec = Records_count();
	int	nelem = var->nb_elem;
	int	ndim = 0;
	int	i;

	if (var->varying) {

		dims [ndim++] = nrec;
		nelem *= nrec;

		if (! IS_SCALAR (var))
			for (i = 0; i < var->nb_size; i++) dims [ndim++] = var->size [i];
	}
	else	for (i = 0; i < var->nb_size; i++) dims [ndim++] = var->size [i];

	*nb_elem = nelem;
	*nb_dim = ndim;
}


/*******************************************************************************
 *
 *	Return an numpy array of variable data
 */
static	PyObject * cef_var (PyObject * self, PyObject * args)
{
	char *		str;
	t_variable *	var = NULL;
	int		i, ndim, nelem;
	int		type;
	npy_intp	dims [10];
	PyObject *	res = NULL;

	if (! PyArg_ParseTuple (args, "s", & str)) 
		return NULL;

	if ((var = Get_variable (str)) == NULL) {

		PyErr_Format (PyExc_KeyError, "Unknwon variable %s", str);
		return NULL;
	}

	Compute_numpy_sizes (var, & nelem, & ndim, dims);

	switch (var->type) {

	case	CEF_ISO_TIME:	
		type = NPY_DOUBLE;
		break;

	case	CEF_ISO_TIME_RANGE:	/* add new dimension of size = 2 */
		type = NPY_DOUBLE;
		dims [ndim++] = 2;
		nelem *= 2;
		break;

	case	CEF_CHAR:
		type = NPY_OBJECT;
		break;

	case	CEF_INT:	
		type = NPY_INT;
		break;

	case	CEF_FLOAT:
		type = NPY_FLOAT;
		break;

	case	CEF_DOUBLE:
		type = NPY_DOUBLE;
		break;

	default: 
		return NULL;
	}

	if (type != NPY_OBJECT) {

		res = PyArray_SimpleNewFromData (ndim, dims, type, var->data);
	}
	else {
		res = PyArray_SimpleNew (ndim, dims, type);

		PyObject ** data = PyArray_DATA ((PyArrayObject *) res);

		for (i=0; i < nelem; i++) {

			char *	str = ((char **) var->data) [i];

			data [i] = Py_BuildValue ("s", str);
		}
	}
	return res;
}


/*******************************************************************************
 *
 *	Return array of Unix timestamps from a given ISOTIME variable
 */
static 	PyObject * timestamp (PyObject * self, PyObject * args)
{
	char *		str;
	t_variable *	var = NULL;
	int		i, nelem, ndim;
	npy_intp	dims [10];

	if (! PyArg_ParseTuple (args, "s", & str)) 
		return NULL;

	if ((var = Get_variable (str)) == NULL) {

		PyErr_Format (PyExc_KeyError, "Unknwon variable %s", str);
		return NULL;
	}

	Compute_numpy_sizes (var, & nelem, & ndim, dims);

	switch (var->type) {

	case	CEF_ISO_TIME:	
		break;

	case	CEF_ISO_TIME_RANGE:	/* add a new dimension of size = 2 */
		dims [ndim++] = 2;
		nelem *= 2;
		break;

	default: 
		PyErr_Format (PyExc_TypeError, "Variable %s type must be a ISO_TIME or ISO_TIME_RANGE", str);
		return NULL;
	}

	PyObject *	res = PyArray_SimpleNew (ndim, dims, NPY_DOUBLE);

	npy_double *	data = (npy_double *) PyArray_DATA ((PyArrayObject *) res);

	double *	t = (double *) var->data;

	npy_int64	offset = 4383L * 86400L;

	for (i=0; i < nelem; i++) {
	
		data [i] = t[i] / 1000.0 - offset;
	}
	return res;
}


/*******************************************************************************
 *
 *	Return array of datetime from a given ISOTIME variable
 */
static 	PyObject * datetime (PyObject * self, PyObject * args)
{
	char *		str;
	t_variable *	var = NULL;
	int		i, ndim, nelem;
	npy_intp	dims [10];

	if (! PyArg_ParseTuple (args, "s", & str)) 
		return NULL;

	if ((var = Get_variable (str)) == NULL) {

		PyErr_Format (PyExc_Exception, "Unkown CEF variable : %s", str);
		return NULL;
	}

	Compute_numpy_sizes (var, & nelem, & ndim, dims);

	switch (var->type) {

	case	CEF_ISO_TIME:	
		break;

	case	CEF_ISO_TIME_RANGE:	/* 2 values per record */
		dims [ndim++] = 2;
		nelem *= 2;
		break;

	default: 
		PyErr_Format (PyExc_TypeError, "Variable %s type must be a ISO_TIME or ISO_TIME_RANGE", str);
		return NULL;
	}

	//	import datetime module

	PyObject * mod = PyImport_ImportModule ("datetime");

	if (mod == NULL) return NULL;

	//	get class datetime.datetime 

	PyObject * class = PyObject_GetAttrString (mod, "datetime");

	if (class == NULL) return NULL;

	//	create array of object (datetime.datetime)
	//
	PyObject *	res = PyArray_SimpleNew (ndim, dims, NPY_OBJECT);

	PyObject **	data = PyArray_DATA ((PyArrayObject *) res);

	double *	t = (double *) var->data;

	for (i=0; i < nelem; i++) {

		double	timestamp = t[i] / 1000.0 - 4383 * 86400;

		data [i] = PyObject_CallMethod (class, "utcfromtimestamp", "d", timestamp);
	}

	return res;
}


/*******************************************************************************
 *
 *	Return array of datetime64 from a given ISOTIME variable
 */
static 	PyObject * datetime64 (PyObject * self, PyObject * args)
{
	char *		str;
	t_variable *	var = NULL;
	int		i, nelem, ndim;
	npy_intp	dims [10];

	if (! PyArg_ParseTuple (args, "s", & str)) 
		return NULL;

	if ((var = Get_variable (str)) == NULL) {

		PyErr_Format (PyExc_KeyError, "Unknown variable %s", str);
		return NULL;
	}

	Compute_numpy_sizes (var, & nelem, & ndim, dims);

	switch (var->type) {

	case	CEF_ISO_TIME:	
		break;

	case	CEF_ISO_TIME_RANGE:	/* add a new dimension of size = 2 */
		dims [ndim++] = 2;
		nelem *= 2;
		break;

	default: 
		PyErr_Format (PyExc_TypeError, "Variable %s type must be a ISO_TIME or ISO_TIME_RANGE", str);
		return NULL;
	}

	PyObject * 	data_type = Py_BuildValue ("s", "datetime64[ms]");

	PyArray_Descr *	descr;

	PyArray_DescrConverter (data_type, & descr);
	
	PyObject *	res = PyArray_SimpleNewFromDescr (ndim, dims, descr);

	npy_int64 *	data = (npy_int64 *) PyArray_DATA ((PyArrayObject *) res);

	double *	t = (double *) var->data;

	npy_int64	offset = 4383L * 86400000L;

	for (i=0; i < nelem; i++) {
	
		data [i] = (npy_int64) t[i] - offset;
	}
	return res;
}


PyDoc_STRVAR (doc_verbosity,
	"verbosity (level: int) > None\n\n"
	"Set CEFLIB verbosity level(0..5)");

PyDoc_STRVAR (doc_version,
	"version() -> str\n\n"
	"Display CEFLIB version");

PyDoc_STRVAR (doc_read,
	"read (filename: str) -> error_code: int\n\n"
	"Read CEF filename and load metadata and variable data in memory" );

PyDoc_STRVAR (doc_read_metadata,
	"read_metadata (filename: str) -> error_code: int\n\n"""""
	"Read CEF filename metadata only" );

PyDoc_STRVAR (doc_close,
	"close() -> None\n\n"
	"Release allocated ressources");

PyDoc_STRVAR (doc_records, 
	"records() -> int\n\n"
	"Get inumber of records of an open CEF file");

PyDoc_STRVAR (doc_varnames,
	"varnames () -> list[str]\n\n"
	"Return the list of CEF variable names");

PyDoc_STRVAR (doc_var,
	"var (varname: str) -> np.array ()\n\n"
	"Return a numpy array with the values of CEF variable");

PyDoc_STRVAR (doc_metanames,
	"metanames() -> list[str]\n\n"
	"Return list of metadata sections names");

PyDoc_STRVAR (doc_meta,
	"meta (name: str) -> str | list[str]\n\n"
	"Return metadata entries for a given name" );

PyDoc_STRVAR (doc_gattributes,
	"gattributes () -> list[str]\n\n"
	"Return list of global attribute names");

PyDoc_STRVAR (doc_vattributes,
	"vattributes (varname: str) -> list[str]\n\n"
	"Return list of attributes for a given CEF variable name");

PyDoc_STRVAR (doc_gattr,
	"gattr (attrname: str) -> str | list[str]\n\n"
	"Return global attribute value");

PyDoc_STRVAR (doc_vattr,
	"vattr (varname: str, attrname: str) -> str | list[str]\n\n"
	"Return attribute value for a given variable");

PyDoc_STRVAR (doc_milli_to_isotime,
	"milli_to_isotime (milli: float [, digits:int]) -> str\n\n"
	"Converts internal time-tags (milli seconds from 1988) to ISOTIME strings\n"
	"digits: 0 = seconds, 3 = milli-secondsi, 6 = micro-seconds");

PyDoc_STRVAR (doc_isotime_to_milli,
	"isotime_to_milli (isotime: str) -> float\n\n"
	"Converts ISOTIME to internal time-tags" );

PyDoc_STRVAR (doc_milli_to_timestamp,
	"milli_to_timestamp (float) -> float\n\n"
	"Converts internal time-tag (milli seconds from 1958) to Unix timestamp");

PyDoc_STRVAR (doc_datetime,
	"datetime (varname: str) -> np.array (datetime.datetime)\n\n"
	"Return numpy array of datetime.datetime from an ISOTIME variable");

PyDoc_STRVAR (doc_datetime64,
	"datetime64 (varname: str) -> np.array (dtype = 'datetime64[ms]')\n\n"
	"Return numpy array of datetime64 from an ISOTIME variable");

PyDoc_STRVAR(doc_timestamp,
	"timestamp (varname: str) -> np.array (dtype=float)\n\n"
	"Return numpy array of Unix timestamps");

PyDoc_STRVAR(doc_gattrs,
	"gattrs () -> Dict [str, str|List[str]]\n\n"
	"Return a dictionnary of global attributes (name, value)");

PyDoc_STRVAR(doc_vattrs,
	"vattrs (varname: str) -> Dict [str, str|List[str]]\n\n"
	"Return a dictionnary of (attribute, value) for a given variable");

PyDoc_STRVAR(doc_metadata,
	"metadata () -> Dict [str, List[str]]\n\n"
	"Return a dictionnary of (metadata-section, values) for all metada sections");


/*******************************************************************************
 *
 *	List of library callable methods
 */
static	PyMethodDef fonctions [] = {

	{ "verbosity",		cef_verbosity,		METH_VARARGS,	doc_verbosity },
	{ "version",		cef_version,		METH_VARARGS,	doc_version },
	{ "read",		cef_read,		METH_VARARGS,	doc_read},
	{ "read_metadata",	cef_read_metadata,	METH_VARARGS,	doc_read_metadata },
	{ "close",		cef_close,		METH_NOARGS,	doc_close},
	{ "records",		cef_records,		METH_VARARGS,	doc_records},
	{ "varnames",		cef_varnames,		METH_VARARGS,	doc_varnames },
	{ "var",		cef_var,		METH_VARARGS,	doc_var },
	{ "metanames",		cef_metanames,		METH_VARARGS,	doc_metanames },
	{ "meta",		cef_meta,		METH_VARARGS,	doc_meta },
	{ "gattributes",	cef_gattributes,	METH_VARARGS,	doc_gattributes },
	{ "vattributes",	cef_vattributes,	METH_VARARGS,	doc_vattributes },
	{ "gattr",		cef_gattr,		METH_VARARGS,	doc_gattr },
	{ "vattr",		cef_vattr,		METH_VARARGS,	doc_vattr },
	{ "milli_to_isotime",	milli_to_isotime, 	METH_VARARGS,	doc_milli_to_isotime },
	{ "isotime_to_milli",	isotime_to_milli,	METH_VARARGS, 	doc_isotime_to_milli },
	{ "milli_to_timestamp", milli_to_timestamp,	METH_VARARGS,	doc_milli_to_timestamp },
	{ "datetime",		datetime,		METH_VARARGS,	doc_datetime },
	{ "datetime64",		datetime64,		METH_VARARGS,	doc_datetime64 },
	{ "timestamp",		timestamp,		METH_VARARGS,	doc_timestamp },
	{ "gattrs",		cef_gattrs,		METH_VARARGS,	doc_gattrs },
	{ "vattrs",		cef_vattrs,		METH_VARARGS,	doc_vattrs},
	{ "metadata",		cef_metadata,		METH_VARARGS,	doc_metadata },
	{ NULL, NULL, 0, NULL }
};


/*******************************************************************************
 *
 *	Initial loading of the CEFLIB library
 *
 * 	Separate behaviour for Python 3.x and Python 2.x
 */
#if PY_MAJOR_VERSION >= 3

	static struct PyModuleDef moduledef = {
		PyModuleDef_HEAD_INIT,
		"ceflib",
		NULL,
		-1,
		fonctions,
		NULL,
		NULL,
		NULL,
		NULL
	};
		
		
	#define INITERROR return NULL

	PyMODINIT_FUNC PyInit_ceflib(void)
	{
		PyObject *module = PyModule_Create (& moduledef);

		import_array ();
		
		return module;
	}
#else
	#define	INITERROR return

	PyMODINIT_FUNC initceflib (void)
	{
		(void) Py_InitModule ("ceflib", fonctions);

		import_array ();
	}
#endif
