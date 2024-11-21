/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: ZZLIB.c,v $
 *
 *	Version	: $Revision: 1.2 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2013/09/27 07:53:51 $
 *
 *	==========================================================================================
 *
 *	Reecriture de certaines fonctions de la ZLIB pour optimisation
 */

#define	MODULE_NAME	"ZZLIB"

#include "ZZLIB.h"

#define	BUF_SIZE	(1 << 20)

static	char	buffer [BUF_SIZE + 1];
static	char *	pos = NULL;
static	char *	end = buffer + BUF_SIZE;
static	int	zip_empty = FALSE;		/* Indicateur si tout a ete lu */
static	int	consumed  = FALSE;		/* Indicateur si tout a ete consomme */


/***************************************************************************************************
 *
 *	Initialisation du buffer de lecture
 *	-----------------------------------
 */
static void GZINIT (void)
{
	char *	fonction = FNAME ("GZINIT");

	Affiche_trace (3, fonction, "Initialisation buffer de %d octets", BUF_SIZE);

	end = buffer + BUF_SIZE;
	pos = end;
	* end = '\n';
	consumed  = FALSE;
	zip_empty = FALSE;
}


/***************************************************************************************************
 *
 *	Test si fin de fichier atteinte 
 *	-------------------------------
 */
int	GZEOF (gzFile file)
{
	return consumed;
}


/***************************************************************************************************
 *
 *	Ouverture du fichier en lecture
 *	-------------------------------
 */
gzFile 	GZOPEN (char * filename, char * mode)
{
	GZINIT ();

	return gzopen (filename, mode);
}


/***************************************************************************************************
 *
 *	Lecture d'une ligne dans le buffer
 *	----------------------------------
 */
char *	GZGETS (gzFile file, char * line, unsigned len)
{
	char *	eos = strchr (pos, '\n');

	if (eos == end) {

		int	offset = (int) (eos - pos);
		int	lus;

		if (zip_empty) {

			consumed = TRUE;
			return NULL;
		}

		strncpy (buffer, pos, offset);
		lus = gzread (file, buffer + offset, BUF_SIZE - offset);

		if (lus < BUF_SIZE - offset) {

			zip_empty = TRUE;
			end = buffer + offset + lus;

			/* Ajouter \n en fin de buffer si necessaire */
			if (lus > 0 && * (end-1) != '\n')  *end++ = '\n';
			*end = '\n';
		}
		pos = buffer;
		eos = strchr (pos, '\n');
	}

	if (eos) * eos = 0;

	/* Handling Windows CR + LF terminator */

	if (eos > buffer && *(eos-1) == '\r') * (eos-1) = 0;

	strcpy (line, pos);

	pos += strlen (pos) + 1;

	return line;
}


/***************************************************************************************************
 *
 *	Retour au debut du fichier
 *	--------------------------
 */
int	GZREWIND (gzFile file)
{
	GZINIT ();
	return gzrewind (file);
}
