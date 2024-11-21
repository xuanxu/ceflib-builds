/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: SUBST.c,v $
 *
 *	Version	: $Revision: 1.5 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2017/06/27 13:02:44 $
 *
 *	==========================================================================================
 *
 *	Module dedie a la substitution de chaines de caracteres
 */

#define	MODULE_NAME	"SUBST"

#ifndef _WIN32

#include <sys/types.h>
#include <regex.h>

#include "CISLIB.h"
#include "SUBST.h"


#define	MAX_SIZE	1024
#define	MAX_ASSOC	100

typedef	struct {				/* Entree table associative	*/

	char *	key;				/* cle : nom de variable	*/
	char *	lib;				/* lib : libelle a substituer	*/ 

}	t_assoc;

static	t_assoc		tab [MAX_ASSOC];

static	int		tab_size = 0;


/***************************************************************************************************
 *
 *	Purge la table des associations (cle, libelle)
 *	----------------------------------------------
 */
t_err	Clear_substitutions ()
{
	char *		fonction = FNAME ("Clear_substitutions");
	t_err		error = OK;
	int		i;

	for (i = 0; i < tab_size; i++) {

		if (tab [i].key != NULL) free (tab [i].key);
		if (tab [i].lib != NULL) free (tab [i].lib);
	}

	tab_size = 0;

EXIT:	return error;
}



/***************************************************************************************************
 *
 *	Affiche la liste des substitutions
 *	----------------------------------
 */
t_err	Display_substitutions (void)
{
	char *		fonction= FNAME ("Display_substitutions");
	t_err		error = OK;
	int		i;

	for (i = 0; i < tab_size; i++) 
		Affiche_trace (1, fonction, "%s = %s", tab [i].key, tab [i].lib);

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Ajoute une association (cle, valeur) a la table
 *	-----------------------------------------------
 */
t_err	Add_substitution (char * key, char * format, ...)
{
	char *		fonction = FNAME ("Add_substitution");
	t_err		error = OK;
	char		buffer [1024];
	va_list		ptr;

	if (tab_size >= MAX_ASSOC) {

		Affiche_erreur (fonction, "Table associations pleine (%d valeurs)", MAX_ASSOC);
		error = ERR_malloc;
		goto EXIT;
	}

	va_start (ptr, format);
	vsprintf (buffer, format, ptr);
	va_end (ptr);

	tab [tab_size].key = (char *) strdup (key);
	tab [tab_size].lib = (char *) strdup (buffer);

	tab_size ++;

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Retourne la valeur de la cle
 *	----------------------------
 */
static	char *	Substitute (char * key)
{
	int	i;

	for (i = 0; i < tab_size; i++) {

		if (strcmp (tab [i].key, key) == 0) return tab [i].lib;		
	}

	return (char *) NULL;
}


/***************************************************************************************************
 *
 *	Recherche et substitution des variables depuis fichier d'entree vers sortie
 *	---------------------------------------------------------------------------
 */
t_err	Search_replace (char * input_file, FILE * sortie)
{
	char *		fonction = FNAME ("Search_replace");
	t_err		error = OK;
	FILE *		entree = NULL;
	char		buffer [512];
	regex_t		expr;
	regmatch_t	match;
	int		nbre_error = 0;

	if ((entree = fopen (input_file, "r")) == NULL) {

		Affiche_erreur (fonction, "Ouverture %s impossible", input_file);
		error = ERR_fopen;
		goto EXIT;
	}

	Affiche_trace (1, fonction, "Lecture %s", input_file);

	if (regcomp (& expr, "<[^>]*>", 0) != 0) {

		Affiche_erreur (fonction, "Expression rationnelle incorrecte");
		error = ERROR;
		goto EXIT;
	}

	while (Read_line (buffer, entree) == OK) {

		char *	start = buffer;

		while (regexec (& expr, start, 1, & match, 0) == 0) {

			char *	key;
			char *	value;

			start [match.rm_so] = 0;
			start [match.rm_eo -1] = 0;

			key = start + match.rm_so + 1;	
			value = Substitute (key);

			if (value == NULL) {

				fprintf (sortie, "%s<%s>", start, key);

				Affiche_erreur (fonction, "Variable inconnue : <%s>", key);
				nbre_error ++;
			}
			else 	fprintf (sortie, "%s%s", start, value);

			start += match.rm_eo;
		}
		fprintf (sortie, "%s\n", start);
	}

	regfree (& expr);

	if (nbre_error > 0) {

		Affiche_erreur (fonction, "%d erreurs rencontrees", nbre_error);
		error = ERROR;
	}
	
EXIT:	if (entree != NULL) fclose (entree);
	return error;
}
#endif
