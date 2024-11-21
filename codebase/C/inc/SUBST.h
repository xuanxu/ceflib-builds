/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: SUBST.h,v $
 *
 *	Version	: $Revision: 1.2 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2017/06/27 13:02:44 $
 *
 *	==========================================================================================
 *
 *	Module permettant la substitution de variables dans un texte
 */

#ifndef	__SUBST_H
#define __SUBST_H

#include "CISLIB.h"

#ifndef _WIN32

/*	Purge la table des associations (cle, libelle)
 *	----------------------------------------------
 */
t_err	Clear_substitutions ();


/*	Affiche la liste des substitutions
 *	----------------------------------
 */
t_err	Display_substitutions (void);


/*	Ajoute une association (cle, valeur) a la table
 *	-----------------------------------------------
 */
t_err	Add_substitution (char * key, char * format, ...);


/*	Recherche et substitution des variables d'une chaine
 *	----------------------------------------------------
 */
t_err	Search_replace (char * input_file, FILE * output_file);

#endif

#endif
