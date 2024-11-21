/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: TIMES.h,v $
 *
 *	Version	: $Revision: 1.6 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2014/04/18 12:10:05 $
 *
 *	==========================================================================================
 *
 *	Ce module fournit un ensemble de fonctions permettant la gestion des dates
 *	et les conversions entre les divers formats utilises.
 */

#ifndef __TIMES_H
#define __TIMES_H

extern const double	MILLI_PER_DAY;		/* Nombre milli-secondes par jour		*/

extern const double	MILLI_TO_1958;		/* Nombre milli-sec du 01-01-0000 au 01-01-1958	*/

typedef struct {				/* Structure representant une date		*/

	int	jour;
	int	mois;
	int	annee;

}	t_date;

typedef struct {				/* Structure representant heure du jour		*/

	int	heure;
	int	minute;
	int	seconde;
	double	milli;

}	t_time;

typedef	struct {				/* CCSDS Day Segmented Code			*/

	byte_t		pfield;			/* - preamble field				*/
	uint16_t	day;			/* - number of days from january 1 1958		*/
	uint32_t	milli;			/* - number of milli seconds from 00:00		*/

}	t_CDS_time;


/*	Constructeur date
 *	-----------------
 */
t_date 	Create_date (int annee, int mois, int jour);


/*	Constructeur time
 * 	-----------------
 */
t_time	Create_time (int heure, int minute, int seconde, double milli);


/*	Calcule duree d'une annee
 *	-------------------------
 */
int	Duree_annee (int annee);


/*	Calcule la duree du mois pour une annee donnee
 *	----------------------------------------------
 */
int	Duree_mois (int mois, int annee);


/*	Retourne un pointeur sur une date, calculee a partir du nombre de jours depuis la date de reference
 *	---------------------------------------------------------------------------------------------------
 */
t_date * Julian_to_date (long nbre_jour);


/* 	Renvoie le nombre de jours depuis la date de reference jusqu'a la date specifiee
 *	--------------------------------------------------------------------------------
 */
long	Date_to_julian (t_date date);


/* 	Renvoie le nombre de jours dans l'annee depuis le 1er janvier de la date specifiee
 *	----------------------------------------------------------------------------------
 */
int	Jour_annee (t_date date);


/*	Calcule une date a partir du nombre de jours depuis le 1er janvier de l'annee specifiee
 *	---------------------------------------------------------------------------------------
 */
int	Jour_annee_to_Date (int jour, int annee, t_date * date);


/* 	Retourne un pointeur sur un time, a partir du nombre de milli-secondes depuis 0H
 *	--------------------------------------------------------------------------------
 */
t_time * Milli_to_time (double milli);


/*	Renvoie le nombre de milli secondes depuis 0H jusqu'a l'heure specifiee
 *	-----------------------------------------------------------------------
 */
double	Time_to_milli (t_time time);


/*	Convertit temps au format CCSDS Day Segmented (CDS) en milli-secondes depuis la date de reference
 *	-------------------------------------------------------------------------------------------------
 */
double	CDS_time_to_milli (long cds_day, long cds_milli);


/*	Convertit temps en milli-secondes au format CCSDS Day Segmented (CDS)
 *	---------------------------------------------------------------------
 */
void	Milli_to_CDS_time (double milli, t_CDS_time * cds_time);


/*	Convertit la date et l'heure en milli-secondes depuis la date de reference
 *	--------------------------------------------------------------------------
 */
void	Date_time_to_milli (t_date date, t_time time, double * milli);


/*	Convertit le nombre de milli-secondes depuis la date de reference en une date et heure
 *	--------------------------------------------------------------------------------------
 */
void	Milli_to_date_time (double milli, t_date * date, t_time * time);


/*	Convertit un temps au format Ascii en milli-secondes depuis la date de reference
 *	--------------------------------------------------------------------------------
 */
double	ISOTIME_to_milli (char * str);

#define	Ascii_time_to_milli(str)	ISOTIME_to_milli(str)
#define	Ascii_time_milli_to_milli(str)	ISOTIME_to_milli(str)


/*	Convertit un temps en milli-secondes depuis la date de reference au format Ascii
 *	--------------------------------------------------------------------------------
 */
char *	Milli_to_ISOTIME (double milli, int resolution);

#define	Milli_to_Ascii_time(m)		Milli_to_ISOTIME(m,0)
#define	Milli_to_Ascii_time_milli(m)	Milli_to_ISOTIME(m,3)


/*	Convertit un temps en milli-secondes depuis la date de reference au format Epoch
 *	--------------------------------------------------------------------------------
 */
double	Milli_to_Epoch (double milli);


/*	Retourne epoch time en fonction de la demi periode d'acquisition
 *	----------------------------------------------------------------
 */
double	Epoch_time (double date_ms, double periode);


/*	Retourne le nombre de milli-secondes depuis 1958, a partir de Epoch
 *	-------------------------------------------------------------------
 */
double	Epoch_time_to_milli (double epoch_time);


/*	Retourne la date et l'heure courante
 *	------------------------------------
 */
void	Get_date_time (t_date * date, t_time * time);


/*	Retourne le nombre de milli-secondes depuis la date de reference
 *	----------------------------------------------------------------
 */
double	Maintenant (void);


/*	Verifie le format d'une date donnee sous la forme d'une chaine de caracteres
 *	----------------------------------------------------------------------------
 *
 *	format :	j/m/a 		a/m/j
 *			j-m-a		a-m-j
 *			aaaa/mm/jj	jj/mm/aaaa
 *			aaaa-mm-jj	jj-mm-aaaa
 *			aaaammjj	jjmmaaaa
 *
 *	str_date :	chaine contenant la date a verifier
 *
 *	ptr_date :	adresse variable t_date lue si format correct, ou NULL si inutile
 */
t_err	Check_date (char * format, char * str_date, t_date * ptr_date);


#endif
