/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: ATTRIBUTE.h,v $
 *
 *	Version	: $Revision: 1.2 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2009/04/17 23:20:16 $
 *
 *	==========================================================================================
 *
 *	Module de gestion des attributs de variables ou globaux
 */

#ifndef	__ATTRIBUTE_H
#define __ATTRIBUTE_H

typedef	struct {			/* Attribute entry	*/

	char * 		key;		/* - attribute name	*/
	t_list *	values;		/* - attribute values	*/

} 	t_attr_entry;

typedef	struct {			/* Attribute table		*/

	int		count;		/* - number of attribute	*/
	int		size;		/* - size of table		*/
	t_attr_entry *	data;		/* - entries			*/

}	t_attr;


/***
 *	Initialize data structure for attributes
 */
t_attr * Create_attr_table (void);


/***
 *	Clear attributes and release ressources
 */
void	Delete_attr_table (t_attr * ptr);


/***
 *	Display attribute's table
 */
void	Display_attr_table (t_attr * ptr, char * title);


/***
 *	Add attribute (key, list of values) to attribute's table
 */
t_err	Set_attr (t_attr * ptr, char * key, t_list * values);


/***
 *	Add attribute (key, value) to attribute's table
 */
t_err	Add_attr (t_attr * ptr, char * key, char * value);


/***
 *	Get attribute's values from key
 */
t_list * Get_attr (t_attr * ptr, char * key);


/***
 *	Get attribute's value if unique
 */
char *	Get_attr_value (t_attr * ptr, char * key);


#endif
