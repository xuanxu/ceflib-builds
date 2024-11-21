/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: LISTS.c,v $
 *
 *	Version	: $Revision: 1.5 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2017/06/27 10:46:57 $
 *
 *	==========================================================================================
 *
 *	Module regroupant la gestion de listes de chaines
 */

#define	MODULE_NAME	"LISTS"

#include "CISLIB.h"

#define	INITIAL_SIZE	20


/***************************************************************************************************
 *
 *	Augmente la taille de la liste de 50 %
 *	--------------------------------------
 */
static	t_err	Increase_list_size (t_list * list)
{
	char *		fonction = FNAME ("Increase_list_size");
	t_err		error = OK;
	int		new_size = (int) list->max_size * 1.5;
	int		i;

	if ((list->item = realloc (list->item, new_size * sizeof (char *))) == NULL) {

		Affiche_erreur (fonction, "Impossible reallouer %d elements", list->max_size);
		error = ERR_malloc;
		goto EXIT;
	}

	for (i = list->max_size; i < new_size; i++) list->item [i] = (char *) NULL;

	list->max_size = new_size;

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Creation d'une liste de chaines
 *	-------------------------------
 */
t_list * Create_list (void)
{
	char *		fonction = FNAME ("Create_list");
	t_list *	list = NULL;
	int		i;

	if ((list = malloc (sizeof (t_list))) == NULL) {

		Affiche_erreur (fonction, "Allocation memoire impossible");
		goto EXIT;
	}

	if ((list->item = calloc (INITIAL_SIZE, sizeof (char *))) == NULL) {

		Affiche_erreur (fonction, "Allocation memoire impossible");
		free (list);
		list = NULL;
		goto EXIT;
	}

	list->max_size	= INITIAL_SIZE;
	list->size	= 0;

	for (i = 0; i < INITIAL_SIZE; i++) list->item [i] = (char *) NULL;

EXIT:	return list;
}


/***************************************************************************************************
 *
 *	Ajout d'un element en fin de liste
 *	----------------------------------
 */
t_err	Add_item (t_list * list, char * item)
{
	char *		fonction = FNAME ("Add_item");
	t_err		error = OK;
	char *		str;

	if (list->size + 1 >= list->max_size) {

		if (Erreur (error = Increase_list_size (list))) goto EXIT;
	}

	if ((str = (char *) strdup (item)) == NULL) {

		Affiche_erreur (fonction, "Erreur strdup ()");
		error = ERR_malloc;
		goto EXIT;
	}

	list->item [list->size] = str;
	
	list->size ++;

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Retourne nombre d'elements
 *	--------------------------
 */
int	Get_size (t_list * list)
{
	return list->size;
}


/***************************************************************************************************
 *
 *	Acces a l'element de rang specifie
 *	----------------------------------
 */
char *	Get_item (t_list * list, int pos)
{
	char *		fonction = FNAME ("Get_item");
	t_err		error = OK;

	if (pos >= list->size) {	

		Affiche_erreur (fonction, "Acces element %d impossible", pos);
		return (char *) NULL;
	}
	return (char *) list->item [pos];
}


/***************************************************************************************************
 *
 *	Modifie l'element de rang specifie
 *	----------------------------------
 */
t_err	Replace_item (t_list * list, int pos, char * str)
{
	char *		fonction = FNAME ("Replace_item");
	t_err		error = OK;

	if (pos >= list->size) {	

		Affiche_erreur (fonction, "Acces element %d impossible", pos);
		error = ERROR;
		goto EXIT;
	}

	free (list->item [pos]);

	list->item [pos] = (char *) strdup (str);

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Suppression des elements de la liste
 *	------------------------------------
 */
void 	Clear_list (t_list * list)
{
	char *		fonction = FNAME ("Clear_list");
	int		i;

	for (i = 0; i < list->size; i++) {

		free (list->item [i]);
		list->item [i] = NULL;
	}

	list->size = 0;
}


/***************************************************************************************************
 *
 *	Liberation ressources allouees
 *	------------------------------
 */
void 	Delete_list (t_list * list)
{
	Clear_list (list);

	free (list);
}


/***************************************************************************************************
 *
 * 	Retourne la liste des valeurs de la chaine
 * 	------------------------------------------
 */
char *	Join_list (t_list * list, char * sep)
{
	char *		fonction = FNAME ("Join_list");
	int		i, size;
	char *		buffer = NULL;

	size = 0;

	for (i = 0; i < list->size; i++) 
		size += strlen (list->item[i]);

	size += (list->size -1) * strlen (sep);


	if ((buffer = malloc (size + 1)) == NULL) {

		Affiche_erreur (fonction, "Dynamic allocation failure");
		goto EXIT;
	}

	strcpy (buffer, list->item[0]);

	for (i = 1; i < list->size; i++) {	

		strcat (buffer, sep);
		strcat (buffer, list->item[i]);
	}

EXIT:	return buffer;
}
