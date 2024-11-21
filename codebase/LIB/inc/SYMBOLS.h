/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: SYMBOLS.h,v $
 *
 *	Version	: $Revision: 1.6 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2005/09/09 07:35:44 $
 *
 *	==========================================================================================
 *
 *	Module regroupant la gestion de tables de symboles
 */

#ifndef	__SYMBOLS_H
#define	__SYMBOLS_H


#define	NB_CIS_MODE	16				/* Number of CIS operational modes	*/

#define	NB_TLM_PRODUCT	64				/* Number of CIS telemetry products	*/

#define	NB_SENSITIVITY	2				/* Number of CIS sensitivity		*/

#define	NB_INSTR	2				/* Number of CIS instruments		*/


/*	Definition de quelques types enumeres
 *	-------------------------------------
 */
typedef	enum { LS = 0, HS = 1 }		t_sensitivity;

typedef	enum { CIS_1 = 0, CIS_2 }	t_instr;


/*	Definition du type t_symbol
 *	---------------------------
 */
typedef struct {		/* Entree d'une table de symboles	*/

	int	val;		/* - indice				*/
	char *	key;		/* - cle de recherche			*/
	char *	lib;		/* - libelle				*/

}	t_symbol;


/*	Declaration de quelques tables de symboles particuliers
 *	-------------------------------------------------------
 */
extern	t_symbol	cis_instruments [];		/* Table of CIS instrument's name	*/

extern	t_symbol	cis_alias [];			/* Table of CIS aliases			*/

extern	t_symbol	cis_flight_models [];		/* Table of CIS flight modeles		*/

extern	t_symbol	cis_sensitivity [];		/* Table of CIS sensitivity		*/

extern	t_symbol	cis_operational_mode [];	/* Table of CIS operational modes	*/

extern	t_symbol	cis_telemetry_rate [];		/* Table of CIS telemetry rate		*/


/*	Affiche le contenu d'un table de symboles
 *	-----------------------------------------
 */
t_err	Display_symbol_table (t_symbol *, char * title);


/*	Retourne libelle en fonction sa cle
 *	-----------------------------------
 */
char *	Lib_from_key (t_symbol * table, char * key); 


/*	Retourne libelle en fonction de sa valeur
 *	-----------------------------------------
 */
char *	Lib_from_val (t_symbol * table, int value);


/*	Retourne valeur en fonction de sa cle
 *	-------------------------------------
 */
int	Val_from_key (t_symbol * table, char * key);


/*	Retourne cle en fonction de sa valeur
 *	--------------------------------------
 */
char *	Key_from_val (t_symbol * table, int value);

#endif
