/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: CEF.c,v $
 *
 *	Version	: $Revision: 1.69 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2025/04/24 15:48:21 $
 *
 *	==========================================================================================
 *
 *	Module d'analyse du contenu d'un fichier au format CEF 
 */

#define	MODULE_NAME	"CEF"

#include "CEFLIB.h"
#include "CEF.h"

#define	MAX_VARS	100
#define	MAX_META	100

#define	BUF_SIZE	1 << 24
#define	STR_SIZE	1024

#define IS_DIGIT(c)     ((c) >= '0' && (c) <= '9')
#define	WHITESPACE(c)	((c) == ' ' || (c) == '\t')
#define	QUOTES(c)	((c) == '"' || (c) == '\'')

#define	COMMENT(str)	(str [strspn (str, " \t")] == '!')

#define	EMPTY(str)	(str [strspn (str, " \t")] == 0)

static	char		buffer [BUF_SIZE];

static	char		dataset_suffix [STR_SIZE];

char *			eof_marker = "End";

typedef	struct {				/* CEF file descriptor		*/

	int		nb_var;			/* - number of variables	*/
	int		nb_const;		/* - number of constants	*/
	int		nb_meta;		/* - number of metadata		*/
	int		nb_line;		/* - number of lines		*/
	int		nb_record;		/* - number of records in file	*/
	int		nb_values;		/* - number of values / record	*/
	int		record;			/* - current record number	*/
	gzFile 		entree;			/* - file handler		*/
	t_variable *	variable [MAX_VARS];	/* - variables table		*/
	t_meta *	meta [MAX_META];	/* - metadata entries		*/
	t_attr *	attr;			/* - global attribute table	*/
	char *		end_of_record;		/* - end of record marker	*/
	char *		data_until;		/* - end of file marker		*/

}	cef_descr;


static	cef_descr	root;			/* CEF root structure		*/

#define	MAX_INC_LEVEL	5

typedef	struct {				/* Included files structure	*/

	int	level;
	FILE *	stack [MAX_INC_LEVEL];
	FILE *	descr;

}	t_include;

static	t_include	include;

t_symbol 		data_types [] = {

	{ CEF_ISO_TIME_RANGE,	"TR",	"ISO_TIME_RANGE"	},
	{ CEF_ISO_TIME,		"T",	"ISO_TIME"		},
	{ CEF_FLOAT,		"F",	"FLOAT"			},
	{ CEF_DOUBLE,		"D",	"DOUBLE"		},
	{ CEF_INT,		"I",	"INT" 			},
	{ CEF_CHAR,		"C",	"CHAR"			},
	{ CEF_BYTE,		"B",	"BYTE"			},
	{ CEF_UNDEF,		"U",	"UNDEF TYPE"		},
	{ EOF,			NULL,	NULL			}
};


/***************************************************************************************************
 *
 *	Ajoute des quotes autour d'une chaine
 *	-------------------------------------
 */
static char * Quote (char * str)
{
	static char	buffer [STR_SIZE];

	sprintf (buffer, "\"%s\"", str);

	return buffer;
}


/***************************************************************************************************
 *
 *	Retourne le type de donnees d'une variable
 *	------------------------------------------
 */
T_CEF_TYPE Get_value_type (char * type)
{
	t_symbol *	ptr = NULL;

	for (ptr = data_types; ptr->val != CEF_UNDEF; ptr++)
		if (strcasecmp (type, ptr->lib) == 0) return ptr->val;

	return CEF_UNDEF;
}



/***************************************************************************************************
 *
 *	Nouvelle version remplacant strtok ()
 *	-------------------------------------
 */
static	char * Get_token (char * buffer, char separator)
{
	char *		fonction = FNAME ("Get_token");
	static char	token [STR_SIZE];
	static char *	p;
	char *		str = token;

	if (buffer != NULL) p = buffer;

	while (WHITESPACE (*p)) p++;

	if (*p == 0 || *p == '!') return NULL;

	if (QUOTES (*p)) { 	/* string value */

		char 	eos = *p++;

		while (*p && *p != eos) *str++ = *p++;

		if (*p != eos)
			return NULL;
		else	
			p++;
	}
	else {	/* numerical value */

		while (*p && ! WHITESPACE (*p) && *p != separator && *p != '!') *str++ = *p++;
	}

	*str = 0;

	while (WHITESPACE (*p)) p++;

	/* check for separator or unauthorized token */

	if (*p != 0) {

		if (*p == separator) 
			p++;
		else if (*p != '!') {
			Affiche_erreur (fonction, "Line %d Unauthorized token : '%s'", root.nb_line, p);
			return NULL;
		}
	}

EXIT:	return token;
}


/***************************************************************************************************
 *
 *	Redefinition conversion ascii -> int
 *	-------------------------------------
 */
int	str_to_int (char * ptr)
{
	int	signe = FALSE;
	int	value = 0;

	while (*ptr == ' ' || * ptr == '\t') ptr++;

	if (* ptr == '+') ptr++;
	if (* ptr == '-') { ptr++; signe = TRUE; }

	while (*ptr && IS_DIGIT (*ptr))
		value = 10 * value + (*ptr++ - '0');

	if (signe) value = -value;

	return value;
}


/***************************************************************************************************
 *
 *	Redefinition conversion ascii -> double
 *	---------------------------------------
 */
double	str_to_double (char * ptr)
{
	int	signe = FALSE;
	double	value = 0.0;

	while (*ptr == ' ' || * ptr == '\t') ptr++;
	
	if (*ptr == '+') ptr++;
	if (*ptr == '-') { ptr++; signe = TRUE; }
	
	while (*ptr && IS_DIGIT (*ptr))
		value = 10 * value + (*ptr++ - '0');

	if (*ptr == '.') {

		double	poids = 0.1;

		ptr++;
		while (*ptr && IS_DIGIT (*ptr)) {
			value += (*ptr++ - '0') * poids;
			poids *= 0.1;
	}	}

	if (signe) value = - value;

	if (*ptr == 'e' || *ptr == 'E') {
	
		unsigned int	mask = 1;
		int		exp = 0;
		double		poids;

		ptr++;
		signe = FALSE;
		if (*ptr == '+') ptr++;
		if (*ptr == '-') { ptr++; signe = TRUE; }
		
		while (*ptr && IS_DIGIT (*ptr))
			exp = 10 * exp + (*ptr++ - '0');

		poids = signe ? 0.1 : 10.0;

		while (mask <= exp) {

			if (exp & mask) value *= poids;
			poids *= poids;
			mask <<= 1; 
	}	}

	return value;
}


/***************************************************************************************************
 *
 *	Convert a string time range to internal t_time_range
 *	----------------------------------------------------
 */
static t_err Ascii_to_time_range (char * time_range, t_time_range * tr)
{
	t_err		error = OK;
	char		buffer [100];
	char * 		p = NULL;

	strcpy (buffer, time_range);

	if ((p = strchr (buffer, '/')) == NULL) {

		tr->start = 0;
		tr->stop  = 0;
		error = ERROR;
		goto EXIT;
	}

	*p++ = 0;

	tr->start = ISOTIME_to_milli (buffer);
	tr->stop  = ISOTIME_to_milli (p);

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Search for a given CEF file :
 *	-----------------------------
 *	- in current directory
 *	- in directories defined in CEFPATH environment variable
 *
 *	Return CEF file full path or empty string if not found
 */	
static char * Search_path (char * filename)
{
	char * 		fonction = FNAME ("Search_path");
	char * 		buffer = NULL;
	char *		ptr;
	int		r;
	static t_filename fullpath;

	strcpy (fullpath, filename);

	if (access (fullpath, F_OK | R_OK) == 0) goto EXIT;

	if ((ptr = getenv ("CEFPATH")) == NULL) {

		strcpy (fullpath, "");
		goto EXIT;
	}

	buffer = strdup (ptr);

	for (ptr = strtok (buffer, PATH_SEPARATOR); ptr != NULL; ptr = strtok (NULL, PATH_SEPARATOR)) {

		r = snprintf (fullpath, FILENAME_LENGTH, "%s%s%s", ptr, DIR_SEPARATOR, filename);

		assert (r < FILENAME_LENGTH);

		if (access (fullpath, F_OK | R_OK) == 0) goto EXIT;
	}

	strcpy (fullpath, "");

EXIT:	if (buffer != NULL) free (buffer);
	return fullpath;
}


/***************************************************************************************************
 *
 *	Open (eventually nested) include file
 *	-------------------------------------
 */
static	t_err	Open_include (char * filename)
{
	char *		fonction = FNAME ("Open_include");
	t_err 		error = OK;
	t_filename	fullpath;

	strcpy (fullpath, Search_path (filename));

	if (strlen (fullpath) == 0) {

		Affiche_erreur (fonction, "File %s not found", filename);
		error = ERR_fopen;
		goto EXIT;
	}

	assert (include.level < MAX_INC_LEVEL -1);

	if ((include.descr = fopen (fullpath, "r")) == NULL) {
	
		Affiche_erreur (fonction, "Error opening %s", fullpath);
		error = ERR_fopen;
		goto EXIT;
	}

	include.stack [include.level++] = include.descr;

	Affiche_trace (2, fonction, "Reading %s", fullpath);

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Close (nested) include file
 *	---------------------------
 */
static	void	Close_include (void)
{
	fclose (include.descr);

	include.level --;

	if (include.level > 0) {

		include.descr = include.stack [include.level -1];
	}
}


/***************************************************************************************************
 *
 *	Suppress variable suffix (__<dataset_id>)
 *	-----------------------------------------
 */
static	char * Prefixe (char * str)
{
	static	char	buffer [STR_SIZE];
	int		suffix_len = strlen (dataset_suffix);
	int		str_len = strlen (str);
	char *		p = NULL;

	strcpy (buffer, str);

#define	TRUNCATE_SUFFIX
#ifdef	TRUNCATE_SUFFIX
	if (suffix_len > 0 && str_len >= suffix_len) {

		p = buffer + str_len - suffix_len;

		if (strcasecmp (p, dataset_suffix) == 0) *p = 0;
	}
#endif
	return (char *) buffer;
}


/***************************************************************************************************
 *
 *	Ajoute une variable dans la table
 *	---------------------------------
 */
static	t_variable *	Create_variable (char * var)
{
	char *		fonction = FNAME ("Create_variable");
	t_variable *	new = NULL;
	int		i;

	assert (root.nb_var < MAX_VARS);

	if ((new = (t_variable *) malloc (sizeof (t_variable))) == NULL) {

		Affiche_erreur (fonction, "Dynamic allocation failure variable %s", var);
		goto EXIT;
	}

	new->name	= strdup (Prefixe (var));
	new->nb_elem	= 1;
	new->nb_size	= 1;
	new->varying	= TRUE;
	new->type	= CEF_UNDEF;
	new->data	= NULL;
	new->fillval	= NULL;
	new->delta_plus	= NULL;
	new->delta_minus= NULL;

	if ((new->attr = Create_attr_table ()) == NULL) goto EXIT;

	for (i = 0; i < MAX_SIZES; i++) new->size [i] = 0;
	for (i = 0; i < MAX_DEPENDS; i++) new->depend [i] = NULL;

	new->size [0] = 1;

	root.variable [root.nb_var] = new;

	root.nb_var ++;

EXIT:	return new;
}


/***************************************************************************************************
 *
 *	Ajoute une constante dans la table
 *	----------------------------------
 */
static	t_variable *	Create_constant (T_CEF_TYPE t, int nb_elem, char * value)
{
	char *		fonction = FNAME ("Create_constant");
	t_err		error = OK;
	t_variable *	new = NULL;
	char		name [15];

	assert (root.nb_const < 999);

	sprintf (name, "K%03d", ++ root.nb_const);

	if ((new = Create_variable (name)) == NULL) {

		Affiche_erreur (fonction, "Unable to create constant %s", name);
		error = ERROR;
		goto EXIT;
	}

	new->type	= t;
	new->nb_elem	= nb_elem;
	new->nb_size	= 1;
	new->size [0]	= nb_elem;
	new->varying	= FALSE;

	/* Positionner attributs DATA et VALUE_TYPE */
	
	if (Erreur (error = Add_attr (new->attr, "DATA", value))) goto EXIT;

	if (Erreur (error = Add_attr (new->attr, "VALUE_TYPE", Lib_from_val (data_types, t)))) goto EXIT;

EXIT:	return new;
}


/***************************************************************************************************
 *
 *	Liberation memoire associee a la variable
 *	-----------------------------------------
 */
static	void	Delete_variable (t_variable * var)
{
	char  *		fonction = FNAME ("Delete_variable");
	int		nb_elem = (var->varying ? root.nb_record : 1) * var->nb_elem;

	Affiche_trace (3, fonction, "%s : %d x %s x %d records",
		var->name,
		var->nb_elem,
		Lib_from_val (data_types, var->type),
		var->varying ? root.nb_record : 1);

	if (var->type == CEF_CHAR) {

		int		i;
		t_string *	ptr = (t_string *) var->data;

		for (i = 0; i < nb_elem; i++) free (ptr [i]);
	}

	if (nb_elem > 0) free (var->data);

	Delete_attr_table (var->attr);

	free (var->name);

	free (var);
}


/***************************************************************************************************
 *
 *	Retourne adresse de la variable
 *	-------------------------------
 */
t_variable * Get_variable (char * name)
{
	int		i;

	for (i = 0; i < root.nb_var; i++) {

		if (strcasecmp (name, root.variable [i]->name) == 0) return root.variable [i];
	}	
	return	(t_variable *) NULL;
}


/***************************************************************************************************
 *
 *	Retourne adresse section metadata
 *	---------------------------------
 */
t_meta * Get_meta (char * name)
{
	int	i;

	for (i = 0; i < root.nb_meta; i++) {

		t_meta *	ptr = root.meta [i];

		if (strcasecmp (ptr->name, name) == 0) return ptr;
	}
	return (t_meta *) NULL;
}


/***************************************************************************************************
 *
 *	Release memory and clear descriptor
 *	-----------------------------------
 */
static	t_err	Clear_descriptor (void)
{
	char *		fonction = FNAME ("Clear_descriptor");
	t_err		error = OK;
	int		i;
	

	Affiche_trace (1, fonction, "Release memory");

	Delete_attr_table (root.attr);

	for (i = 0; i < MAX_VARS; i++) {

		if (root.variable [i] != NULL) Delete_variable (root.variable [i]);
		root.variable [i] = NULL;
	}

	for (i = 0; i < MAX_META; i++) {

		if (root.meta [i] != NULL) Delete_meta (root.meta [i]);
		root.meta [i] = NULL;
	}

	root.nb_var	= 0;
	root.nb_const	= 0;
	root.nb_meta	= 0;
	root.nb_line	= 0;
	root.nb_record	= 0;
	root.record	= 0;
	root.nb_values	= 0;
	root.attr	= NULL;

EXIT:	return error;
}
	


/*******************************************************************************
 *
 *	Initialize CEF descriptor
 *	-------------------------
 */
static	t_err Init_descriptor (void)
{
	char *		fonction = FNAME ("Init_descriptor");
	t_err		error = OK;

	if (root.entree != NULL) Clear_descriptor ();

	root.nb_record		= 0;
	root.record		= 0;
	root.nb_line		= 0;
	root.nb_var		= 0;
	root.nb_const		= 0;
	root.nb_meta		= 0;
	root.nb_values		= 0;
	root.entree		= NULL;
	root.data_until		= NULL;
	root.attr		= NULL;
	root.end_of_record	= strdup ("");
	include.level 		= 0;

	if ((root.attr = Create_attr_table ()) == NULL) {

		Affiche_erreur (fonction, "Dynamic allocation failure");
		error = ERR_malloc;
		goto EXIT;
	}

	strcpy (dataset_suffix, "");

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Ouverture fichier CEF (eventuellement zippe) en lecture
 *	-------------------------------------------------------
 */
static	t_err	Open_CEF_file (char * filename)
{
	char *		fonction = FNAME ("Open_CEF_file");
	t_err		error = OK;

	if (Erreur (error = Init_descriptor ())) goto EXIT;

	if ((root.entree = GZOPEN (filename, "rt")) == NULL) {

		Affiche_erreur (fonction, "Error opening %s", filename);
		Affiche_erreur (fonction, "%d : %s", errno, strerror (errno));
		error = ERR_fopen;
		goto EXIT;
	}

	Affiche_trace (1, fonction, "%s", filename);

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Fermuture fichier CEF
 *	---------------------
 */
t_err	Close_CEF_file (void)
{
	char *		fonction = FNAME ("Close_CEF_file");
	t_err		error = OK;

	if (root.entree != NULL) GZCLOSE (root.entree);

	root.entree = NULL;

	(void) Clear_descriptor ();

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Lecture d'une ligne du fichier CEF
 *	----------------------------------
 */
t_err	Read_CEF_line (char * buffer)
{
	char *		fonction = FNAME ("Read_CEF_line");
	t_err		error = OK;

	if (include.level > 0) {

		if (Erreur (error = Read_line (buffer, include.descr))) goto EXIT;
		
		if (error == WAR_eof) {

			(void) Close_include ();

			strcpy (buffer, "! End of include");
			
			error = OK;
		}
	}
	else {	if (GZGETS (root.entree, buffer, BUF_SIZE) == NULL) {

			strcpy (buffer, "");
			error = (GZEOF (root.entree) && root.data_until == NULL)
				? WAR_eof 
				: ERR_fgets;
			goto EXIT;
		}

		root.nb_line ++;

		if (root.data_until != NULL && strcasecmp (buffer, root.data_until) == 0) {

			error = WAR_eof;
			goto EXIT;
		}
	}
EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Lecture d'un ligne au format <cle> = <valeur>
 *	---------------------------------------------
 */
static	t_err	Parse_line (char * buffer, char * key, t_list * values)
{
	char *		fonction = FNAME ("Parse_line");
	t_err		error = OK;
	char *		tok = NULL;

	Clear_list (values);

	if ((tok = Get_token (buffer, '=')) == NULL) {

		error = ERROR;
		goto EXIT;
	}

	strcpy (key, tok);
	
	while ((tok = Get_token (NULL, ',')) != NULL) {

		if (Erreur (error = Add_item (values, tok))) goto EXIT;
	}

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Affiche la liste des variables et leurs caracteristiques
 *	--------------------------------------------------------
 */
t_err	Display_variables (void)
{
	char *		fonction = FNAME ("Display_variables");
	t_err		error = OK;
	int		i, j;

	Affiche_trace (1, fonction, "%d variables + %d constants", root.nb_var - root.nb_const, root.nb_const);

	for (i = 0; i < root.nb_var; i++) {

		char	buffer [STR_SIZE];

		t_variable * 	var = root.variable [i];

		sprintf (buffer, "%s : %d", var->name, var->size [0]);

		for (j = 1; j < var->nb_size; j++) 
			sprintf (buffer + strlen (buffer), " x %d", var->size [j]);

		sprintf (buffer + strlen (buffer), " x %s", Lib_from_val (data_types, var->type));

		if (var->varying) 
			sprintf (buffer + strlen (buffer), " x %d records", root.nb_record);

		Affiche_trace (2, fonction, "%s", buffer);

		for (j = 0; j < MAX_DEPENDS; j++) 
			if (var->depend [j] != NULL)
				Affiche_trace (2, fonction, "- depend_%d    = %s", j, var->depend [j]->name);

		if (var->delta_minus != NULL && var->delta_minus->name != NULL)
			Affiche_trace (2, fonction, "- delta_minus : %s", var->delta_minus->name);

		if (var->delta_plus != NULL && var->delta_plus->name != NULL)
			Affiche_trace (2, fonction, "- delta_plus  : %s", var->delta_plus->name);
	}

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Affiche les metadonnees
 *	-----------------------
 */
void	Display_metadata (void)
{
	char *		fonction = FNAME ("Display_metadata");
	int		m, e;
	
	Affiche_trace (2, fonction, "%d METADATA SECTIONS", root.nb_meta);

	for (m = 0; m < root.nb_meta; m++) {

		t_meta * 	ptr = root.meta [m];

		Affiche_trace (3, fonction, "SECTION : %s", ptr->name);

		for (e = 0; e < Get_size (ptr->entry); e++)
			Affiche_trace (3, fonction, "> %s", Get_item (ptr->entry, e));
	}
}


/***************************************************************************************************
 *
 *	Compte le nombre d'enregistrements apres l'entete
 *	-------------------------------------------------
 *
 *	ATTENTION : il faut au prealable avoir saute l'entete et connaitre end_of_record
 */
static	int Get_records_count ()
{
	char * 		fonction = FNAME ("Get_records_count");
	int		records = 0;
	int		offset  = strlen (root.end_of_record);

	Affiche_trace (3, fonction, "End_of_record_marker = %s", root.end_of_record);

	if (root.data_until != NULL) 
		Affiche_trace (3, fonction, "Data_until = %s", root.data_until);

	while (GZGETS (root.entree, buffer, BUF_SIZE) != NULL) {

		char * 	p;

		if (COMMENT (buffer) || EMPTY (buffer)) continue;

		if (root.data_until != NULL && strncmp (buffer, root.data_until, strlen (root.data_until)) == 0)
			break;
 
		p = buffer + strlen (buffer);

		while (p > buffer && strchr (" \t", *(p-1))) * --p = 0;

		if (strcmp (p - offset, root.end_of_record) == 0) records ++;
	}
	return records;
}


/***************************************************************************************************
 *
 *	Analyse d'un enregistrement
 *	---------------------------
 */
t_err	Read_CEF_record (void)
{
	char *		fonction = FNAME ("Read_CEF_record");
	t_err		error = OK;
	int		offset = strlen (root.end_of_record);
	int		nb_values = 0;
	int		last_line = FALSE;
	t_variable **	ptr = root.variable;
	t_variable *	var = NULL;
	int		count = 0;
	int		base = 0;

	while ((* ptr)->varying == FALSE) ptr ++;
	var    = * ptr;
	base   = root.record * var->nb_elem;
	
	while ((error = Read_CEF_line (buffer)) == OK) {

		char *	p = buffer;
		char *	tok = NULL;

		if (COMMENT (buffer) || EMPTY (buffer)) continue;
		
		if (root.record >= root.nb_record) {

			Affiche_erreur (fonction, "Unexpected line %d : %s", root.nb_line, buffer);
			error = WARNING;
			break;
		}

		p = buffer + strlen (buffer);

		while (p > buffer && WHITESPACE (*(p-1))) *--p = 0;

		p -= offset;

		if (strcmp (p, root.end_of_record) == 0) {

			* p = 0;
			last_line = TRUE;
		}

		for (tok = Get_token (buffer, ','); tok != NULL; tok = Get_token (NULL, ',')) {

			if (var == NULL) {

				Affiche_erreur (fonction, "Line %d : too much values", root.nb_line);
				error = ERROR;
				goto EXIT;
			}

			switch (var->type) {

			case	CEF_ISO_TIME_RANGE:
				Ascii_to_time_range (tok, & ((t_time_range *) var->data) [base + count ++]);
				break;

			case	CEF_ISO_TIME:
				((double *) var->data) [base + count ++] = ISOTIME_to_milli (tok);
				break;

			case	CEF_FLOAT:
				((float *) var->data) [base + count ++] = (float) str_to_double (tok);
				break;

			case	CEF_DOUBLE:
				((double *) var->data) [base + count ++] = str_to_double (tok);
				break;

			case	CEF_INT:	
				((int *) var->data) [base + count ++] = str_to_int (tok);
				break;

			case	CEF_BYTE:
				((char *) var->data) [base + count ++] = (char) str_to_int (tok);
				break;

			case	CEF_CHAR:		
				((t_string *) var->data) [base + count ++] = strdup (tok);
				break;

			default:
				break;
			}
	
			nb_values ++;

			if (count >= var->nb_elem) {

				count = 0;

				do { 	ptr ++;
					var = *ptr;
				} 
				while (var != NULL && var->varying == FALSE);

				if (var != NULL) base = root.record * var->nb_elem;
			}
		}

		if (last_line == TRUE) break;

		/***
		 *	PROBLEME DATASETS PEACE : Le EOR marker est sur une ligne separee
		 *
		if (var == NULL) {

			Affiche_erreur (fonction, "Line %d : EOR marker missing", root.nb_line);
			error = ERROR;
			goto EXIT;
		}
		***/
	}

	if (error == OK) {

		root.record ++;

		if (nb_values != root.nb_values) {

			Affiche_erreur (fonction, "Record %d : %d/%d valeurs lues", root.record, nb_values, root.nb_values);
			error = ERR_fread;
			goto EXIT;
	}	}

EXIT:	return error;
}


/*******************************************************************************
 *
 *	Initialisation d'une variable
 *	-----------------------------
 */	
static	t_err	Allocate_var (t_variable * var)
{
	char *		fonction = FNAME ("Allocate_var");
	t_err		error = OK;
	t_list *	values;
	char *		attr;
	char *		p;
	int		i, nb_elem;

	Affiche_trace (3, fonction, "NAME  : %s", var->name);
	
	/* VALUE_TYPE */

	attr = Get_attr_value (var->attr, "VALUE_TYPE");

	Affiche_trace (3, fonction, "TYPE  : %s", attr);

	var->type = Get_value_type (attr);

	if (var->type == CEF_UNDEF) {

		Affiche_erreur (fonction, "Unknown type : %s", attr);
		error = ERROR;
		goto EXIT;
	}

	/* SIZES */

	if ((values = Get_attr (var->attr, "SIZES")) != NULL) {

		var->nb_size = 0;
		var->nb_elem = 1;

		for (i = 0; i < Get_size (values); i++) {

			int	size = atoi (Get_item (values, i));

			var->nb_elem *= size;
			var->size [var->nb_size++] = size;
		}
	}
	else {	var->nb_size = 1;
		var->nb_elem = 1;
	}

	/* DEPEND_i */

	for (i = 0; i <= var->nb_size; i++) {
	
		char 		key [10];

		sprintf (key, "DEPEND_%d", i);

		attr = Get_attr_value (var->attr, key);

		if (strlen (attr)) Affiche_trace (3, fonction, "DEPEND_%d = %s", i, attr);

		var->depend [i] = Get_variable (attr);
	}

	/* DATA */

	values = Get_attr (var->attr, "DATA");

	var->varying = (values == NULL);

	nb_elem = ((var->varying) ? root.nb_record : 1) * var->nb_elem;

	switch (var->type) {

	case	CEF_ISO_TIME_RANGE:
		var->data = (t_time_range *) calloc (nb_elem, sizeof (t_time_range));
		break;

	case	CEF_ISO_TIME:
		var->data = (double *) calloc (nb_elem, sizeof (double));
		break;	

	case	CEF_FLOAT:
		var->data = (float *) calloc (nb_elem, sizeof (float));
		break;

	case	CEF_DOUBLE:
		var->data = (double *) calloc (nb_elem, sizeof (double));
		break;

	case	CEF_INT:
		var->data = (int *) calloc (nb_elem, sizeof (int));
		break;

	case	CEF_CHAR:		
		var->data = (t_string *) calloc (nb_elem, sizeof (t_string));
		break;

	case	CEF_BYTE:
		var->data = (char *) calloc (nb_elem, sizeof (char));
		break;

	default:	
		Affiche_erreur (fonction, "Unexpected variable type: %s", var->name);
		var->data = NULL;
		error = ERROR;
		break;
	}

	if (var->data == NULL) {

		Affiche_erreur (fonction, "Dynamic allocation failure");
		error = ERROR;
		goto EXIT;
	}

	if (var->varying == FALSE) {

		int	count = 0;

		for (count = 0; count < Get_size (values); count++) {

			char *	str = Get_item (values, count);

			switch (var->type) {

			case	CEF_ISO_TIME_RANGE:
				Ascii_to_time_range (str, & ((t_time_range *) var->data) [count]);
				break;

			case	CEF_ISO_TIME:
				((double *) var->data) [count] = ISOTIME_to_milli (str);
				break;

			case	CEF_FLOAT:
				((float *) var->data) [count] = (float) str_to_double (str);
				break;

			case	CEF_DOUBLE:
				((double *) var->data) [count] = str_to_double (str);
				break;

			case	CEF_INT:
				((int *) var->data) [count] = str_to_int (str);
				break;

			case	CEF_CHAR:
				((t_string *) var->data) [count] = strdup (str);
				break;

			case	CEF_BYTE:
				((char *) var->data) [count] = (char) str_to_int (str);
				break;

			default:
				break;
			}
		}

		if (count != var->nb_elem) {

			Affiche_erreur (fonction, "Variable %s : %d/%d valeurs lues", 
				var->name, count, var->nb_elem);
			error = ERROR;
			goto EXIT;
		}
	}

	/* DELTA_PLUS */

	if (strlen (attr = Get_attr_value (var->attr, "DELTA_PLUS"))) {

		T_CEF_TYPE	type = (var->type == CEF_ISO_TIME) ? CEF_FLOAT : var->type;
		t_variable *	dp = Get_variable (attr);

		if (dp == NULL && ((dp = Create_constant (type, 1, attr)) == NULL)) goto EXIT;
	
		var->delta_plus = dp;
	}

	/* DELTA_MINUS */

	if (strlen (attr = Get_attr_value (var->attr, "DELTA_MINUS"))) {

		T_CEF_TYPE	type = (var->type == CEF_ISO_TIME) ? CEF_FLOAT : var->type;
		t_variable *	dm = Get_variable (attr);

		if (dm == NULL && ((dm = Create_constant (type, 1, attr)) == NULL)) goto EXIT;

		var->delta_minus = dm;
	}
	
EXIT:	return error;
}


/*******************************************************************************
 *
 *	Parsing global attribute : KEY = VALUE
 *	--------------------------------------
 */
static	t_err	Parse_global_attr (char * key, char * value)
{
	char * 		fonction = FNAME ("Parse_global_attr");
	t_err		error = OK;

	Affiche_trace (3, fonction, "GLOBAL   : %s = %s", key, value);

	if (strcasecmp (key, "INCLUDE") == 0) {

		error = Open_include (value);

		goto EXIT;		/* Ne pas enregistrer cet attribut */
	}

	if (strcasecmp (key, "END_OF_RECORD_MARKER") == 0) {

		root.end_of_record = strdup (value);
	}

	if (strcasecmp (key, "DATA_UNTIL") == 0) {

		if (strcasecmp (value, "EOF") == 0)
			root.data_until = NULL;
		else
			root.data_until = strdup (value);
	}

	if (Erreur (error = Add_attr (root.attr, key, value))) goto EXIT;

EXIT:	return error;
}


/*******************************************************************************
 *
 *	Parsing metadata block : START_META...END_META
 *	----------------------------------------------
 */
static	t_err	Parse_metadata (char * metaname)
{
	char * 		fonction = FNAME ("Parse_metadata");
	t_err		error = OK;
	t_meta *	meta = NULL;
	t_list *	values = Create_list ();

	Affiche_trace (3, fonction, "METADATA : %s", metaname);

	assert (root.nb_meta < MAX_META);

	if ((meta = Create_meta (metaname)) == NULL) {

		error = ERROR;
		goto EXIT;
	}

	root.meta [root.nb_meta ++] = meta;
 
	while (Read_CEF_line (buffer) == OK) {

		char	key [STR_SIZE];
		char *	value = NULL;

		if (COMMENT (buffer) || EMPTY (buffer)) continue;

		if (Erreur (error = Parse_line (buffer, key, values))) goto EXIT;

		if (Get_size (values) != 1) {

			Affiche_erreur (fonction, "%s", buffer);
			Affiche_erreur (fonction, "Non unique value for keyword %s", key);
			error = ERROR;
			goto EXIT;
		}
		else	value = Get_item (values, 0);

		if (strcasecmp (key, "VALUE_TYPE") == 0)
			if (Erreur (error = Set_meta_type (meta, value))) goto EXIT;

		if (strcasecmp (key, "ENTRY") == 0) 
			if (Erreur (error = Add_meta_entry (meta, value))) goto EXIT;

		if (strcasecmp (key, "END_META") == 0) break;
	}

	if (strcasecmp (metaname, "DATASET_ID") == 0 && Get_size (meta->entry) > 0) {

		sprintf (dataset_suffix, "__%s", Get_item (meta->entry, 0));
	}

EXIT:	Delete_list (values);
	return error;
}


/*******************************************************************************
 *
 *	Parsing variable block : START_VARIABLE...END_VARIABLE
 *	------------------------------------------------------
 */
static	t_err	Parse_variable (char * varname)
{
	char * 		fonction = FNAME ("Parse_variable");
	t_err		error = OK;
	t_variable *	var = NULL;
	t_list *	values = Create_list ();

	Affiche_trace (3, fonction, "VARIABLE : %s", varname);

	if ((var = Create_variable (varname)) == NULL) {

		Affiche_erreur (fonction, "Acces variable %s impossible", varname);
		error = ERROR;
		goto EXIT;
	}

	while (Read_CEF_line (buffer) == OK) {

		char	key   [STR_SIZE];
		char *	p = buffer + strlen (buffer) -1;
		int	i;

		if (COMMENT (buffer) || EMPTY (buffer)) continue;

		/* Concatener les lignes suivantes tant que le dernier caractere est \ */

		while (*p == '\\') {

			Read_CEF_line (p);
			p += strlen (p) -1;
		}

		if (Erreur (error = Parse_line (buffer, key, values))) goto EXIT;

		if (strcasecmp (key, "END_VARIABLE") == 0) break;

		for (i = 0; i < Get_size (values); i++) {

			char *	str = Prefixe (Get_item (values, i));

			Replace_item (values, i, str);
		}

		if (Erreur (error = Set_attr (var->attr, key, values))) goto EXIT;
	}	

EXIT:	Delete_list (values);
	return error;
}


/*******************************************************************************
 *
 *	Parsing CEF header
 *	------------------
 */
static t_err	Read_CEF_header (int header_only)
{
	char *		fonction = FNAME ("Read_CEF_header");
	t_err		error = OK;
	double		t1, t2;
	int		skip_header = 0;
	int		i;
	t_list *	values = Create_list ();

	while (Read_CEF_line (buffer) == OK) {

		char	key [STR_SIZE];
		char *	value = NULL;

		if (COMMENT (buffer) || EMPTY (buffer)) continue;

		if (Erreur (error = Parse_line (buffer, key, values))) {

			Affiche_erreur (fonction, "%s", buffer);	
			Affiche_erreur (fonction, "Uncorrect line %d", root.nb_line);
			goto EXIT;
		}

		if (Get_size (values) != 1) {

			Affiche_erreur (fonction, "%s", buffer);
			Affiche_erreur (fonction, "Non unique value for keyword %s", key);
			error = ERROR;
			goto EXIT;
		}
		else	value = Get_item (values, 0);

		if (strcasecmp (key, "START_META") == 0)	error = Parse_metadata (value);
		else
		if (strcasecmp (key, "START_VARIABLE") == 0) 	error = Parse_variable (value);
		else						
								error = Parse_global_attr (key, value);
		if (Erreur (error)) goto EXIT;

		if (strcasecmp (key, "DATA_UNTIL") == 0)	break;
	}

	skip_header = root.nb_line;

	if (header_only == TRUE)  {

		root.nb_record = 1;
	}
	else {	t1 = Maintenant ();
		root.nb_record = Get_records_count ();
		t2 = Maintenant ();

		Affiche_trace (1, fonction, "%d records found in %.2f seconds", root.nb_record, (t2 - t1) / 1000.0);

		Affiche_trace (2, fonction, "Rewinding");

		if (GZREWIND (root.entree) == -1) {

			Affiche_erreur (fonction, "Error while rewinding file");
			error = ERR_fseek;
			goto EXIT;
		}

		Affiche_trace (2, fonction, "Skipping %d header lines", skip_header);

		root.nb_line = 0;

		while (root.nb_line != skip_header) Read_CEF_line (buffer);
	}

	root.nb_values = 0;

	for (i = 0; i < root.nb_var; i++) {

		if (Erreur (error = Allocate_var (root.variable [i]))) goto EXIT;
		if (root.variable [i]->varying) root.nb_values += root.variable [i]->nb_elem;
	}
	
	Affiche_trace (1, fonction, "%d values per record", root.nb_values);

	(void) Display_metadata ();
	(void) Display_variables ();

EXIT:	return error;
	Delete_list (values);
}


/***************************************************************************************************
 *
 *	Read CEF metadata only, ignoring records
 *	----------------------------------------
 */
t_err	Read_CEF_metadata (char * filename)
{
	char *		fonction = FNAME ("Read_CEF_metadata");
	t_err		error = OK;

	if (Erreur (error = Open_CEF_file (filename))) goto EXIT;

	if (Erreur (error = Read_CEF_header (TRUE))) goto EXIT;

EXIT:	Affiche_trace (1, fonction, "STATUS = %d : %s : %s",
		error, Mnemonique_erreur (error), Libelle_erreur (error));
	return error;
}


/***************************************************************************************************
 *
 *	Read CEF entire file
 *	--------------------
 */
t_err	Read_CEF_file (char * filename)
{
	char *		fonction = FNAME ("Read_CEF_file");
	t_err		error = OK;
	double		t1, t2, t3, dt;

	t1 = Maintenant ();

	if (Erreur (error = Open_CEF_file (filename))) goto EXIT;

	if (Erreur (error = Read_CEF_header (FALSE))) goto EXIT;

	t2 = Maintenant ();

	if (root.nb_var > 0) while ((error = Read_CEF_record ()) == OK);

	if (error == WAR_eof) error = OK;

	t3 = Maintenant ();

	dt = (t3 - t2) / 1000.0;

	if (root.record != root.nb_record) {

		Affiche_erreur (fonction, "%d/%d records read", root.record, root.nb_record);
		error = ERROR;
	}
	else	Affiche_trace (1, fonction, "%d records physically read in %.2f seconds", root.record, dt);

EXIT:	Affiche_trace (1, fonction, "STATUS = %d : %s : %s",
		error, Mnemonique_erreur (error), Libelle_erreur (error));
	return error;
}


/***************************************************************************************************
 *
 *	Compute number of records in a file
 *	-----------------------------------
 */
t_err	Get_CEF_records_count (char * filename, int * count)
{
	char *		fonction = FNAME ("Get_CEF_records_count");
	t_err		error = OK;
	t_list *	values = Create_list ();

	* count = -1;

	if (Erreur (error = Open_CEF_file (filename))) goto EXIT;

	while (Read_CEF_line (buffer) == OK) {

		char	key   [STR_SIZE];

		if (COMMENT (buffer) || EMPTY (buffer)) continue;

		if (Erreur (error = Parse_line (buffer, key, values))) continue;

		if (strcasecmp (key, "END_OF_RECORD_MARKER") == 0)
			root.end_of_record = strdup (Get_item (values, 0));

		if (strcasecmp (key, "DATA_UNTIL") == 0)	break;
	}

	* count = Get_records_count ();

	if (root.entree != NULL) GZCLOSE (root.entree);

EXIT:	Delete_list (values);
	return error;
}


/***************************************************************************************************
 *
 *	Various functions to access hidden data
 *	---------------------------------------
 */
int	Records_count ()
{ 
	return root.nb_record;
}

int	Variables_count ()
{
	return root.nb_var;
}

t_variable * Get_variable_number (int i)
{
	if (i < 0 || i >= root.nb_var) return NULL;

	return root.variable [i];
}

int	Metadata_count ()
{
	return root.nb_meta;
}

t_meta * Get_meta_number (int i)
{
	if (i < 0 || i >= root.nb_meta) return NULL;

	return root.meta [i];
}

t_attr * Get_gattr (void) 
{	
	return root.attr; 
}

int	Record_varying (t_variable * var)
{
	return var->varying;
}


/***************************************************************************************************
 *
 *	Initialise pointeur sur la variable
 *	-----------------------------------
 */
t_err	Set_variable (t_variable ** var, char * varname)
{
	char *		fonction = FNAME ("Set_variable");
	t_err		error = OK;

	if ((* var = Get_variable (varname)) == NULL) {

		Affiche_trace (1, fonction, "Variable inconnue : %s", varname);
		error = ERROR;
		goto EXIT;
	}

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Initialise tables de valeurs d'une variable
 *	-------------------------------------------
 */
t_err	Allocate_table (t_variable * var, t_descr ** table)
{
	char *		fonction = FNAME ("Allocate_table");
	t_err		error = OK;

	if ((* table = calloc (var->nb_elem, sizeof (t_descr))) == NULL) {

		Affiche_erreur (fonction, "Dynamic allocation failure");
		error = ERR_malloc;
		goto EXIT;
	}
	
EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Remplissage du contenu d'une table
 *	----------------------------------
 *
 *	Remplit la table a partir des champs DATA, DELTA_PLUS et DELTA_MINUS,
 *	- soit une fois pour toutes en debut de fichier
 *	- soit a chaque enregistrement
 */
t_err	Compute_table (t_variable * var, int record, t_descr * table)
{
	char *		fonction = FNAME ("Compute_table");
	t_err		error = OK;
	t_variable *	dp = var->delta_plus;
	t_variable *	dm = var->delta_minus;
	int		i, indice;

	Affiche_trace (3, fonction, "");
	Affiche_trace (3, fonction, "Variable : %s", var->name);
	Affiche_trace (3, fonction, "");
	Affiche_trace (3, fonction, "No    Mimimum    Moyenne    Maximum");
	Affiche_trace (3, fonction, "-- ---------- ---------- ----------");

	for (i = 0; i < var->nb_elem; i++) {

		table [i].max = table [i].min = table [i].moy = F1D (var, record, i);

		if (dp != NULL) table [i].max += F1D (dp, record, (dp->nb_elem > 1) ? i : 0);
		if (dm != NULL) table [i].min -= F1D (dm, record, (dm->nb_elem > 1) ? i : 0);

		if (Record_varying (var)) continue;

		Affiche_trace (3, fonction, "%02d % 10.3f % 10.3f % 10.3f", 
			i, 
			table [i].min,
			table [i].moy,
			table [i].max);
	}

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Ecriture entete du fichier
 *	--------------------------
 */
t_err	Produce_CEF_header (FILE * output, char * fullname)
{
	char *		fonction = FNAME ("Produce_CEF_header");
	t_err		error = OK;
	t_filename	logical_file_id;
	char *		ptr;
	t_date		date;
	int		y, m, d, version;
	char		amj [12];
	t_filename	include;
	t_filename	filename;

	strcpy (filename, basename (fullname));

	strcpy (logical_file_id, filename);

	if ((ptr = strstr (logical_file_id, ".cef")) == NULL) {

		Affiche_erreur (fonction, "%s : unknown file extension", filename);
		error = ERROR;
		goto EXIT;
	}

	* ptr = 0;

	if ((ptr = strstr (logical_file_id, "__")) == NULL) {

		Affiche_erreur (fonction, "%s : invalid file name", filename);
		error = ERROR;
		goto EXIT;
	}

	if (sscanf (ptr, "__%04d%02d%02d_V%d", & y, & m, & d, & version) != 4) {

		Affiche_erreur (fonction, "%s : invalid file name", filename);
		error = ERROR;
		goto EXIT;
	}
	sprintf (amj, "%04d-%02d-%02d", y, m, d);

	strcpy (include, logical_file_id);

	strcpy (strstr (include, "__"), ".ceh");
	strncpy (strstr (include, "_CP_"), "_CH_", 4);

	fprintf (output, "FILE_NAME            = %s\n", Quote (filename));
	fprintf (output, "FILE_FORMAT_VERSION  = %s\n", Quote ("CEF-2.0"));
	fprintf (output, "END_OF_RECORD_MARKER = %s\n", Quote (";"));
	fprintf (output, "\n");
	fprintf (output, "START_META   = Logical_file_ID\n");
	fprintf (output, "  ENTRY      = %s\n", Quote (logical_file_id));
	fprintf (output, "END_META     = Logical_file_ID\n");
	fprintf (output, "\n");
	fprintf (output, "START_META   = Generation_date\n");
	fprintf (output, "  VALUE_TYPE = ISO_TIME\n");
	fprintf (output, "  ENTRY      = %s\n", Milli_to_Ascii_time (Maintenant()));
	fprintf (output, "END_META     = Generation_date\n");
	fprintf (output, "\n");
	fprintf (output, "START_META   = File_time_span\n");
	fprintf (output, "  VALUE_TYPE = ISO_TIME_RANGE\n");
	fprintf (output, "  ENTRY      = %sT00:00:00.000Z/%sT23:59:59.999Z\n", amj, amj);
	fprintf (output, "END_META     = File_time_span\n");
	fprintf (output, "\n");
	fprintf (output, "START_META   = Version_number\n");
	fprintf (output, "  VALUE_TYPE = INT\n");
	fprintf (output, "  ENTRY      = %02d\n", version);
	fprintf (output, "END_META     = Version_number\n");
	fprintf (output, "\n");
	fprintf (output, "INCLUDE = %s\n", Quote (include));
	fprintf (output, "\n");

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	CEF_ISOTIME accessors
 *	---------------------
 */
double	T1D (t_variable * var, int record, int i)
{
	int	indice, offset;

	assert (var->nb_size == 1 && var->type == CEF_ISO_TIME);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + i;

	return ((double *) var->data) [indice];
}

double	T2D (t_variable * var, int record, int i, int j)
{
	int	indice, offset;

	assert (var->nb_size == 2 && var->type == CEF_ISO_TIME);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + i * var->size [1] + j;

	return ((double *) var->data) [indice];
}

double	T3D (t_variable * var, int record, int i, int j, int k)
{
	int	indice, offset;

	assert (var->nb_size == 3 && var->type == CEF_ISO_TIME);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + ((i * var->size [1] + j) * var->size [2]) + k;

	return ((double *) var->data) [indice];
}


/***************************************************************************************************
 *
 *	CEF_CHAR accessors
 *	---------------------
 */
t_string S1D (t_variable * var, int record, int i)
{
	int	indice, offset;

	assert (var->nb_size == 1 && var->type == CEF_CHAR);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + i;

	return ((t_string *) var->data) [indice];
}

t_string S2D (t_variable * var, int record, int i, int j)
{
	int	indice, offset;

	assert (var->nb_size == 2 && var->type == CEF_CHAR);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + i * var->size [1] + j;
	
	return ((t_string *) var->data) [indice];
}

t_string S3D (t_variable * var, int record, int i, int j, int k)
{
	int	indice, offset;

	assert (var->nb_size == 3 && var->type == CEF_CHAR);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + ((i * var->size [1] + j) * var->size [2]) + k;
	
	return ((t_string *) var->data) [indice];
}
	

/***************************************************************************************************
 *
 *	CEF_BYTE accessors
 *	------------------
 */
char	B1D (t_variable * var, int record, int i)
{
	int	indice, offset;

	assert (var->nb_size == 1 && var->type == CEF_BYTE);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + i;

	return ((char *) var->data) [indice];
}

char	B2D (t_variable * var, int record, int i, int j)
{
	int	indice, offset;

	assert (var->nb_size == 2 && var->type == CEF_BYTE);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + i * var->size [1] + j;

	return ((char *) var->data) [indice];
}

char	B3D (t_variable * var, int record, int i, int j, int k)
{
	int	indice, offset;

	assert (var->nb_size == 3 && var->type == CEF_BYTE);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + ((i * var->size [1] + j) * var->size [2]) + k;

	return ((char *) var->data) [indice];
}



/***************************************************************************************************
 *
 *	CEF_INT accessors
 *	-----------------
 */
int	I1D (t_variable * var, int record, int i)
{
	int	indice, offset;

	assert (var->nb_size == 1 && var->type == CEF_INT);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + i;

	return ((int *) var->data) [indice];
}

int	I2D (t_variable * var, int record, int i, int j)
{
	int	indice, offset;

	assert (var->nb_size == 2 && var->type == CEF_INT);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + i * var->size [1] + j;

	return ((int *) var->data) [indice];
}

int	I3D (t_variable * var, int record, int i, int j, int k)
{
	int	indice, offset;

	assert (var->nb_size == 3 && var->type == CEF_INT);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + ((i * var->size [1] + j) * var->size [2]) + k;

	return ((int *) var->data) [indice];
}


/***************************************************************************************************
 *
 *	CEF_FLOAT accessors
 *	-------------------
 */
float 	F1D (t_variable * var, int record, int i)
{
	int	indice, offset;

	assert (var->nb_size == 1 && var->type == CEF_FLOAT);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + i;

	return ((float *) var->data) [indice];
}

float 	F2D (t_variable * var, int record, int i, int j)
{
	int	indice, offset;

	assert (var->nb_size == 2 && var->type == CEF_FLOAT);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + i * var->size [1] + j;

	return ((float *) var->data) [indice];
}

float 	F3D (t_variable * var, int record, int i, int j, int k)
{
	int	indice, offset;

	assert (var->nb_size == 3 && var->type == CEF_FLOAT);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + ((i * var->size [1] + j) * var->size [2]) + k;

	return ((float *) var->data) [indice];
}


/***************************************************************************************************
 *
 *	CEF_DOUBLE accessors
 *	--------------------
 */
double 	D1D (t_variable * var, int record, int i)
{
	int	indice, offset;

	assert (var->nb_size == 1 && var->type == CEF_DOUBLE);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + i;

	return ((double *) var->data) [indice];
}

double 	D2D (t_variable * var, int record, int i, int j)
{
	int	indice, offset;

	assert (var->nb_size == 2 && var->type == CEF_DOUBLE);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + i * var->size [1] + j;

	return ((double *) var->data) [indice];
}

double 	D3D (t_variable * var, int record, int i, int j, int k)
{
	int	indice, offset;

	assert (var->nb_size == 3 && var->type == CEF_DOUBLE);

	offset = (var->varying) ? record * var->nb_elem : 0;

	indice = offset + ((i * var->size [1] + j) * var->size [2]) + k;

	return ((double *) var->data) [indice];
}
