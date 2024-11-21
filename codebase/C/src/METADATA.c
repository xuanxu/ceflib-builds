/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: METADATA.c,v $
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

#define	MODULE_NAME	"METADATA"

#include "CEFLIB.h"
#include "METADATA.h"

/***************************************************************************************************
 *
 *	Create metadata section
 *	-----------------------
 */
t_meta * Create_meta (char * name)
{
	char *		fonction = FNAME ("Create_meta");
	t_meta *	new = NULL;

	if ((new = malloc (sizeof (t_meta))) == NULL) {

		Affiche_erreur (fonction, "Dynamic allocation failure");
		return (t_meta *) NULL;
	}

	if ((new->entry = Create_list ()) == NULL) {

		Affiche_erreur (fonction, "Dynamic allocation failure");
		return (t_meta *) NULL;
	}

	new->name = strdup (name);
	new->type = strdup ("CHAR");

	return new;
}


/***************************************************************************************************
 *
 *	Delete all metadata
 *	-------------------
 */
void	Delete_meta (t_meta * ptr)
{
	char *		fonction = FNAME ("Delete_meta");
	
	Delete_list (ptr->entry);

	ptr->entry = NULL;

	free (ptr->name);
	free (ptr->type);

	ptr->name = NULL;
	ptr->type = NULL;

	free (ptr);
}
 

/***************************************************************************************************
 *
 *	Set meta type
 *	-------------
 */
t_err	Set_meta_type (t_meta * ptr, char * type)
{
	ptr->type = strdup (type);
}


/***************************************************************************************************
 *
 *	Add meta entry
 *	--------------
 */
t_err	Add_meta_entry (t_meta * ptr, char * entry)
{
	char *		fonction = FNAME ("Add_meta_entry");
	t_err		error = OK;

	error = Add_item (ptr->entry, entry);

EXIT:	return error;	
}
