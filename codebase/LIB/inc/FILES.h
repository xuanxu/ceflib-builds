/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: FILES.h,v $
 *
 *	Version	: $Revision: 1.12 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2014/04/17 13:54:12 $
 *
 *	==========================================================================================
 *
 *	Module regroupant les operations specifiques liees a la manipulation des fichiers
 */

#ifndef	__FILES_H
#define	__FILES_H

#include "SYMBOLS.h"

#define	FILENAME_LENGTH	200

typedef	char		t_filename [FILENAME_LENGTH];


/*	Retourne date des dernieres donnees pour un instrument et un satellite
 *	----------------------------------------------------------------------
 */
double	End_of_mission (int sat, t_instr instr);

/*	Recherche de fichiers avec jokers
 *	---------------------------------
 *
 *	Le masque autorise les jokers *, ? et [ a la maniere de la commande ls
 *
 *	count represente le nombre de fichiers correspondant au masque
 */
t_err	Search_files (char * mask, int * count);


/*	Liberation des ressources allouees par la recherche precedente
 *	--------------------------------------------------------------
 */
void	Search_free (void);


/*	Acces au nom d'un des fichiers de la recherche precedente
 *	---------------------------------------------------------
 *
 *	number doit etre compris entre 0 et count -1, sinon retourne NULL
 */
char *	Get_filename (int number);


/*	Lecture d'un ligne dans un fichier texte
 *	----------------------------------------
 *
 *	- recopie la ligne dans la variable buffer
 *	- suppression du saut de ligne "\n" final
 *	- retrourne OK, WAR_eof ou ERR_fgets suivant les cas
 */
t_err   Read_line (char * buffer, FILE * pf);


/*	Verification nombre valeurs lues par rapport aux valeurs attendues
 *	------------------------------------------------------------------
 */
t_err	Check_read_values (char * fonction, int count, int expected);


/*	Verification nombre valeurs ecrites par rapport aux valeurs attendues
 *	---------------------------------------------------------------------
 */
t_err	Check_write_values (char * fonction, int count, int expected);


/*	Creation repertoire temporaire /tmp/<key>
 *	-----------------------------------------
 */
t_err	Create_tmp_directory (char * key);


/*	Suppression des fichiers *.tmp du repertoire temporaire
 *	-------------------------------------------------------
 */
t_err	Clear_tmp_directory (void);


/*	Suppression des fichiers et du repertoire temporaire
 *	----------------------------------------------------
 */
t_err	Delete_tmp_directory (void);


/*	Ouverture du fichier <key>.tmp dans le repertoire temporaire
 *	------------------------------------------------------------
 */
char * 	Get_tmp_directory (void);


/*	Ouverture du fichier temporaire /tmp/<key>/<prefix>.tmp 
 *	-------------------------------------------------------
 */
FILE *	Open_tmp_file (char * prefix, char * mode);

#endif
