/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: ATTRIBUTE.c,v $
 *
 *	Version	: $Revision: 1.5 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2009/04/17 23:20:17 $
 *
 *	==========================================================================================
 *
 *	Module de gestion des attributs de variables ou locaux
 */

#define	MODULE_NAME	"ATTRIBUTE"

#include "CEFLIB.h"
#include "ATTRIBUTE.h"

#define	MIN_ENTRIES	10
#define	BUF_SIZE	1024


/***************************************************************************************************
 *
 *	Initialize data structure for attributes
 *	----------------------------------------
 */
t_attr * Create_attr_table (void)
{
	char *		fonction = FNAME ("Create_attr_table");
	t_attr *	ptr = NULL;

	if ((ptr = calloc (sizeof (t_attr), 1)) == NULL) {

		Affiche_erreur (fonction, "Dynamic allocation failure");
		return NULL;
	}

	ptr->count = 0;
	ptr->size  = MIN_ENTRIES;
	ptr->data  = calloc (sizeof (t_attr_entry), ptr->size);

EXIT:	return ptr;
}


/***************************************************************************************************
 *
 *	Clear attribute's table and release ressource
 *	---------------------------------------------
 */
void	Delete_attr_table (t_attr * ptr)
{
	char *		fonction = FNAME ("Delete_attr_table");
	int		i;

	if (ptr != NULL) {

		for (i = 0; i < ptr->count; i++) {

			free (ptr->data [i].key);
			Delete_list (ptr->data [i].values);
		}

		free (ptr->data);

		ptr->count = ptr->size = 0;

		free (ptr);
	}
}


/***************************************************************************************************
 *
 * 	Increase attribute's table size
 *	-------------------------------
 */
static	t_err	Increase_attr_table (t_attr * ptr)
{
	char *		fonction = FNAME ("Increase_attr_table");
	t_err		error = OK;

	ptr->size += (ptr->size / 2);

	ptr->data = realloc (ptr->data, sizeof (t_attr_entry) * ptr->size);

	if (ptr->data == NULL) {

		Affiche_erreur (fonction, "Dynamic allocation failure");
		error = ERR_malloc;
		goto EXIT;
	}

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Add an attribute (key, values) to the table
 *	-------------------------------------------
 */
t_err	Set_attr (t_attr * ptr, char * key, t_list * values)
{
	char *		fonction = FNAME ("Set_attr");
	t_err		error = OK;
	t_list *	list = Create_list ();
	int		i;

	if (ptr->count == ptr->size && Erreur (error = Increase_attr_table (ptr))) goto EXIT;

	for (i = 0; i < Get_size (values); i++)
		if (Erreur (error = Add_item (list, Get_item (values, i)))) goto EXIT;

	ptr->data [ptr->count].key = strdup (key);
	ptr->data [ptr->count].values = list;
	ptr->count ++;

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Add an attribute (key, values) to the table
 *	-------------------------------------------
 */
t_err	Add_attr (t_attr * ptr, char * key, char * value)
{
	char *		fonction = FNAME ("Add_attr");
	t_err		error = OK;
	t_list *	list = Create_list ();

	if (Erreur (error = Add_item (list, value))) goto EXIT;

	if (Erreur (error = Set_attr (ptr, key, list))) goto EXIT;

EXIT:	Delete_list (list);
	return error;
}


/***************************************************************************************************
 *
 *	Get attribute's value from key
 *	------------------------------
 */
t_list * Get_attr (t_attr * ptr, char * key)
{
	int	i;

	for (i = 0; i < ptr->count; i++) { 

		if (strcasecmp (ptr->data [i].key, key) == 0) return ptr->data [i].values;
	}
	return NULL;
}


/***************************************************************************************************
 *
 *	Get attribute's value from key if unique
 *	----------------------------------------
 */
char * 	Get_attr_value (t_attr * ptr, char * key)
{
	static char	buffer [512];
	t_list *	values = Get_attr (ptr, key);

	strcpy (buffer, "");

	if (values != NULL && Get_size (values) == 1)
		strcpy (buffer, Get_item (values, 0));

	return buffer;
}


/***************************************************************************************************
 *
 *	Display attribute's table
 *	-------------------------
 */
void	Display_attr_table (t_attr * ptr, char * title)
{
	char *		fonction = FNAME ("Display_attr_table");
	int		i;

	Affiche_trace (1, fonction, "%s", title);

	for (i = 0; i < ptr->count; i++) {

		char	buffer [BUF_SIZE];
		int	v;

		t_list * values = ptr->data [i].values;

		strcpy (buffer, Get_item (values, 0));

		for (v = 1; v < Get_size (values); v++) {

			sprintf (buffer, "%s, %s", buffer, Get_item (values, v));
		}

		Affiche_trace (1, fonction, "- %s = %s", ptr->data [i].key, buffer);
	}
}
