/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: ERRORS.c,v $
 *
 *	Version	: $Revision: 1.6 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2009/02/26 14:35:57 $
 *
 *	==========================================================================================
 *
 *	Ce module est charge de la gestion des erreurs retournes par les differentes procecures,
 *	ainsi que des messages d'erreur et de trace
 */

#define	MODULE_NAME	"ERRORS"

#include "CISLIB.h"

#define	MESSAGE_LENGTH	1024			/* Taille maximale message d'erreur		*/

#define	MODULE_LENGTH	30			/* Taille du nom du module + fonction		*/

static	int		niveau_trace = 1;	/* Valeur par defaut du niveau de trace		*/

static	t_symbol	liste_erreur [] = {

	{ OK,			"OK",			"Terminaison correcte" 					},
	{ WARNING,		"WARNING",		"Erreur non fatale non precisee (avertissement)" 	},
	{ WAR_esoc,		"WAR_esoc",		"Trame ESOC incorrecte"					},
	{ WAR_frame,		"WAR_frame",		"Trame de donnees incorrecte"				},
	{ WAR_packet,		"WAR_packet",		"Paquet de donnees incorrect"				},
	{ WAR_eof,		"WAR_eof",		"Fin de fichier rencontree" 				},
	{ WAR_fopen,		"WAR_fopen",		"Erreur ouverture de fichier en lecture ou ecriture" 	},
	{ WAR_datation,		"WAR_datation",		"Datation incorrecte"					},
	{ WAR_data,		"WAR_data",		"Donnees incorrectes"					},
	{ ERROR,		"ERROR",		"Erreur fatale non precisee" 				},
	{ ERR_argument,		"ERR_argument",		"Arguments ligne de commande incorrects"		},
	{ ERR_trace_file,	"ERR_trace_file",	"Erreur fichier de trace"				},
	{ ERR_satellite,	"ERR_satellite",	"No de satellite incorrect"				},
	{ ERR_input_file,	"ERR_input_file",	"Erreur fichier liste des fichiers en entree"		},
	{ ERR_parameter_file,	"ERR_parameter_file",	"Erreur fichier parametres"				},
	{ ERR_expected_file,	"ERR_expected_file",	"Erreur fichier liste des produits attendus"		},
	{ ERR_tmp_directory,	"ERR_tmp_directory",	"Erreur espace de traitement"				},
	{ ERR_generated_file,	"ERR_generated_file",	"Erreur fichier liste des produits generes"		},
	{ ERR_architecture,	"ERR_architecture",	"Architecture de la machine non compatible" 		},
	{ ERR_getenv,		"ERR_getenv",		"Variable environnement non positionnee"		},
	{ ERR_date,		"ERR_date",		"Erreur format d'une date"				},
	{ ERR_esoc,		"ERR_esoc",		"Trame ESOC incorrecte"					},
	{ ERR_frame,		"ERR_frame",		"Trame de donnees incorrecte"				},
	{ ERR_packet,		"ERR_packet",		"Paquet de donnees incorrect"				},
	{ ERR_malloc,	 	"ERR_malloc",		"Erreur allocation dynamique (plus de memoire)" 	},
	{ ERR_access,		"ERR_access",		"Droits d'acces un fichier ou repertoire insuffisants" 	},
	{ ERR_eof,		"ERR_fopen",		"Fin de fichier rencontree" 				},
	{ ERR_ferror,		"ERR_ferror",		"Erreur operation sur fichier"				},
	{ ERR_fopen,		"ERR_fopen",		"Erreur ouverture de fichier en lecture ou ecriture" 	},
	{ ERR_fgets,		"ERR_fgets",		"Erreur de lecture fichier par la fonction fgets ()" 	},
	{ ERR_fread,		"ERR_fread",		"Erreur de lecture fichier par la fonction fread ()" 	},
	{ ERR_fwrite,		"ERR_fwrite",		"Erreur d'ecriture fichier par la fonction fwrite ()" 	},
	{ ERR_fprintf,		"ERR_fprintf",		"Erreur d'ecriture fichier par la fonction fprintf ()"	},
	{ ERR_fseek,		"ERR_fseek",		"Erreur deplacement fichier par la fonction fseek ()"	},
	{ ERR_glob,		"ERR_glob",		"Erreur recherche de fichiers par la fonction glob ()"	},
	{ ERR_datation,		"ERR_datation",		"Erreur de datation"					},
	{ ERR_data,		"ERR_data",		"Erreur dans le contenu des donnees"			},
	{ ERR_idl,		"ERR_idl",		"Erreur fatale IDL"					},
	{ ERR_postcript,	"ERR_postscript",	"Erreur creation fichier Postsript"			},
	{ END_error,		"END_error",		"Fin de la liste des erreurs"				},
	{ EOF,			"???",			"Unknown ERROR symbol"					}
	};


/***************************************************************************************************
 *
 *	Verification coherence numero du satellite
 *	------------------------------------------
 */
t_err	Check_satellite (char * str, int * satellite)
{
	char *		fonction = FNAME ("Check_satellite");
	t_err		error = OK;
	int		value = atoi (str);

	* satellite = 0;

	if (value < 1 || value > 4) {

		Affiche_erreur (fonction, "Numero satellite incorrect : %s", str);
		error = ERR_argument;
		goto EXIT;
	}

	* satellite = value;

EXIT:	return error;
}


/***************************************************************************************************
 *
 *	Positionne niveau de trace a la valeur specifiee
 *	------------------------------------------------
 */
void	Set_trace_level (int valeur)
{
	niveau_trace = valeur;
}


/***************************************************************************************************
 *
 *	Lecture niveau de trace courant
 *	-------------------------------
 */
int	Get_trace_level (void)
{
	return niveau_trace;
}


/***************************************************************************************************
 *
 *	Affichage du message d'erreur
 *	-----------------------------
 */
void	Affiche_erreur (char * fonction, char * format, ...)
{
	char		buffer [MESSAGE_LENGTH];
	va_list		ptr;

	va_start (ptr, format);
	vsnprintf (buffer, MESSAGE_LENGTH -1, format, ptr);
	va_end   (ptr);

	fprintf (stdout, "E : %-*.*s : %s\n", MODULE_LENGTH, MODULE_LENGTH, fonction, buffer);
	fflush  (stdout);
}


/***************************************************************************************************
 *
 *	Affichage d'un message de trace de l'application
 *	------------------------------------------------
 */
void	Affiche_trace (int indice, char * fonction, char * format, ...)
{
	char		buffer [MESSAGE_LENGTH];
	va_list		ptr;

	if (niveau_trace >= indice) {

		va_start (ptr, format);
		vsnprintf (buffer, MESSAGE_LENGTH -1, format, ptr);
		va_end   (ptr);

		fprintf (stdout, "%1d : %-*.*s : %s\n", indice, MODULE_LENGTH, MODULE_LENGTH, fonction, buffer);

		fflush (stdout);
	}
}


/***************************************************************************************************
 *
 *	Teste si le code d'erreur correspond a une erreur fatale
 *	--------------------------------------------------------
 */
int	Erreur (t_err code)
{
	return (code >= ERROR && code < END_error) ? TRUE : FALSE;
}	


/***************************************************************************************************
 *
 *	Teste si le code d'erreur correspond a une erreur simple (warning)
 *	------------------------------------------------------------------
 */
int	Warning (t_err code)
{
	return (code >= WARNING && code < ERROR) ? TRUE : FALSE;
}	


/***************************************************************************************************
 *
 *	Retourne le mnemonique de l'erreur specifiee
 *	--------------------------------------------
 */
char *	Mnemonique_erreur (t_err valeur)
{
	static char 	str [MESSAGE_LENGTH];

	strcpy (str, Key_from_val (liste_erreur, valeur));

	return str;
}


/***************************************************************************************************
 *
 *	Retourne le libelle de l'erreur specifiee
 *	-----------------------------------------
 */
char *	Libelle_erreur (t_err valeur)
{
	static char 	str [MESSAGE_LENGTH];

	strcpy (str, Lib_from_val (liste_erreur, valeur));

	return str;
}


/***************************************************************************************************
 *
 *	Genere la liste des codes d'erreurs possibles
 *	---------------------------------------------
 */
t_err	Genere_liste_code_erreur (void)
{
	char *		fonction = FNAME ("Genere_liste_code_erreur");
	t_err		error = OK;
	t_symbol *	symbol;

	for (symbol = liste_erreur; symbol->val != EOF; symbol ++) {

		if (symbol->val == ERROR || symbol->val == WARNING) printf ("\n");

		printf ("%4d  %-20s  %s\n", symbol->val, symbol->key, symbol->lib);
	}

EXIT:	return error;
}
