/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: METADATA.h,v $
 *
 *	Version	: $Revision: 1.2 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2008/11/24 16:50:33 $
 *
 *	==========================================================================================
 *
 *	Module de gestion des metadonnees
 */

#ifndef	__METADATA_H
#define	__METADATA_H

#include "CISLIB.h"

typedef	struct {

	char *		name;
	char *		type;
	t_list *	entry;

}	t_meta;

/***
 *	Add a new metadata section
 */
t_meta * Create_meta (char * name);


/***
 *	Delete metadata section
 */
void 	Delete_meta (t_meta * ptr);


/***
 *	Set metadata section type
 */
t_err	Set_meta_type (t_meta * ptr, char * type);


/***
 *	Add metadata section entry
 */
t_err	Add_meta_entry (t_meta * ptr, char * entry);


/***
 *	Get metadata section from name
 */
t_meta * Get_meta (char * name);


#endif
