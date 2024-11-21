/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: CEF.h,v $
 *
 *	Version	: $Revision: 1.19 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2015/06/24 14:06:38 $
 *
 *	==========================================================================================
 *
 *	Module d'analyse d'un fichier au format CEF
 */

#ifndef	__CEF_H
#define __CEF_H

#include <zlib.h>

#include "CEFLIB.h"

typedef	char *	t_string;

typedef	struct {		/* CEF time range */

	double	start;
	double	stop;

}	t_time_range;

typedef	enum {			/* CEF data types */

	CEF_UNDEF,
	CEF_INT,
	CEF_FLOAT,
	CEF_DOUBLE,
	CEF_CHAR,
	CEF_BYTE,
	CEF_ISO_TIME,
	CEF_ISO_TIME_RANGE,

}	T_CEF_TYPE;

extern 	t_symbol 	data_types [];


#define	MAX_SIZES	5
#define	MAX_DEPENDS	6

typedef	struct var {				/* Variable structure */

	char *		name;	
	T_CEF_TYPE	type;
	int		nb_size;
	int		nb_elem;
	char *		fillval;
	int		varying;	
	int		size	[MAX_SIZES];
	struct var *	depend	[MAX_DEPENDS];
	struct var *	delta_minus;
	struct var *	delta_plus;
	void *		data;			
	t_attr *	attr;

}	t_variable;


typedef	struct {				/* Variable range descriptor */

	double	max;
	double	moy;
	double	min;

}	t_descr;


extern	char *		eof_marker;		/* Default end of file marker	*/


/**
 *	Return T_CEF_TYPE for a VALUE_TYPE string
 */
T_CEF_TYPE Get_value_type (char * type);



/***
 *	Produce default header, with inclusion of separate header file
 */
t_err	Produce_CEF_header (FILE * output, t_filename filename);


/***
 *	Read CEF metadata only
 */
t_err	Read_CEF_metadata (char * filename);


/***
 *	Read CEF entire file
 */
t_err	Read_CEF_file (char * filename);


/***
 *
 *	Close an open CEF file
 */
t_err	Close_CEF_file (void);


/***
 *	Opens a CEF file and return number of records
 */
t_err	Get_CEF_records_count (char * filename, int * count);


/***
 *	Return number or records in the file
 */
int	Records_count (void);

	
/***
 *	Return number of variables in the file
 */
int	Variables_count (void);


/***
 *	Display the name of the variables in the file
 */
t_err	Display_variables (void);


/***
 *	Return variable descriptor (NULL is variable name not found)
 */
t_variable * Get_variable (char * name);	

/***
 *	Return the nth variable
 */
t_variable * Get_variable_number (int i);


/***
 *	Indicates if variable is record varying
 */
int	Record_varying (t_variable *);


/***
 *	Return number of metadata sections
 */
int	Metadata_count ();


/***
 *	Return metadata descriptor (NULL is metadata name not found)
 */
t_meta * Get_meta (char * name);


/***
 *	Return metadata descriptor (NULL is metadata name not found)
 */
t_meta * Get_meta_number (int i);


/***
 *	Set variable descriptor, and display error message if variable name not found
 */
t_err	Set_variable (t_variable ** var, char * varname);


/***
 *	Allocate table for variable ranges descriptors
 */
t_err	Allocate_table (t_variable * var, t_descr ** table);


/***
 *	Fill variable range descriptors
 */
t_err	Compute_table (t_variable * var, int record, t_descr * table);


/***
 *	Access to globals attributes
 */
t_attr * Get_gattr (void);


/***
 *	Acces to variable data :
 *
 *	Method name is in the form <type><dimension>D
 *
 *	<type>		T : ISO_TIME
 *			I : INT
 *			F : FLOAT
 *			S : STRING
 *
 *	<dimension>	Variable dimensions (1, 2 or 3)
 */
double	T1D (t_variable * var, int record, int i);
char	B1D (t_variable * var, int record, int i);
int	I1D (t_variable * var, int record, int i);
float	F1D (t_variable * var, int record, int i);
double	D1D (t_variable * var, int record, int i);
char *	S1D (t_variable * var, int record, int i);

double	T2D (t_variable * var, int record, int i, int j);
char	B2D (t_variable * var, int record, int i, int j);
int	I2D (t_variable * var, int record, int i, int j);
float	F2D (t_variable * var, int record, int i, int j);
double	D2D (t_variable * var, int record, int i, int j);
char *	S2D (t_variable * var, int record, int i, int j);

double	T3D (t_variable * var, int record, int i, int j, int k);
char	B3D (t_variable * var, int record, int i, int j, int k);
int	I3D (t_variable * var, int record, int i, int j, int k);
float	F3D (t_variable * var, int record, int i, int j, int k);
double	D3D (t_variable * var, int record, int i, int j, int k);
char *	S3D (t_variable * var, int record, int i, int j, int k);

#endif
