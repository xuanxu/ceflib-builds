/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: ZZLIB.h,v $
 *
 *	Version	: $Revision: 1.2 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2013/09/27 07:53:51 $
 *
 *	==========================================================================================
 *
 *	Module dédie a la gestion des unites physiques utilises pour le CAA
 */

#ifndef	__ZZLIB_H
#define __ZZLIB_H

#include "CISLIB.h"

#define	GZCLOSE	gzclose


/*	test fin de fichier atteinte
 *	----------------------------
 */
int	GZEOF (gzFile file);


/*	Ouverture du fichier en lecture
 *	-------------------------------
 */
gzFile	GZOPEN (char * filename, char * mode);


/*	Lecture d'une ligne dans le buffer
 *	----------------------------------
 */
char *	GZGETS (gzFile file, char * line, unsigned len);


/*	Retour au debut du fichier
 *	--------------------------
 */
int	GZREWIND (gzFile file);

#endif
