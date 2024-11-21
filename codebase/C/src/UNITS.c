/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: UNITS.c,v $
 *
 *	Version	: $Revision: 1.1 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2008/09/26 11:02:06 $
 *
 *	==========================================================================================
 *
 *	Module dedie a la gestion des unites physiques utilisees pour le CAA
 */

#define	MODULE_NAME	"UNITS"

#include "UNITS.h"

#define	ELECTRON_VOLT	1.60217646E-19			/* Valeur d'un eV en Joules */

t_symbol	properties [] = {

	{ RC,	"RC",	"Raw_Particle_Counts"		},
	{ CS,	"CS",	"Corrected_Particle_Count_Rate"	},
	{ PF,	"PF",	"Differential_Particle_Flux"	},
	{ PEF,	"PEF",	"Particle_Energy_Flux"		},
	{ PSD,	"PSD",	"Phase_Space_Density"		},
	{ EOF,	"???",	"Unkown_Physical_Unit"		}
};


/***************************************************************************************************
 *
 *	Retourne la chaine decrivant l'unite physique d'une propriete
 *	-------------------------------------------------------------
 */
char *	Physical_unit (t_property property)
{
	static char	buffer [80];

	switch (property) {

	case RC:	strcpy (buffer, "Counts");
			break;	
	case CS:	strcpy (buffer, "Particles s^-1");
			break;	
	case PF:	strcpy (buffer, "Particles cm^-2 s^-1 sr^-1 Kev^-1");
			break;	
	case PEF:	strcpy (buffer, "Kev cm^-2 s^-1 sr^-1 Kev^-1");
			break;	
	case PSD:	strcpy (buffer, "Particles s^3 km^-6");
			break;	
	default:	strcpy (buffer, "");
	}

	return buffer;
}


/***************************************************************************************************
 *
 *	Retourne la chaine SI_CONVERSION d'une variable property
 *	--------------------------------------------------------
 */
char *	SI_conversion (t_property property)
{
	static char	buffer [80];

	switch (property) {

	case RC:	strcpy (buffer, "1>(count)");
			break;	
	case CS:	strcpy (buffer, "1>(particles) s^-1");
			break;	
	case PF:	sprintf (buffer, "%1.4E>(particles) m^-2 s^-1 sr^-1 J^-1",
				1.0E+4 * 1.0 / ELECTRON_VOLT * 1.0E-3);
			break;	
	case PEF:	strcpy (buffer, "1.0E+4>J m^-2 s^-1 sr^-1 J^-1");
			break;
	case PSD:	strcpy (buffer, "1.0E-18>(particles) s^3 m^-6");
			break;
	default:	strcpy (buffer, "");
	}

	return buffer;
}
