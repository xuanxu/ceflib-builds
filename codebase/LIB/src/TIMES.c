/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: TIMES.c,v $
 *
 *	Version	: $Revision: 1.21 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2017/06/27 09:37:17 $
 *
 *	==========================================================================================
 *
 *	Ce module fournit un ensemble de fonctions permettant la gestion des dates
 *	et les conversions entre les divers formats utilises.
 */

#define	MODULE_NAME	"TIMES"

#include "CISLIB.h"

#define	IS_DIGIT(c) ((c >= '0') && (c <= '9'))

const	double		MILLI_PER_DAY	= 86400000.0;

const	double		MILLI_TO_1958	= 715145.0 * 86400000.0;

static	const t_date	reference	= { 1, 1, 1958 };

/*	Donnees mise en cache pour optimiser les calculs sur la date
 */
static	t_date		s_date = { -1, -1, -1 };
static	long		s_julian = -1;

typedef struct {				/* Structure definissant un format de date		*/

	char *		externe;		/* - format externe utilise pour appeler la fonction	*/
	int		taille;			/* - taille a verifier ou 0 si taille variable		*/
	char *		interne;		/* - format interne utilise pour la lecture par scanf	*/

}	t_format_date;

static	t_format_date	format_date [] = {

	{ "a/m/j",	 0, "%d/%d/%d",		},
	{ "a-m-j",	 0, "%d-%d-%d"		},
	{ "j/m/a",	 0, "%3$d/%2$d/%1$d"	},
	{ "j-m-a",	 0, "%3$d-%2$d-%1$d"	},
	{ "aaaammjj",	 8, "%4d%2d%2d",	},
	{ "jjmmaaaa",	 8, "%3$2d%2$2d%1$4d"	},
	{ "aaaa/mm/jj",	10, "%4d/%2d/%2d",	},
	{ "aaaa-mm-jj",	10, "%4d-%2d-%2d",	},
	{ "jj/mm/aaaa",	10, "%3$2d/%2$2d/%1$4d"	},
	{ "jj-mm-aaaa",	10, "%3$2d-%2$2d-%1$4d"	},
	{ NULL,		 0, NULL }
};

static	char *		isotime_format [] = {

	"%04d-%02d-%02dT%02d:%02d:%02.0lfZ",	/* Seconds resolution */
	"%04d-%02d-%02dT%02d:%02d:%04.1lfZ",
	"%04d-%02d-%02dT%02d:%02d:%05.2lfZ",
	"%04d-%02d-%02dT%02d:%02d:%06.3lfZ",	/* Milli-seconds resolution */
	"%04d-%02d-%02dT%02d:%02d:%07.4lfZ",
	"%04d-%02d-%02dT%02d:%02d:%08.5lfZ",
	"%04d-%02d-%02dT%02d:%02d:%09.6lfZ",	/* Micro-seconds resolution */
};

#if defined (_WIN32)

#include <sys/timeb.h>

/***************************************************************************************************
 *
 *	Simulation gettimeofday() pour Windows
 *	--------------------------------------
 */
int	gettimeofday (struct timeval *tp, void *tz)
{
	struct	_timeb	timebuffer;

	_ftime (& timebuffer);
	tp->tv_sec  = timebuffer.time;
	tp->tv_usec = timebuffer.millitm * 1000;

	return 0;
}

#endif


/***************************************************************************************************
 *
 * 	Constructeur structure t_date
 *	-----------------------------
 */
t_date	Create_date (int annee, int mois, int jour)
{
	t_date	d;

	d.annee = annee;
	d.mois = mois;
	d.jour = jour;

	return d;
}


/***************************************************************************************************
 *
 * 	Constructeur structure t_time
 *	-----------------------------
 */
t_time	Create_time (int heure, int minute, int seconde, double milli)
{
	t_time	t;

	t.heure = heure;
	t.minute = minute;
	t.seconde = seconde;
	t.milli = milli;

	return t;
}


/***************************************************************************************************
 *
 *	Calcule duree d'une annee
 *	-------------------------
 */
int	Duree_annee (int annee)
{
	if ((annee % 400) == 0)	return 366;
	if ((annee % 100) == 0)	return 365;
	if ((annee %   4) == 0)	return 366;

	return 365;
}


/***************************************************************************************************
 *
 *	Calcule la duree du mois pour une annee donnee
 *	----------------------------------------------
 */
int	Duree_mois (int mois, int annee)
{
	char *		fonction = FNAME ("Duree_mois");
	int		duree [13] = { 0, 31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if (mois == 2) 			return 28 + Duree_annee (annee) - 365;
	if (mois > 0 && mois < 13)	return duree [mois];

	Affiche_erreur (fonction, "ERREUR dans le calcul du mois No %d\n", mois);

	return -1;
}


/***************************************************************************************************
 *
 *	Calcule la date du jour depuis la date de reference
 *	---------------------------------------------------
 */
t_date * Julian_to_date (long nbre_jour)
{
	static t_date	date;
	int		annee = reference.annee;
	int		mois  = reference.mois;
	int		jour  = reference.jour;

	nbre_jour += jour;

	while (nbre_jour > (long) Duree_annee (annee)) {

		nbre_jour -= Duree_annee (annee);
		annee++;
	}

	while (nbre_jour > (long) Duree_mois (mois, annee)) {

		nbre_jour -= Duree_mois (mois++, annee);

		if (mois > 12) {

			mois = 1;
			annee++;
		}
	}
	date.jour 	= nbre_jour;
	date.mois	= mois;
	date.annee	= annee;

	return & date;
}


/***************************************************************************************************
 *
 *	Renvoie le nombre de jours depuis la date de reference
 *	------------------------------------------------------
 */
long	Date_to_julian (t_date date)
{
	char *		fonction = FNAME ("Date_to_julian");
	long		julian	= 0L;
	int		annee = reference.annee;
	int		mois  = reference.mois;
	int		jour  = reference.jour;

	if (date.annee < reference.annee) {

		Affiche_erreur (fonction, "Erreur: Annee %d < %d\n", date.annee, reference.annee);
		exit (1);
	}
	while (annee < date.annee)	julian += Duree_annee (annee++);
	while (mois  < date.mois)	julian += Duree_mois  (mois++, annee);
	julian += (date.jour - jour);

	return julian;
}


/***************************************************************************************************
 *
 *	Renvoie le nombre de jours depuis le 1er janvier
 *	------------------------------------------------
 */
int	Jour_annee (t_date date)
{
	int		mois;
	int		total = 0;

	for (mois = 1; mois < date.mois; mois++)
		total += Duree_mois (mois, date.annee);

	total += date.jour;

	return total;
}


/***************************************************************************************************
 *
 *	Calcule une date donnee en jour, annee
 *	--------------------------------------
 */
int	Jour_annee_to_Date (int jour, int annee, t_date * date)
{
	char *		fonction = FNAME ("Jour_annee_to_Date");
	int		code = OK;

	if (jour < 1 || jour > Duree_annee (annee)) {

		Affiche_erreur (fonction, "Date incorrecte : Jour %d annee %d", jour, annee);
		code = ERROR;
		goto EXIT;
	}

	date->annee = annee;
	date->mois  = 1;

	while (jour > 0) {

		if (jour > Duree_mois (date->mois, date->annee)) {

			jour -= Duree_mois (date->mois, date->annee);
			date->mois ++;
		}
		else {	date->jour = jour;
			jour = 0;
		}
	}
EXIT:	return code;
}


/***************************************************************************************************
 *
 *	Calcule l'heure donnee en milli secondes depuis 0H00:00:000
 *	-----------------------------------------------------------
 */
t_time * Milli_to_time (double milli)
{
	static t_time	temps;
	int		nbsec = (int) milli / 1000.0;

	temps.milli	= milli - nbsec * 1000;
	temps.seconde	= nbsec % 60;
	nbsec /= 60;
	temps.minute	= nbsec % 60;
	nbsec /= 60;
	temps.heure	= nbsec % 60;

	return & temps;
}


/***************************************************************************************************
 *
 *	Renvoie le nombre de milli secondes depuis 0H00:00:000 jusqu'a l'heure
 *	----------------------------------------------------------------------
 */
double	Time_to_milli (t_time time)
{
	return (double) ((time.heure * 60 + time.minute) * 60 + time.seconde) * 1000 + time.milli;
}	


/***************************************************************************************************
 *
 *	Convertit temps au format CCSDS Day Segmented (CDS) en milli-secondes depuis la date de reference
 *	-------------------------------------------------------------------------------------------------
 */
double	CDS_time_to_milli (long cds_day, long cds_milli)
{
	double		milli;

	milli = (double) cds_day * MILLI_PER_DAY + (double) cds_milli;

	return milli;
}


/***************************************************************************************************
 *
 *	Convertit temps en milli-secondes au format CCSDS Day Segmented (CDS)
 *	---------------------------------------------------------------------
 *
 *	P_FIELD est defini comme suit :
 *
 *	- bit 0		extension flag				0
 *	- bit 1-3	time code identification		100
 *	- bit 4		epoch identification			0	1958 January 1
 *	- bit 5		length of day segment			0	16 bit day segment
 *	- bit 6-7	length of submillisecond segment	00	submillisecond segment absent
 */
void	Milli_to_CDS_time (double milli, t_CDS_time * cds_time)
{
	t_date		date;
	t_time		time;
	byte_t		P_FIELD = 0x40;

	Milli_to_date_time (milli, & date, & time);

	cds_time->pfield = P_FIELD;
	cds_time->day    = (uint16_t) Date_to_julian (date);
	cds_time->milli  = (uint32_t) Time_to_milli (time);
}


/***************************************************************************************************
 *
 *	Convertit la date et l'heure en milli secondes depuis la date de reference
 *	--------------------------------------------------------------------------
 */
void	Date_time_to_milli (t_date date, t_time time, double * milli)
{
	if (date.annee != s_date.annee || date.mois != s_date.mois || date.jour != s_date.jour) {

		/* Conserver la derniere date traitee en cache */

		s_julian = Date_to_julian (date);

		memcpy (& s_date, & date, sizeof (t_date));
	}

	* milli = (double) s_julian * MILLI_PER_DAY + Time_to_milli (time);
}	


/***************************************************************************************************
 *
 *	Convertit le nombre de milli-secondes depuis la date de reference en une date et heure
 *	--------------------------------------------------------------------------------------
 */
void	Milli_to_date_time (double milli, t_date * date, t_time * time)
{
	long		jours;
	double		ms;

	jours	= (long) floor (milli / MILLI_PER_DAY);
	ms	= milli - (double) jours * MILLI_PER_DAY;

	if (jours != s_julian) { 	/* Conserver la date dans le cache */

		s_julian = jours;
		memcpy (& s_date, Julian_to_date (jours), sizeof (t_date));
	}

	memcpy (date, & s_date, sizeof (t_date));

	memcpy (time, Milli_to_time (ms), sizeof (t_time));
}


/***************************************************************************************************
 *
 *	Conversion "yyyy-mm-ddThh:mm:ss[.mmm]Z" => double
 *	-------------------------------------------------
 */
double	ISOTIME_to_milli (char * str_date)
{
	t_date		date;
	t_time		time;
	char *		ptr = str_date;
	double		milli = 0.0;

	date.annee = date.mois = date.jour = 0;
	time.heure = time.minute = time.seconde = time.milli = 0;

	while (IS_DIGIT (*ptr)) {
		date.annee *= 10;
		date.annee += (*ptr++ - '0');
	}

	if (* ptr ++ != '-') return -1.0;

	while (IS_DIGIT (*ptr)) {
		date.mois *= 10;
		date.mois += (*ptr++ - '0');
	}

	if (* ptr ++ != '-') return -2.0;

	while (IS_DIGIT (*ptr)) {
		date.jour *= 10;
		date.jour += (*ptr++ - '0');
	}

	if (* ptr ++ != 'T') return -3.0;

	while (IS_DIGIT (*ptr)) {
		time.heure *= 10;
		time.heure += (*ptr++ - '0');
	}

	if (* ptr ++ != ':') return -4.0;

	while (IS_DIGIT (*ptr)) {
		time.minute *= 10;
		time.minute += (*ptr++ - '0');
	}

	if (* ptr ++ != ':') return -5.0;

	while (IS_DIGIT (*ptr)) {
		time.seconde *= 10;
		time.seconde += (*ptr++ - '0');
	}

	if (* ptr == '.') {

		float	base = 100.0;

		ptr++;

		while (IS_DIGIT (*ptr)) {

			time.milli += (*ptr++ - '0') * base;
			base *= 0.1;
		}
	}
	if (* ptr != 'Z') return -6.0;

	Date_time_to_milli (date, time, & milli);;

	return milli;
}
	

/***************************************************************************************************
 *
 *	Conversion double => yyyy-mm-ddThh:mm:ss[.mmmmmm]	
 *	-------------------------------------------------
 */
char *	Milli_to_ISOTIME (double milli, int resolution)
{
	static char	buffer [30];
	t_date		date;
	t_time		time;
	double		secs;
	double		factor;

	Milli_to_date_time (milli, & date, & time);

	if (resolution < 0 || resolution > 6) {

		sprintf (buffer, "Unknown ISOTIME format : %d", resolution);
		goto EXIT;
	}

	secs = time.seconde + time.milli / 1000.0 ;

	/*	Correction bug detecte par Tomasz Klotz :
	 *
         *	sprintf ("%02d:%02d:%6.3lf", heure, minute, secs) 
	 *	arrondissait a hh:mm:60.000Z si secs > 59.9995
	 */
	factor = pow (10.0, resolution);

	secs = floor (secs * factor) / factor;

	sprintf (buffer, isotime_format [resolution],
		date.annee, date.mois, date.jour,
		time.heure, time.minute, secs);

EXIT:	return buffer;
}


/***************************************************************************************************
 *
 *	Convertit un temps en milli-secondes depuis la date de reference au format Epoch
 *	--------------------------------------------------------------------------------
 */
double	Milli_to_Epoch (double milli)
{
	return milli + MILLI_TO_1958;
}

 
/***************************************************************************************************
 *
 *	Retourne epoch time en fonction de la demi periode d'acquisition
 *	----------------------------------------------------------------
 */
double	Epoch_time (double date_ms, double periode)
{
	return (double) MILLI_TO_1958 + date_ms + periode / 2.0;
}	


/***************************************************************************************************
 *
 *	Retourne le nombre de milli-secondes depuis 1958, a partir de Epoch
 *	-------------------------------------------------------------------
 */
double	Epoch_time_to_milli (double epoch_time)
{
	return (double) epoch_time - MILLI_TO_1958;
}	


/***************************************************************************************************
 *
 *	Determine la date et l'heure courantes
 *	--------------------------------------
 */
void	Get_date_time (t_date * d, t_time * t)
{
	struct tm *	time_now;
	struct timeval	tv;
	time_t		seconds;

	gettimeofday (& tv, NULL);

	seconds = tv.tv_sec;

	time_now = localtime (& seconds);

	d->annee   = time_now->tm_year + 1900;
	d->mois    = time_now->tm_mon + 1;
	d->jour    = time_now->tm_mday;

	t->heure   = time_now->tm_hour;
	t->minute  = time_now->tm_min;
	t->seconde = time_now->tm_sec;
	t->milli   = tv.tv_usec / 1000.0;
}


/***************************************************************************************************
 *
 *	Retourne le temps courant en milli-secondes depuis la date de reference
 *	-----------------------------------------------------------------------
 */
double	Maintenant (void)
{
	t_date		date;
	t_time		time;
	double		milli;

	Get_date_time (& date, & time);

	Date_time_to_milli (date, time, & milli);

	return milli;
}


/***************************************************************************************************
 *
 *	Verifie le format d'une date
 *	----------------------------
 */
t_err	Check_date (char * format, char * str_date, t_date * ptr_date)
{
	char *		fonction = FNAME ("Check_date");
	t_err		error = OK;
	t_date		date;
	int		trv = -1;
	int		i, lus;
	
	date.annee = reference.annee;
	date.mois  = reference.mois;
	date.jour  = reference.jour;

	for (i = 0; format_date [i].externe != NULL; i++) {

		if (strcasecmp (format, format_date [i].externe) == 0) trv = i;
	}

	if (trv == -1) {

		Affiche_erreur (fonction, "Format de date incorrect : %s", format);

		for (i = 0; format_date [i].externe != NULL; i++)
			Affiche_erreur (fonction, "%s", format_date [i].externe);

		error = ERR_date;
		goto EXIT;
	}

	if (format_date [trv].taille != 0 && strlen (str_date) != format_date [trv].taille) {

		Affiche_erreur (fonction, "Date %s incorrecte : %d caracteres au lieu de %d",
			str_date, strlen (str_date), format_date [trv].taille);
		error = ERR_date;
		goto EXIT;
	}

	lus = sscanf (str_date, format_date [trv].interne, & date.annee, & date.mois, & date.jour);

	if (lus != 3) {

		Affiche_erreur (fonction, "Date %s incorrecte : %d valeurs lues au lieu de 3",
			str_date, lus);
		error = ERR_date;
		goto EXIT;
	}

	if (date.annee < reference.annee) {

		Affiche_erreur (fonction, "Date %s incorrecte : Annee < %d", str_date, reference.annee);
		error = ERR_date;
		goto EXIT;
	}

	if (date.mois < 1 || date.mois > 12) {

		Affiche_erreur (fonction, "Date %s incorrecte : Mois non compris entre 1 et 12", str_date);
		error = ERR_date;
		goto EXIT;
	}

	if (date.jour < 1 || date.jour > Duree_mois (date.mois, date.annee)) {

		Affiche_erreur (fonction, "Date %s incorrecte : Jour non compris entre %d et %d",
			str_date, 1, Duree_mois (date.mois, date.annee));
		error = ERR_date;
		goto EXIT;
	}

	if (ptr_date != NULL) memcpy ((char *) ptr_date, (char *) & date, sizeof (t_date));

EXIT:	return error;
}
