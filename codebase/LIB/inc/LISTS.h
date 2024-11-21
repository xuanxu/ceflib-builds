/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: LISTS.h,v $
 *
 *	Version	: $Revision: 1.3 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2014/02/19 14:47:21 $
 *
 *	==========================================================================================
 *
 *	Module regroupant la gestion de listes de chaines
 */

#ifndef	__LISTS_H
#define	__LISTS_H

typedef struct {

	int		max_size;
	int		size;
	char **		item;

}	t_list;


/*	Creation d'une liste de chaines
 *	-------------------------------
 */
t_list * Create_list (void);


/*	Retourne nombre d'elements
 *	--------------------------
 */
int	Get_size (t_list * list);


/*	Ajout d'une chaine dans la liste
 *	--------------------------------
 */
t_err	Add_item (t_list * list, char * item);


/*	Acces a l'element de rang specifie
 *	----------------------------------
 */
char *	Get_item (t_list * list, int pos);


/*	Modification element de rang specifie
 *	-------------------------------------
 */
t_err	Replace_item (t_list * list, int pos, char * str);


/*	Suppression du contenu de la liste
 *	----------------------------------
 */
void 	Clear_list (t_list *);


/*	Destruction de la liste
 *	-----------------------
 */
void 	Delete_list (t_list *);


/*	Retourne une chaine contenant la liste des valeurs
 *	--------------------------------------------------
 */
char *	Join_list (t_list *, char * sep);

#endif
