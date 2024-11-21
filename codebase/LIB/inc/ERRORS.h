/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: ERRORS.h,v $
 *
 *	Version	: $Revision: 1.4 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2014/04/17 13:26:20 $
 *
 *	==========================================================================================
 *
 *	Ce module est charge de la gestion des erreurs retournes par les differentes procecures,
 *	ainsi que des messages d'erreur et de trace
 */

#ifndef __ERRORS_H
#define __ERRORS_H

#ifndef	FALSE
#define	FALSE		0			
#endif
#ifndef	TRUE
#define	TRUE		1		
#endif

#define	FNAME(fonction)		MODULE_NAME"."fonction

typedef enum {			/* Liste codes d'erreur retournes par les differents modules	*/

	OK = 0,			/*  0 : Terminaison correcte					*/

	WARNING	= 2,		/* 2 a 99 : Warning (erreur non fatale)				*/
	WAR_esoc,
	WAR_frame,
	WAR_packet,
	WAR_eof,
	WAR_fopen,
	WAR_datation,
	WAR_data,

	ERROR = 102,		/* 102 a 200 : Erreur fatale					*/
	ERR_argument,
	ERR_trace_file,
	ERR_satellite,
	ERR_input_file,
	ERR_parameter_file,
	ERR_expected_file,
	ERR_tmp_directory,
	ERR_generated_file,
	ERR_architecture,
	ERR_getenv,
	ERR_date,
	ERR_esoc,
	ERR_frame,
	ERR_packet,
	ERR_malloc,
	ERR_access,
	ERR_eof,
	ERR_ferror,
	ERR_fopen,
	ERR_fgets,
	ERR_fread,
	ERR_fwrite,
	ERR_fprintf,
	ERR_fseek,
	ERR_glob,
	ERR_datation,
	ERR_data,
	ERR_idl	= 150,		/* 150 et plus : erreur propres au sources IDL			*/
	ERR_postcript,

	END_error		/* Indicateur de fin de la table				*/

}	t_err;



/*	Verification coherence numero satellite contenu dans la chaine
 *	--------------------------------------------------------------
 */
t_err	Check_satellite (char * str, int * satellite);


/*	Positionne le niveau de trace du programme
 *	------------------------------------------
 *
 *	Seuls les messages d'un niveau inferieur ou egal au niveau specifie 
 *	seront affiches dans le fichier de trace
 */
void	Set_trace_level (int valeur);


/*	Lecture niveau de trace courant
 *	-------------------------------
 */
int	Get_trace_level ();


/*	Affiche un message d'erreur 
 *	---------------------------
 *
 *	- Lettre E (Erreur)
 *	- Nom du module et de la fonction,
 *	- texte en clair precisant le type d'erreur rencontree
 */
void	Affiche_erreur (char * fonction, char * format, ...);


/*	Affiche un message de trace
 *	---------------------------
 *
 *	Seuls les messages ou l'indice est inferieur ou egal au niveau de trace
 *	courant seront affiches
 *
 *	- Niveau de trace (1 a N)
 *	- Nom du module et de la fonction,
 *	- Texte en clair precisant le type d'information.
 */
void	Affiche_trace (int indice, char * fonction, char * format, ...);


/*	Teste si le code d'erreur correspond a une erreur fatale
 *	--------------------------------------------------------
 */
int	Erreur (t_err code);


/*	Teste si le code d'erreur correspond a un simple avertissement
 *	--------------------------------------------------------------
 */
int	Warning (t_err code);


/* 	Retourne le mnemonique correspondant au code d'erreur
 *	-----------------------------------------------------
 */
char *	Mnemonique_erreur (t_err code);


/*	Retourne le libelle correspondant au code d'erreur
 *	--------------------------------------------------
 */
char *	Libelle_erreur (t_err code);


/*	Affiche la liste des codes d'erreurs possibles
 *	----------------------------------------------
 *
 *	- Code de l'erreur
 *	- Mnemonique de l'erreur
 *	- Libelle en clair de l'erreur
 */
t_err	Genere_liste_code_erreur (void);

#endif
