/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: SYMBOLS.c,v $
 *
 *	Version	: $Revision: 1.9 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2005/09/09 07:35:44 $
 *
 *	==========================================================================================
 *
 *	Module regroupant la gestion de tables de symboles
 */

#define	MODULE_NAME	"SYMBOLS"

#include "CISLIB.h"

t_symbol	cis_instruments [] = {

		{ 0,	"COD",	"CODIF"			},
		{ 1,	"HIA",	"HIA"			},
		{ EOF,	"???",	"Unknown instrument"	}
};

t_symbol	cis_alias [] = {

		{ 1,	"C1",	"RUMBA"			},
		{ 2,	"C2",	"SALSA"			},
		{ 3,	"C3",	"SAMBA"			},
		{ 4,	"C4",	"TANGO"			},
		{ EOF,	"???",	"Unknown spacecraft"	}
};
		
t_symbol	cis_flight_models [] = {

		{ 1, 	"FM8",	"Flight Model 8"	},
		{ 2, 	"FM5",	"Flight Model 5"	},
		{ 3, 	"FM6",	"Flight Model 6"	},
		{ 4, 	"FM7",	"Flight Model 7"	},
		{ EOF,	"???",	"Unknown flight Model"	}
};

t_symbol	cis_sensitivity [] = {

		{ LS,	"LS",	"Low-g"			},
		{ HS,	"HS",	"High-G"		},
		{ EOF,	"??",	"Unknwown sensitivity"	}
};

t_symbol	cis_operational_mode [] = {	

		{ 0,	"M00",	"SW-1>Solar Wind-Mode-1"		},
		{ 1,	"M01",	"SW-2>Solar Wind-Upstreams-Mode-2"	},
		{ 2,	"M02",	"SW-3>Solar Wind-Mode-3"		},
		{ 3,	"M03",	"SW-4>Solar Wind-Upstreams-Mode-4"	},
		{ 4,	"M04",	"SW-C1>Solar Wind-Data compression"	},
		{ 5,	"M05",	"SW-C2>Solar Wind-Data compression"	},
		{ 6,	"M06",	"RPA>RPA-Mode"				},
		{ 7,	"M07",	"PROM>PROM Operation"			},
		{ 8,	"M08",	"MAG-1>Magnetospheric-Mode-1"		},
		{ 9,	"M09",	"MAG-2>Magnetospheric-Mode-2"		},
		{ 10,	"M10",	"MAG-3>Magnetospheric-Mode-3"		},
		{ 11,	"M11",	"MAG-5>Magnetosheath-Mode-1"		},
		{ 12,	"M12",	"MAG-1>Magnetosheath-Mode-2"		},
		{ 13,	"M13",	"MAG-C1>Magnetospheric-Data Comp"	},
		{ 14,	"M14",	"MAG-C2>Magnetosheath-Data Comp"	},
		{ 15,	"M15",	"CALIBR>Calibration mode"		},
		{ EOF,	"???",	"Unknown operational mode"		}
};

t_symbol	cis_telemetry_rate [] = {

		{ 0,	"HSK",	"Housekeeping Mode"		},
		{ 1,	"NM1",	"Normal Mode 1"			},
		{ 2,	"NM2",	"Normal Mode 2"			},
		{ 3,	"NM3",	"Normal Mode 3"			},
		{ 4,	"BM1",	"Burst Mode 1"			},
		{ 5,	"BM2",	"Burst Mode 2"			},
		{ 6,	"BM3",	"Burst Mode 3"	 		},
		{ EOF,	"???",	"Unknowm telemetry rate"	}
};


/***************************************************************************************************
 *
 *	Verifie que le symbole existe dans la table
 *	-------------------------------------------
 */
static	int	Unknown_symbol (t_symbol * ptr)
{
	return ptr->val == EOF;
}


/***************************************************************************************************
 *
 *	Fonctions d'acces aux membres d'un symbole
 *	------------------------------------------
 */
static	int	Val (t_symbol * symbol) { return symbol->val; }
static	char *	Key (t_symbol * symbol) { return symbol->key; }
static	char *	Lib (t_symbol * symbol)	{ return symbol->lib; }


/***************************************************************************************************
 *
 *	Cherche l'entree correspondant a la cle dans la table
 *	-----------------------------------------------------
 *
 *	Retourne un pointeur sur le symbole ou sur dernier element si non existant
 */
static	t_symbol * Search_symbol_key (t_symbol * table, char * key)
{
	t_symbol *	ptr;

	for (ptr = table; ptr->val != EOF; ptr ++) if (strcmp (ptr->key, key) == 0) return ptr;

	return ptr;
}


/***************************************************************************************************
 *
 *	Cherche l'entree correspond a la valeur dans la table
 *	-----------------------------------------------------
 *
 *	Retourne un pointeur sur le symbole ou sur dernier element si non existant
 */
static	t_symbol * Search_symbol_value (t_symbol * table, int value)
{
	t_symbol *	ptr;

	for (ptr = table; ptr->val != EOF; ptr ++) if (ptr->val == value) return ptr;

	return ptr;
}



/***************************************************************************************************
 *
 *	Affiche contenu d'un table de symboles
 *	--------------------------------------
 */
t_err	Display_symbol_table (t_symbol * table, char * title)
{
	char *		fonction = FNAME ("Display_symbol_table");
	t_err		error = OK;

	if (title != NULL && strlen (title) > 0) {

		int	len = strlen (title) + 8;
		Affiche_trace (1, fonction, "Table : %s", title);
		Affiche_trace (1, fonction, "%*.*s", len, len, "-----------------------------------------");
	}

	while (table->val != EOF) {

		Affiche_trace (1, fonction, "%3d  %-10.10s %s", table->val, table->key, table->lib);
		table ++;
	}

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Retourne libelle en fonction sa cle
 *	-----------------------------------
 */
char *	Lib_from_key (t_symbol * table, char * key)
{
	return Lib (Search_symbol_key (table, key));
}


/***************************************************************************************************
 *
 *	Retourne libelle en fonction de sa valeur
 *	-----------------------------------------
 */
char *	Lib_from_val (t_symbol * table, int value)
{
	return Lib (Search_symbol_value (table, value));
}



/***************************************************************************************************
 *
 *	Retourne valeur en fonction de sa cle
 *	-------------------------------------
 */
int	Val_from_key (t_symbol * table, char * key)
{
	return Val (Search_symbol_key (table, key));
}


/***************************************************************************************************
 *
 *	Retourne cle en fonction de sa valeur
 *	--------------------------------------
 */
char *	Key_from_val (t_symbol * table, int value)
{
	return Key (Search_symbol_value (table, value));
}
