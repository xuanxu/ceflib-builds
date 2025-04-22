/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: IRAP.c,v $
 *
 *	Version	: $Revision: 1.7 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2025/04/01 13:53:29 $
 *
 *	==========================================================================================
 *
 *	Regroupement des outils propres au fonctionnement IRAP
 *
 *	- Definition des variables CIS_ROOT, CAA_ROOT, CSA_ROOT
 *	- Location des jeux de donnees CEF
 */

#define	MODULE_NAME	"IRAP"

#include "CISLIB.h"

static char *	cis_root = NULL;	/* Racine arborescence Cluster	*/
static char *	caa_root = NULL;	/* Racine arborescence CAA	*/
static char *	csa_root = NULL;	/* Racine arborescence CSA	*/

/*** 
 *	Numero minimal de version d'un fichier CEF
 *
 *	Le numero est maintenant lu dans la variable d'environnement CEF_VERSION_MIN
 *
 *	Historique :
 *
 *	V07 	: 2014/04/26 : Apres reorganisation dans /DATA/CSA/CATALOG
 *	V10 	: 2015/09/25 : Avant retraitement donnees CODIF
 */

static	int	cef_version_min = 0;


/***************************************************************************************************
 *
 *	Verification existence variable CIS_ROOT
 *	----------------------------------------
 */
t_err	Check_CIS_root (void)
{
	char *		fonction = FNAME ("Check_CIS_root");
	t_err		error = OK;

	if ((cis_root = getenv ("CIS_ROOT")) == NULL) {

		Affiche_erreur (fonction, "Variable CIS_ROOT non positionnee");
		error = ERR_getenv;
		goto EXIT;
	}

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Verification existence variable CAA_ROOT
 *	----------------------------------------
 */
t_err	Check_CAA_root (void)
{
	char *		fonction = FNAME ("Check_CAA_root");
	t_err		error = OK;

	if ((caa_root = getenv ("CAA_ROOT")) == NULL) {

		Affiche_erreur (fonction, "Variable CAA_ROOT non positionnee");
		error = ERR_getenv;
		goto EXIT;
	}

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Verification existence variable CAA_ROOT
 *	----------------------------------------
 */
t_err	Check_CSA_root (void)
{
	char *		fonction = FNAME ("Check_CSA_root");
	t_err		error = OK;
	char *		var = NULL;

	if ((csa_root = getenv ("CSA_ROOT")) == NULL) {

		Affiche_erreur (fonction, "Variable CAA_ROOT non positionnee");
		error = ERR_getenv;
		goto EXIT;
	}

	if ((var = getenv ("CEF_VERSION_MIN")) == NULL) {

		Affiche_erreur (fonction, "Variable CEF_VERSION_MIN non positionnee");
		error = ERR_getenv;
		goto EXIT;
	}

	if (sscanf (var, "V%d", & cef_version_min) != 1) {

		Affiche_erreur (fonction, "Format variable CEF_VERSION_MIN incorrect");
		error = ERR_getenv;
		goto EXIT;
	}
EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Accesseur aux variables d'environnement
 *	---------------------------------------
 */
const char * Get_CIS_root (void)
{
	return cis_root;
}

const char * Get_CAA_root (void)
{
	return caa_root;
}

const char * Get_CSA_root (void)
{
	return csa_root;
}


/***************************************************************************************************
 *
 *	Retourne experience d'un dataset
 *	--------------------------------
 */
t_err	Get_experiment (char * dataset, char * experiment)
{
	char * 		fonction = FNAME ("Get_experiment");
	t_err		error = OK;
	int		count;

	count = sscanf (dataset, "C%*d_C%*[PQ]_%[^_]s_", experiment);

	if (count != 1) {

		Affiche_erreur (fonction, "Dataset unvalid : %s", dataset);
		error = ERROR;
		strcpy (experiment, "");
		goto EXIT;
	}

EXIT:	return error;	
}


/***************************************************************************************************
 *
 *	Retourne repertoire correspondant a un dataset archive sous CSA_ROOT
 *	--------------------------------------------------------------------
 */
t_err	Get_CEF_directory (char * dataset, t_filename directory)
{
	char *		fonction = FNAME ("Get_CEF_directory");
	t_err		error = OK;
	t_filename	experiment;
	int		r;

	strcpy (directory, "");

	if (Erreur (error = Get_experiment (dataset, experiment))) goto EXIT;

	r = snprintf (directory, FILENAME_LENGTH, "%s/%s/%s", csa_root, experiment, dataset);

	assert (r < FILENAME_LENGTH);	

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Retourne chemin d'acces d'un fichier CEF archive sous CSA_ROOT
 *	--------------------------------------------------------------
 *
 *	Format : <csa_root>/<experiment>/<dataset>/<dataset>__<yyyymmdd>_*_V*.cef[.gz]
 */
t_err	Get_CEF_filename (char * dataset, t_date date, t_filename filename)
{
	char *		fonction = FNAME ("Get_CEF_filename");
	t_err		error = OK;
	t_filename 	directory;
	char		amj [15];
	t_filename	pattern;
	int		r, count;

	strcpy (filename, "");

	if (Erreur (error = Get_CEF_directory (dataset, directory))) goto EXIT;

	sprintf (amj, "%04d%02d%02d", date.annee, date.mois, date.jour);

	r = snprintf (pattern, FILENAME_LENGTH, "%s/%s__%s*_V*.cef*", directory, dataset, amj);

	assert (r < FILENAME_LENGTH);

	if (Erreur (error = Search_files (pattern, & count))) goto EXIT;

	if (count > 0) 
		strcpy (filename, Get_filename (count -1));
	else {
		Affiche_erreur (fonction, "%s : not found", pattern);	
		error = ERR_fopen;
	}

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Creation si necessaire du repertoire correspondant au dataset dans CAA_ROOT
 *	---------------------------------------------------------------------------
 */
t_err	Create_CEF_directory (char * dataset)
{
	char *		fonction = FNAME ("Create_CEF_directory");
	t_err		error = OK;
	t_filename 	experiment;
	t_filename	directory;
	mode_t		mode = 0755;
	int		r;

	if (Erreur (error = Get_experiment  (dataset, experiment))) goto EXIT;

	r = snprintf (directory, FILENAME_LENGTH, "%s/%s/%s", caa_root, experiment, dataset);

	assert (r < FILENAME_LENGTH);

#if defined (_WIN32)

	if (mkdir (directory) == -1 && errno != EEXIST) {
#else

	if (mkdir (directory, mode) == -1 && errno != EEXIST) {
#endif

		Affiche_erreur (fonction, "Creation repertoire %s impossible", directory);
		Affiche_erreur (fonction, "Erreur %d : %s", errno, strerror (errno));
		error = ERR_fopen;
		goto EXIT;
	}

	if (errno != EEXIST) 
		Affiche_trace (1, fonction, "Creation repertoire %s", directory);

EXIT:	return error;
}



/***************************************************************************************************
 *
 *	Retourne numero de version pour un nouveau fichier CEF
 *	------------------------------------------------------
 */
t_err	Get_CEF_version (char * dataset, t_date date, int * version)
{
	char *		fonction = FNAME ("Get_CEF_version");
	t_err		error = OK;
	FILE *		entree = NULL;
	t_filename	catalog;
	char		buffer [200];
	t_filename	pattern;
	int		r;

	* version = 0;

	r = snprintf (catalog, FILENAME_LENGTH, "%s/CATALOG/%s.cat", csa_root, dataset);

	assert (r < FILENAME_LENGTH);

	if ((entree = fopen (catalog, "r")) == NULL) {

		Affiche_erreur (fonction, "Lecture %s impossible", catalog);
		error = ERR_fopen;
		goto EXIT;
	}

	Affiche_trace (1, fonction, "Lecture %s", catalog);

	r = snprintf (pattern, FILENAME_LENGTH, "%%*s %s__%04d%02d%02d_V%%2d.cef", 
		dataset,
		date.annee, date.mois, date.jour);

	assert (r < FILENAME_LENGTH);

	while (Read_line (buffer, entree) == OK) {

		int	ver;

		if (sscanf (buffer, pattern, & ver) == 1) {
	
			if (ver > * version) * version = ver;

			Affiche_trace (3, fonction, "%s", buffer);
		}
	}

	* version += 1;

	if (* version < cef_version_min) *version = cef_version_min;

	Affiche_trace (1, fonction, "Version = %02d", * version);

EXIT:	if (entree != NULL) fclose (entree);
	return error;
}


/***************************************************************************************************
 *
 *	Retourne chemin d'acces d'un nouveau fichier CEF cree sous CAA_ROOT
 *	-------------------------------------------------------------------
 *
 *	Format : <caa_root>/<experiment>/<dataset>/<dataset>__<yyyymmdd>_V<nn>.cef
 */
t_err	New_CEF_filename (char * dataset, t_date date, t_filename filename)
{
	t_err		error = OK;
	int		version;
	char		amj [10];
	t_filename	experiment;
	int		r;

	strcpy (filename, "");

	if (Erreur (error = Get_CEF_version (dataset, date, & version))) {

		error = ERROR;
		goto EXIT;
	}

	if (Erreur (error = Get_experiment (dataset, experiment))) goto EXIT;

	sprintf (amj, "%04d%02d%02d", date.annee, date.mois, date.jour);

	r = snprintf (filename, FILENAME_LENGTH, "%s/%s/%s/%s__%s_V%02d.cef", 
		caa_root, 
		experiment, 
		dataset, 
		dataset, 
		amj, 
		version);

	assert (r < FILENAME_LENGTH);

EXIT:	return error;
}
