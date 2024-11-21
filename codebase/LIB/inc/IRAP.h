/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: IRAP.h,v $
 *
 *	Version	: $Revision: 1.3 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2015/05/04 16:03:19 $
 *
 *	==========================================================================================
 *
 *	Regroupement des outils propres au fonctionnement IRAP
 *
 *	- Definition des variables CIS_ROOT, CAA_ROOT, CSA_ROOT
 *	- Location des jeux de donnees CEF
 */

#ifndef	__IRAP_H
#define	__IRAP_H


/***
 *	Verification existence variable CIS_ROOT
 */
t_err	Check_CIS_root (void);


/***
 *	Verification existence variable CAA_ROOT
 */
t_err	Check_CAA_root (void);


/***
 *	Verification existence variable CSA_ROOT
 */
t_err	Check_CSA_root (void);


/***
 *	Accesseur aux variables d'environnement
 */
const char * Get_CIS_root (void);

const char * Get_CAA_root (void);

const char * Get_CSA_root (void);


/***
 *	Retourne experience d'un dataset
 */
t_err	Get_experiment (char * dataset, char * experiment);


/***
 *	Retourne repertoire d'un dataset archive sous CSA_ROOT
 */
t_err	Get_CEF_directory (char * dataset, t_filename directory);


/***
 *	Retourne chemin d'acces d'un fichier CEF archive sous CSA_ROOT
 *
 *	Format : <csa_root>/<experiment>/<dataset>/<dataset>__<yyyymmdd>_*_V*.cef[.gz]
 */
t_err	Get_CEF_filename (char * dataset, t_date date, t_filename filename);


/***
 *	Creation si necessaire du repertoire correspondant au dataset
 */
t_err	Create_CEF_directory (char * dataset);


/***
 *	Retourne numero de version pour un nouveau fichier CEF
 */
t_err	Get_CEF_version (char * dataset, t_date date, int * version);


#endif
