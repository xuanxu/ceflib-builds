/*******************************************************************************
 *
 *	Fichier:	ceflib.c
 *
 *	Auteur:		Alain BARTHE
 *
 *	Fonction:	Librairie dynamique de lecture du format CEF
 *
 *	$Id: ceflib.c,v 1.13 2016/06/24 09:37:26 barthe Exp $
 **/

#include <Python.h>
#include "numpy/arrayobject.h"

#include <stdlib.h>

#include "CEFLIB.h"

#define	IS_SCALAR(var) ((var->nb_size == 1) && (var->nb_elem == 1))


/*******************************************************************************
 *
 *	Set CEFLIB verbosity
 *	--------------------
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
 * 	----------------------
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
static PyObject * cef_gattributes (PyObject * self, PyObject * args)
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

		printf ("ERROR : Unknown variable : %s \n", str);
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

		printf ("ERROR : unknown global attribute : %s\n", str);
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

		printf ("Variable %s inconnue\n", str);
		return NULL;
	}

	if ((attr = Get_attr (var->attr, key)) == NULL) {

		/* On renvoie None, indiquant que l'attribut n'existe pas */
		Py_RETURN_NONE;
	}

	return make_attribute_value (attr);
}


/*******************************************************************************
 *
 *	Return an n_D array of variable data
 */
static	PyObject * cef_var (PyObject * self, PyObject * args)
{
	char *		str;
	t_variable *	var = NULL;
	int		i, ndim;
	int		nelem;
	int		nrec;
	npy_intp	dims [10];
	PyObject *	res = NULL;
	int		type;
	int		size;

	if (! PyArg_ParseTuple (args, "s", & str)) 
		return NULL;

	if ((var = Get_variable (str)) == NULL) {

		printf ("ERROR : unkown variable : %s\n", str);
		return NULL;
	}

	nrec  = Records_count ();
	nelem = var->nb_elem;
	ndim  = 0;
	
	if (var->varying) {

		dims [ndim++] = nrec;
		nelem *= nrec;

		if (! IS_SCALAR (var))
			for (i = 0; i < var->nb_size; i++) dims [ndim++] = var->size [i];
	}
	else	for (i = 0; i < var->nb_size; i++) dims [ndim++] = var->size [i];

	switch (var->type) {

	case	CEF_ISO_TIME:	
		type = NPY_DOUBLE;
		size = sizeof (double);
		break;

	case	CEF_ISO_TIME_RANGE:	/* Ajouter une dimension (taille = 2) */
		type = NPY_DOUBLE;
		dims [ndim++] = 2;
		nelem *= 2;
		size = sizeof (double);
		break;

	case	CEF_CHAR:
		type = NPY_OBJECT;
		size = 0;
		break;

	case	CEF_INT:	
		type = NPY_INT;
		size = sizeof (int);
		break;

	case	CEF_FLOAT:
		type = NPY_FLOAT;
		size = sizeof (float);
		break;

	case	CEF_DOUBLE:
		type = NPY_DOUBLE;
		size = sizeof (double);
		break;

	default: return NULL;

	}

	if (size != 0) {

		res = PyArray_SimpleNewFromData (ndim, dims, type, var->data);

	/*	memcpy (PyArray_DATA (res), var->data, nelem * size); */
	}
	else {	res = PyArray_SimpleNew (ndim, dims, type);

		PyObject ** data = PyArray_DATA (res);

		for (i=0; i < nelem; i++) {

			char *		str = ((char **) var->data) [i];

			data [i] = Py_BuildValue ("s", str);
		}
	}

	return res;
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
 *	Convertes internal time-tags to Unix time stamp
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
 *
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
 *	List of library callable methods
 */
static	PyMethodDef fonctions [] = {

	{ "verbosity",		cef_verbosity,		METH_VARARGS,	"verbosity (int) -- return None\n"
									"Set CEFLIB verbosity (0..5)" },
	{ "version",		cef_version,		METH_VARARGS,	"Display CEFLIB version"},
	{ "read",		cef_read,		METH_VARARGS,	"read(string) -- return error code\n"
									"Read CEF file and load data in memory" },
	{ "read_metadata",	cef_read_metadata,	METH_VARARGS,	"read_metadata(string) -- return error code\n"
									"Read CEF file metadata only" },
	{ "close",		cef_close,		METH_NOARGS,	"Release allocated ressources"},
	{ "records",		cef_records,		METH_VARARGS,	"Get records number of an open CEF file"},
	{ "varnames",		cef_varnames,		METH_VARARGS,	"varnames () -- List of strings\n"
									"Get CEF variable names" },
	{ "var",		cef_var,		METH_VARARGS,	"Get variable data" },
	{ "metanames",		cef_metanames,		METH_VARARGS,	"Get metadata sections names" },
	{ "meta",		cef_meta,		METH_VARARGS,	"Get metadata entries" },
	{ "gattributes",	cef_gattributes,	METH_VARARGS,	"Get CEF global attributes" },
	{ "vattributes",	cef_vattributes,	METH_VARARGS,	"Get CEF global attributes" },
	{ "gattr",		cef_gattr,		METH_VARARGS,	"Get globale attribute value"},
	{ "vattr",		cef_vattr,		METH_VARARGS,	"Get attribute value for a given variable"},
	{ "milli_to_isotime",	milli_to_isotime, 	METH_VARARGS,	"Converts internal time-tags to ISOTIME strings" },
	{ "isotime_to_milli",	isotime_to_milli,	METH_VARARGS, 	"Converts ISOTIME to internal time-tags" },
	{ "milli_to_timestamp", milli_to_timestamp,	METH_VARARGS,	"Converts internal time-tags fo Unix timestamp" },
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

	PyObject *PyInit_ceflib(void)
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
