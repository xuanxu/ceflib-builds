/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: FILES.c,v $
 *
 *	Version	: $Revision: 1.23 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2025/04/01 13:52:45 $
 *
 *	==========================================================================================
 *
 *	Module regroupant diverses fonctions de manipulations sur les fichiers
 */

#define	MODULE_NAME	"FILES"

#include "CISLIB.h"

#define	MAX_LINE_LENGTH	4096


static	t_filename	tmp_directory = "";

static	char *		codif_switch_off [] = {	/* Dernieres donnees CODIF 	*/

	"2004-10-25T11:35:00Z",
	"2001-01-01T00:00:00Z",
	"2009-11-11T08:17:00Z",
	"2099-12-31T00:00:00Z",
};

static	char *		hia_switch_off [] = {	/* Dernieres donnees HIA	*/

	"2099-12-31T00:00:00Z",
	"2001-01-01T00:00:00Z",
	"2009-11-11T08:17:00Z",
	"2001-01-01T00:00:00Z",
};

/***************************************************************************************************
 *
 *	Retourne la date des dernieres donnees pour un satellite et instrument
 *	----------------------------------------------------------------------
 */
double	End_of_mission (int sat, t_instr instr)
{
	if (sat < 1 || sat > 4) return 0.0;

	switch (instr) {

		case CIS_1:	return Ascii_time_to_milli (codif_switch_off [sat -1]);
		case CIS_2:	return Ascii_time_to_milli (hia_switch_off [sat -1]);
		default:	return 0.0;
	}
}


static	glob_t		buffer;			/* Buffer utilise par glob ()	*/


/****************************************************************************************************
 *
 *	Recherche de fichiers correspondant a un masque 
 *	-----------------------------------------------
 */
t_err	Search_files (char * mask, int * count)
{
	char *		fonction = FNAME ("Search_files");
	t_err		error = OK;

	* count = 0;

	buffer.gl_offs = 0;

	if (glob (mask, 0, NULL, & buffer) == -1) {

		Affiche_erreur (fonction, "Erreur glob () : masque = %s : ", mask);
		error = ERR_glob;
		goto EXIT;
	}

	* count = buffer.gl_pathc;

EXIT:	return error;	
}


/****************************************************************************************************
 *
 *	Libere ressources memoire allouees par glob ()
 *	----------------------------------------------
 */
void	Search_free (void)
{
	globfree (& buffer);

	buffer.gl_pathc = 0;
}


/****************************************************************************************************
 *
 *	Retourne le nom du nieme fichier
 *	--------------------------------
 */
char *	Get_filename (int number)
{
	if (number < 0 || number >= buffer.gl_pathc) return (char *) NULL;

	return buffer.gl_pathv [number];
}


/****************************************************************************************************
 *
 *	Lecture d'une ligne dans un fichier et suppression du saut de ligne final
 *	-------------------------------------------------------------------------
 */
t_err	Read_line (char * buffer, FILE * pf)
{
	char *		fonction = FNAME ("Read_line");
	t_err		error = OK;
	int		end;

	if (fgets (buffer, MAX_LINE_LENGTH, pf) == NULL) {

		strcpy (buffer, "");

		error = feof (pf) ? WAR_eof : ERR_fgets;

		goto EXIT;
	}

	end = strlen (buffer) -1;

	if (end >= 0 && buffer [end] == '\n') buffer [end] = 0;

	/* Handling Windows CR + LF end of line terminator */

	end --;

	if (end >= 0 && buffer [end] == '\r') buffer [end] = 0;

EXIT:	return error;
}


/****************************************************************************************************
 *
 *	Verification nombre valeurs lues par rapport aux valeurs attendues
 *	------------------------------------------------------------------
 */
t_err	Check_read_values (char * fonction, int count, int expected)
{
	t_err		error = OK;

	if (count != expected) {

		Affiche_erreur (fonction, "%d/%d valeurs lues", count, expected);
		error = ERR_fread;
	}

	return error;
}



/*	Verification nombre valeurs ecrites par rapport aux valeurs attendues
 *	---------------------------------------------------------------------
 */
t_err	Check_write_values (char * fonction, int count, int expected)
{
	t_err		error = OK;

	if (count != expected) {

		Affiche_erreur (fonction, "%d/%d valeurs ecrites", count, expected);
		error = ERR_fwrite;
	}

	return error;
}


/****************************************************************************************************
 *
 *	Creation d'un repertoire pour enregistrer les fichiers temporaires
 *	------------------------------------------------------------------
 */
t_err	Create_tmp_directory (char * key)
{
	char *		fonction = FNAME ("Create_tmp_directory");
	t_err		error = OK;
	int		r;

	r = snprintf (tmp_directory, FILENAME_LENGTH, "/tmp/%s", key);

	assert (r < FILENAME_LENGTH);

#if defined(_WIN32)

        if (mkdir (tmp_directory) == -1 && errno != EEXIST) {
#else

        if (mkdir (tmp_directory, S_IRWXU) == -1 && errno != EEXIST) {
#endif

		Affiche_erreur (fonction, "Creation repertoire %s impossible", tmp_directory);
		Affiche_erreur (fonction, "%d %s", errno, strerror (errno));
		error = ERR_fopen;
		goto EXIT;
	}

	Affiche_trace (1, fonction, "Creation repertoire %s", tmp_directory);
	
EXIT:	return error;
}


/****************************************************************************************************
 *
 *	Recuperation du nom du repertoire temporaire
 *	--------------------------------------------
 */
char *	Get_tmp_directory (void)
{
	char *		fonction = FNAME ("Get_tmp_diretory");

	if (access (tmp_directory, F_OK | W_OK) == -1) {

		Affiche_erreur (fonction, "Acces repertoire %s impossible", tmp_directory);
		return (char *) NULL;
	}

	return (char *) tmp_directory;
}


/****************************************************************************************************
 *
 *	Purge du contenu du repertoire temporaire
 *	-----------------------------------------
 */
t_err	Clear_tmp_directory (void)
{
	char *		fonction = FNAME ("Clear_tmp_directory");
	t_err		error = OK;
	t_filename	masque;
	int		i, r;
	int		count = -1;

	if (strlen (tmp_directory) == 0) goto EXIT;

	r = snprintf (masque, FILENAME_LENGTH, "%s/*.tmp", tmp_directory);

	assert (r < FILENAME_LENGTH);

	Affiche_trace (1, fonction, "Purge repertoire %s", tmp_directory);

	if (Erreur (error = Search_files (masque, & count))) goto EXIT;

	for (i = 0; i < count; i++) {

		if (unlink (Get_filename (i)) == -1) {

			Affiche_erreur (fonction, "Suppression %s impossible", Get_filename (i));
			error = ERROR;
			goto EXIT;
		}
		Affiche_trace (3, fonction, "Suppression %s", Get_filename (i));
	}

EXIT:	if (count != -1) Search_free ();
	return error;
}


/****************************************************************************************************
 *
 *	Suppression du repertoire temporaire
 *	------------------------------------
 */
t_err	Delete_tmp_directory (void)
{
	char *		fonction = FNAME ("Delete_tmp_directory");
	t_err		error = OK;

	if (Erreur (error = Clear_tmp_directory ())) goto EXIT;

	if (rmdir (tmp_directory) == -1) {

		Affiche_erreur (fonction, "Suppression repertoire %s impossible", tmp_directory);
		error = ERROR;
		goto EXIT;
	}

	strcpy (tmp_directory, "");

EXIT:	return error;
}


/****************************************************************************************************
 *
 *	Ouverture fichier temporaire
 *	----------------------------
 */
FILE *	Open_tmp_file (char * key, char * mode)
{
	t_filename	filename;
	int		r;

	r = snprintf (filename, FILENAME_LENGTH, "%s/%s.tmp", tmp_directory, key);

	assert (r < FILENAME_LENGTH);

	return fopen (filename, mode);
}
