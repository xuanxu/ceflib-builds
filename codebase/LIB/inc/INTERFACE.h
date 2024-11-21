/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: INTERFACE.h,v $
 *
 *	Version	: $Revision: 1.5 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2006/10/11 16:15:16 $
 *
 *	==========================================================================================
 *
 *	Module charge de la gestion des fichiers Double Star
 */

#ifndef	__INTERFACE_H
#define	__INTERFACE_H

#include "CISLIB.h"

#define	VMAX		99			/* Maximum version number		*/

typedef	enum {					/* CIS data source			*/

	L1,					/* Level 1 data				*/
	L2,					/* Level 2 data				*/
	L3					/* Level 3 data				*/
}	t_data_source;

extern	t_symbol	cis_data_source [];	/* Table des sources de donnees CIS	*/

typedef	enum {					/* Identificateurs de fichiers CIS	*/

	/* Produits CODIF */

	COD_00, COD_01, COD_02, COD_03, COD_04, COD_05, COD_06, COD_07, COD_09,
	COD_11, COD_12, COD_13, COD_14, COD_15, COD_16, COD_17, COD_18, COD_19,
	COD_20, COD_21, COD_22, COD_23, COD_24, COD_27, COD_28, COD_29,
	COD_32, COD_33, COD_34, COD_35,
	COD_36,	COD_38,
	COD_39, COD_40, COD_41, COD_42, COD_43, COD_44, COD_45,
	COD_46, COD_47, COD_48, COD_49,
	COD_61, COD_62,
	COD_06B,

	/* Produits HIA */

	HIA_00, HIA_01, HIA_02, HIA_04, HIA_05, HIA_06, HIA_07, HIA_08, HIA_09,
	HIA_10, HIA_11, HIA_12, HIA_13, HIA_14, HIA_15, HIA_16, HIA_17, HIA_18, HIA_19,
	HIA_20, HIA_21, HIA_22, HIA_23, HIA_24,
	HIA_61, HIA_62,
	HIA_06B, HIA_17B, HIA_21B, HIA_22B,

	/* Produits Housekeeping */

	HSK,
	HSK_00, HSK_01, HSK_02, HSK_03, HSK_04, HSK_05, 
	HSK_06, HSK_07, HSK_08, HSK_09, HSK_10, HSK_11,

	/* Produits auxiliaires */

	STOF, STEF, SATT, CHDS, COVM,
	HKDSAT, HPDSAT, 
	ENV, CATAL, ARCH, CDROM, TRACE,

}	t_file_ident;

extern	t_symbol	cis_file_ident [];	/* Table des symboles des fichiers */


/*	Ouverture du fichier correspondant a la source et au type specifies
 */
FILE *	Open_CIS_file (t_data_source source, t_file_ident ident, char * mode);


/*	Ouverture si necessaire du fichier correspondant a la source et au type specifies
 */
FILE *	Reopen_CIS_file (t_data_source source, t_file_ident ident, char * mode);


/*	Purge du repertoire correspondant a la source et date
 */
t_err	Clear_directory (t_data_source source, int sat, t_date date);


/*	Redirection sortie standard
 */
t_err	Redirect_stdout (t_data_source source);


/*	Calcul des noms de fichiers des fichiers produits
 */
t_err	Compute_CIS_filenames (t_data_source source, int sat, t_date date, int version);


/*	Retourne le nom du fichier correspondant a la source et au type specifies
 */
char *	Product_filename (t_data_source source, t_file_ident ident);

#endif
