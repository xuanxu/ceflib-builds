/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: INTERFACE.c,v $
 *
 *	Version	: $Revision: 1.14 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2025/04/01 13:53:11 $
 *
 *	==========================================================================================
 *
 *	Module charge de la gestion des divers fichiers Double Star
 */

#define	MODULE_NAME	"INTERFACE"

#include "CISLIB.h"

t_symbol cis_data_source [] = {		/* Differentes sources de donnees traitees	*/

	{ L1,		"L1",		"Level 1 data"			},
	{ L2,		"L2",		"Level 2 data"			},
	{ L3,		"L3",		"Level 3 data"			},
	{ EOF,		"???",		"Unknown file source"		}
};

t_symbol cis_file_ident [] = {		/* Identificateurs des fichiers 	*/

	{ COD_00,	"COD_00",	"" },
	{ COD_01,	"COD_01",	"" },
	{ COD_02,	"COD_02",	"" },
	{ COD_03,	"COD_03",	"" },
	{ COD_04,	"COD_04",	"" },
	{ COD_05,	"COD_05",	"" },
	{ COD_06,	"COD_06",	"" },
	{ COD_07,	"COD_07",	"" },
	{ COD_09,	"COD_09",	"" },
	{ COD_11,	"COD_11",	"" },
	{ COD_12,	"COD_12",	"" },
	{ COD_13,	"COD_13",	"" },
	{ COD_14,	"COD_14",	"" },
	{ COD_15,	"COD_15",	"" },
	{ COD_16,	"COD_16",	"" },
	{ COD_17,	"COD_17",	"" },
	{ COD_18,	"COD_18",	"" },
	{ COD_19,	"COD_19",	"" },
	{ COD_20,	"COD_20",	"" },
	{ COD_21,	"COD_21",	"" },
	{ COD_22,	"COD_22",	"" },
	{ COD_23,	"COD_23",	"" },
	{ COD_24,	"COD_24",	"" },
	{ COD_27,	"COD_27",	"" },
	{ COD_28,	"COD_28",	"" },
	{ COD_29,	"COD_29",	"" },
	{ COD_32,	"COD_32",	"" },
	{ COD_33,	"COD_33",	"" },
	{ COD_34,	"COD_34",	"" },
	{ COD_35,	"COD_35",	"" },
	{ COD_39,	"COD_39",	"" },
	{ COD_40,	"COD_40",	"" },
	{ COD_41,	"COD_41",	"" },
	{ COD_42,	"COD_42",	"" },
	{ COD_43,	"COD_43",	"" },
	{ COD_44,	"COD_44",	"" },
	{ COD_45,	"COD_45",	"" },
	{ COD_46,	"COD_46",	"" },
	{ COD_47,	"COD_47",	"" },
	{ COD_48,	"COD_48",	"" },
	{ COD_49,	"COD_49",	"" },
	{ COD_61,	"COD_61",	"" },
	{ COD_62,	"COD_62",	"" },

	{ HIA_00,	"HIA_00",	"" },
	{ HIA_01,	"HIA_01",	"" },
	{ HIA_02,	"HIA_02",	"" },
	{ HIA_04,	"HIA_04",	"" },
	{ HIA_05,	"HIA_05",	"" },
	{ HIA_06,	"HIA_06",	"" },
	{ HIA_07,	"HIA_07",	"" },
	{ HIA_08,	"HIA_08",	"" },
	{ HIA_09,	"HIA_09",	"" },
	{ HIA_10,	"HIA_10",	"" },
	{ HIA_11,	"HIA_11",	"" },
	{ HIA_12,	"HIA_12",	"" },
	{ HIA_13,	"HIA_13",	"" },
	{ HIA_14,	"HIA_14",	"" },
	{ HIA_15,	"HIA_15",	"" },
	{ HIA_16,	"HIA_16",	"" },
	{ HIA_17,	"HIA_17",	"" },
	{ HIA_18,	"HIA_18",	"" },
	{ HIA_19,	"HIA_19",	"" },
	{ HIA_20,	"HIA_20",	"" },
	{ HIA_21,	"HIA_21",	"" },
	{ HIA_22,	"HIA_22",	"" },
	{ HIA_23,	"HIA_23",	"" },
	{ HIA_24,	"HIA_24",	"" },
	{ HIA_61,	"HIA_61",	"" },
	{ HIA_62,	"HIA_62",	"" },

	{ HSK_00,	"HSK_00",	"" },
	{ HSK_01,	"HSK_01",	"" },
	{ HSK_02,	"HSK_02",	"" },
	{ HSK_03,	"HSK_03",	"" },
	{ HSK_04,	"HSK_04",	"" },
	{ HSK_05,	"HSK_05",	"" },
	{ HSK_06,	"HSK_06",	"" },
	{ HSK_07,	"HSK_07",	"" },
	{ HSK_08,	"HSK_08",	"" },
	{ HSK_09,	"HSK_09",	"" },
	{ HSK_10,	"HSK_10",	"" },
	{ HSK_11,	"HSK_11"	"" },

};

typedef struct {			/* Entree table des fichiers produits	*/

	t_data_source	source;		/* - origine des donnees		*/
	t_file_ident	ident;		/* - identificateur du fichier		*/
	char *		pattern;	/* - masque de recherche		*/
	FILE *		f_desc;		/* - descripteur Unix			*/
	t_filename	filename;	/* - nom du fichier			*/

}	t_prod_desc;


static	t_prod_desc	cis_files [] = {

	/****** Level 1 data files **********************************************/

	{ L1,	COD_00,		"C%d_L1_CISCOD_%s_V%s_PROM_0xxxx.sdd",	NULL, "" },
	{ L1,	COD_01,		"C%d_L1_CISCOD_%s_V%s_PROM_1xxxx.sdd",	NULL, "" },
	{ L1,	COD_02,		"C%d_L1_CISCOD_%s_V%s_PROM_2xxxx.sdd",	NULL, "" },
	{ L1,	COD_03,		"C%d_L1_CISCOD_%s_V%s_PROM_3xxxx.sdd",	NULL, "" },
	{ L1,	COD_04,		"C%d_L1_CISCOD_%s_V%s_PROM_4xxxx.sdd",	NULL, "" },
	{ L1,	COD_05,		"C%d_L1_CISCOD_%s_V%s_WEC_5xxxxx.sdd",	NULL, "" },
	{ L1,	COD_06,		"C%d_L1_CISCOD_%s_V%s_FGM_6xxxxx.sdd",	NULL, "" },
	{ L1,	COD_07,		"C%d_L1_CISCOD_%s_V%s_MOM_7xxxxx.sdd",	NULL, "" },
	{ L1,	COD_09,		"C%d_L1_CISCOD_%s_V%s_MOMCOLD_9x.sdd",	NULL, "" },
	{ L1,	COD_11,		"C%d_L1_CISCOD_%s_V%s_3DMASS_11x.sdd",	NULL, "" },
	{ L1,	COD_12,		"C%d_L1_CISCOD_%s_V%s_3DP16E_12x.sdd",	NULL, "" },
	{ L1,	COD_13,		"C%d_L1_CISCOD_%s_V%s_3DP31E_13x.sdd",	NULL, "" },
	{ L1,	COD_14,		"C%d_L1_CISCOD_%s_V%s_3DP24A_14x.sdd",	NULL, "" },
	{ L1,	COD_15,		"C%d_L1_CISCOD_%s_V%s_3DA16E_15x.sdd",	NULL, "" },
	{ L1,	COD_16,		"C%d_L1_CISCOD_%s_V%s_3DA31E_16x.sdd",	NULL, "" },
	{ L1,	COD_17,		"C%d_L1_CISCOD_%s_V%s_3DHM16E_17.sdd",	NULL, "" },
	{ L1,	COD_18,		"C%d_L1_CISCOD_%s_V%s_3DHM31E_18.sdd",	NULL, "" },
	{ L1,	COD_19,		"C%d_L1_CISCOD_%s_V%s_2D4M31E_19.sdd",	NULL, "" },
	{ L1,	COD_20,		"C%d_L1_CISCOD_%s_V%s_2D4M16E_20.sdd",	NULL, "" },
	{ L1,	COD_21,		"C%d_L1_CISCOD_%s_V%s_2D2M16E_21.sdd",	NULL, "" },
	{ L1,	COD_22,		"C%d_L1_CISCOD_%s_V%s_2D1M31E_22.sdd",	NULL, "" },
	{ L1,	COD_23,		"C%d_L1_CISCOD_%s_V%s_PAD16E_23x.sdd",	NULL, "" },
	{ L1,	COD_24,		"C%d_L1_CISCOD_%s_V%s_PAD31E_24x.sdd",	NULL, "" },
	{ L1,	COD_27,		"C%d_L1_CISCOD_%s_V%s_RAT_27xxxx.sdd",	NULL, "" },
	{ L1,	COD_28,		"C%d_L1_CISCOD_%s_V%s_EVE_28xxxx.sdd",	NULL, "" },
	{ L1,	COD_29,		"C%d_L1_CISCOD_%s_V%s_RPA16E_29x.sdd",	NULL, "" },
	{ L1,	COD_36,		"C%d_L1_CISCOD_%s_V%s_BM3COUN_36.sdd",	NULL, "" },
	{ L1,	COD_38,		"C%d_L1_CISCOD_%s_V%s_BM3TRIG_38.sdd",	NULL, "" },
	{ L1,	COD_32,		"C%d_L1_CISCOD_%s_V%s_3DHE1_46xx.sdd",	NULL, "" },
	{ L1,	COD_33,		"C%d_L1_CISCOD_%s_V%s_3DO16E_47x.sdd",	NULL, "" },
	{ L1,	COD_34,		"C%d_L1_CISCOD_%s_V%s_3DHE1_48xx.sdd",	NULL, "" },
	{ L1,	COD_35,		"C%d_L1_CISCOD_%s_V%s_3DO31E_49x.sdd",	NULL, "" },
	{ L1,	COD_39,		"C%d_L1_CISCOD_%s_V%s_3DP16E_12x.sdd",	NULL, "" },
	{ L1,	COD_40,		"C%d_L1_CISCOD_%s_V%s_3DP31E_13x.sdd",	NULL, "" },
	{ L1,	COD_41,		"C%d_L1_CISCOD_%s_V%s_3DP24A_14x.sdd",	NULL, "" },
	{ L1,	COD_42,		"C%d_L1_CISCOD_%s_V%s_3DA16E_15x.sdd",	NULL, "" },
	{ L1,	COD_43,		"C%d_L1_CISCOD_%s_V%s_3DA31E_16x.sdd",	NULL, "" },
	{ L1,	COD_44,		"C%d_L1_CISCOD_%s_V%s_3DHM16E_17.sdd",	NULL, "" },
	{ L1,	COD_45,		"C%d_L1_CISCOD_%s_V%s_3DHM31E_18.sdd",	NULL, "" },
	{ L1,	COD_46,		"C%d_L1_CISCOD_%s_V%s_3DHE1_46xx.sdd",	NULL, "" },
	{ L1,	COD_47,		"C%d_L1_CISCOD_%s_V%s_3DO16E_47x.sdd",	NULL, "" },
	{ L1,	COD_48,		"C%d_L1_CISCOD_%s_V%s_3DHE1_48xx.sdd",	NULL, "" },
	{ L1,	COD_49,		"C%d_L1_CISCOD_%s_V%s_3DO31E_49x.sdd",	NULL, "" },
	{ L1,	COD_61,		"C%d_L1_CISCOD_%s_V%s_IOREAD_61x.sdd",	NULL, "" },
	{ L1,	COD_62,		"C%d_L1_CISCOD_%s_V%s_MEMDUMP_62.sdd",	NULL, "" },

	{ L1,	HIA_00,		"C%d_L1_CISHIA_%s_V%s_PROM_0xxxx.sdd",	NULL, "" },
	{ L1,	HIA_01,		"C%d_L1_CISHIA_%s_V%s_PROM_1xxxx.sdd",	NULL, "" },
	{ L1,	HIA_02,		"C%d_L1_CISHIA_%s_V%s_MOM_2xxxxx.sdd",	NULL, "" },
	{ L1,	HIA_04,		"C%d_L1_CISHIA_%s_V%s_MOMCOLD_4x.sdd",	NULL, "" },
	{ L1,	HIA_05,		"C%d_L1_CISHIA_%s_V%s_3DM62E_5xx.sdd",	NULL, "" },
	{ L1,	HIA_06,		"C%d_L1_CISHIA_%s_V%s_3D31E88A_6.sdd",	NULL, "" },
	{ L1,	HIA_06B,	"C%d_L1_CISHIA_%s_V%s_3D31EBM3_6.sdd",	NULL, "" },
	{ L1,	HIA_07,		"C%d_L1_CISHIA_%s_V%s_3D31E42A_7.sdd",	NULL, "" },
	{ L1,	HIA_08,		"C%d_L1_CISHIA_%s_V%s_3DCOLD_8xx.sdd",	NULL, "" },
	{ L1,	HIA_08,		"C%d_L1_CISHIA_%s_V%s_3DCBM3_8xx.sdd",	NULL, "" },
	{ L1,	HIA_09,		"C%d_L1_CISHIA_%s_V%s_HOT1D62E_9.sdd",	NULL, "" },
	{ L1,	HIA_10,		"C%d_L1_CISHIA_%s_V%s_2DAZ_10xxx.sdd",	NULL, "" },
	{ L1,	HIA_11,		"C%d_L1_CISHIA_%s_V%s_2DPO_11xxx.sdd",	NULL, "" },
	{ L1,	HIA_12,		"C%d_L1_CISHIA_%s_V%s_PAD_12xxxx.sdd",	NULL, "" },
	{ L1,	HIA_13,		"C%d_L1_CISHIA_%s_V%s_2DPOCLD_13.sdd",	NULL, "" },
	{ L1,	HIA_14,		"C%d_L1_CISHIA_%s_V%s_2DAZCLD_14.sdd",	NULL, "" },
	{ L1,	HIA_15,		"C%d_L1_CISHIA_%s_V%s_3DH16E_15x.sdd",	NULL, "" },
	{ L1,	HIA_16,		"C%d_L1_CISHIA_%s_V%s_3DH30E_16x.sdd",	NULL, "" },
	{ L1,	HIA_17,		"C%d_L1_CISHIA_%s_V%s_3DH62E_17x.sdd",	NULL, "" },
	{ L1,	HIA_17B,	"C%d_L1_CISHIA_%s_V%s_3DHBM3_17x.sdd",	NULL, "" },
	{ L1,	HIA_18,		"C%d_L1_CISHIA_%s_V%s_1DH31E_18x.sdd",	NULL, "" },
	{ L1,	HIA_19,		"C%d_L1_CISHIA_%s_V%s_HOTPAD_19x.sdd",	NULL, "" },
	{ L1,	HIA_20,		"C%d_L1_CISHIA_%s_V%s_2DH3A_20xx.sdd",	NULL, "" },
	{ L1,	HIA_21,		"C%d_L1_CISHIA_%s_V%s_3DH31E_21x.sdd",	NULL, "" },
	{ L1,	HIA_21B,	"C%d_L1_CISHIA_%s_V%s_3DHBM3_21x.sdd",	NULL, "" },
	{ L1,	HIA_22,		"C%d_L1_CISHIA_%s_V%s_3DC31E_22x.sdd",	NULL, "" },
	{ L1,	HIA_22B,	"C%d_L1_CISHIA_%s_V%s_3DCBM3_22x.sdd",	NULL, "" },
	{ L1,	HIA_23,		"C%d_L1_CISHIA_%s_V%s_COMP3DH_23.sdd",	NULL, "" },
	{ L1,	HIA_24,		"C%d_L1_CISHIA_%s_V%s_COMP3DC_24.sdd",	NULL, "" },
	{ L1,	HIA_61,		"C%d_L1_CISHIA_%s_V%s_IO_61xxxxx.sdd",	NULL, "" },
	{ L1,	HIA_62,		"C%d_L1_CISHIA_%s_V%s_MEM_62xxxx.sdd",	NULL, "" },

	{ L1,	HSK_00,		"C%d_L1_CISHSK_%s_V%s_COMMANDxxx.hkk",	NULL, "" },
	{ L1,	HSK_01,		"C%d_L1_CISHSK_%s_V%s_SWVERxxxxx.hkk",	NULL, "" },
	{ L1,	HSK_02,		"C%d_L1_CISHSK_%s_V%s_ERRORxxxxx.hkk",	NULL, "" },
	{ L1,	HSK_03,		"C%d_L1_CISHSK_%s_V%s_STATUSxxxx.hkk",	NULL, "" },
	{ L1,	HSK_04,		"C%d_L1_CISHSK_%s_V%s_HIA_DISCRx.hkk",	NULL, "" },
	{ L1,	HSK_05,		"C%d_L1_CISHSK_%s_V%s_RPAxxxxxxx.hkk",	NULL, "" },
	{ L1,	HSK_06,		"C%d_L1_CISHSK_%s_V%s_COD_VOLT_0.hkk",	NULL, "" },
	{ L1,	HSK_07,		"C%d_L1_CISHSK_%s_V%s_COD_VOLT_1.hkk",	NULL, "" },
	{ L1,	HSK_08,		"C%d_L1_CISHSK_%s_V%s_COD_VOLT_2.hkk",	NULL, "" },
	{ L1,	HSK_09,		"C%d_L1_CISHSK_%s_V%s_HIA_VOLT_0.hkk",	NULL, "" },
	{ L1,	HSK_10,		"C%d_L1_CISHSK_%s_V%s_HIA_VOLT_2.hkk",	NULL, "" },
	{ L1,	HSK_11,		"C%d_L1_CISHSK_%s_V%s_COD_COVERx.hkk",	NULL, "" },
	{ L1,	STOF,		"C%d_L1_CISAUX_%s_V%s_STOFxxxxxx.axx",	NULL, "" },
	{ L1,	STEF,		"C%d_L1_CISAUX_%s_V%s_STEFxxxxxx.axx",	NULL, "" },
	{ L1,	SATT,		"C%d_L1_CISAUX_%s_V%s_SATTxxxxxx.axx",	NULL, "" },
	{ L1,	CHDS,		"C%d_L1_CISAUX_%s_V%s_CHDSxxxxxx.axx",	NULL, "" },
	{ L1,	COVM,		"C%d_L1_CISAUX_%s_V%s_COVMxxxxxx.axx",	NULL, "" },
	{ L1,	HKDSAT,		"C%d_L1_CISSAT_%s_V%s_HKDSATxxxx.axx",	NULL, "" },
	{ L1,	HPDSAT,		"C%d_L1_CISSAT_%s_V%s_HPDSATxxxx.axx",	NULL, "" },
	{ L1,	ENV,		"C%d_L1_CISENV_%s_V%s_ENVIRONMTx.sdd",	NULL, "" },
	{ L1,	CATAL,		"C%d_L1_CISENV_%s_V%s_PRODCATALx.asc",	NULL, "" },
	{ L1,	ARCH,		"C%d_L1_CISARC_%s_V%s_ARCHIVExxx.arc",	NULL, "" },
	{ L1,	CDROM,		"C%d_L1_CISCDR_%s_V%s_CDROMNAMEx.axx",	NULL, "" },
	{ L1,	TRACE,		"C%d_L1_CISxxx_%s_V%s_TRACExxxxx.tra",	NULL, "" },

	/****** Level 2 data files **********************************************/

	{ L2,	HSK,		"C%d_L2_CISHSK_%s_V%s_VALUESxxxx.hkk",	NULL, "" },

	/****** Level 3 data files **********************************************/

	/****** CAA 1 data files ************************************************/

	/****** End of table ****************************************************/

	{ EOF,	EOF,	NULL,					NULL, "" }
};


/****************************************************************************************************
 *
 *	Retourne un chaine contenant les references du fichier
 *	------------------------------------------------------
 */
static	char *	File_reference (t_data_source source, t_file_ident ident)
{
	static char	buffer [40];

	sprintf (buffer, "%s %s", 
		Key_from_val (cis_data_source, source),
		Key_from_val (cis_file_ident, ident));
		
	return buffer;	
}


/****************************************************************************************************
 *
 *	Recherche une entree de la table des fichiers Cluster
 *	-----------------------------------------------------
 */
static	t_prod_desc * Get_CIS_file_entry (t_data_source source, t_file_ident ident)
{
	t_prod_desc *	ptr;

	for (ptr = cis_files; ptr->source != EOF; ptr ++) {

		if (ptr->source == source && ptr->ident == ident) return ptr;
	}
	return (t_prod_desc *) NULL;
}


/****************************************************************************************************
 *
 *	Ouverture d'un fichier produit
 *	------------------------------
 */
FILE *	Open_CIS_file (t_data_source source, t_file_ident ident, char * mode)
{
	char *		fonction = FNAME ("Open_CIS_file");
	FILE *		file = NULL;
	t_prod_desc *	ptr = NULL;

	if ((ptr = Get_CIS_file_entry (source, ident)) == NULL) {

		Affiche_trace (1, fonction, "Erreur : Produit %s inconnu", File_reference (source, ident));
		goto EXIT;
	}

	if ((file = fopen (ptr->filename, mode)) == NULL) {

		Affiche_erreur (fonction, "Ouverture %s impossible", ptr->filename);
		Affiche_erreur (fonction, "%s", strerror (errno));
		goto EXIT;
	}
	
	ptr->f_desc = file;

EXIT:	return file;
}


/****************************************************************************************************
 *
 *	Re-ouverture d'un fichier produit
 *	---------------------------------
 */
FILE *	Reopen_CIS_file (t_data_source source, t_file_ident ident, char * mode)
{
	char *		fonction = FNAME ("Reopen_CIS_file");
	FILE *		file = NULL;
	t_prod_desc *	ptr = NULL;

	if ((ptr = Get_CIS_file_entry (source, ident)) == NULL) {

		Affiche_trace (1, fonction, "Erreur : Produit %s inconnu", File_reference (source, ident));
		goto EXIT;
	}

	/*	Si le fichier est deja ouvert on retourne son descripteur
	 */
	if (ptr->f_desc != NULL) return ptr->f_desc;

	/*	Sinon, on l'ouvre et on met a jour la table des descripteurs
	 */
	if ((file = fopen (ptr->filename, mode)) == NULL) {

		Affiche_erreur (fonction, "Ouverture %s impossible", ptr->filename);
		Affiche_erreur (fonction, "%s", strerror (errno));
		goto EXIT;
	}
	
	ptr->f_desc = file;

EXIT:	return file;
}


/****************************************************************************************************
 *
 *	Retourne le nom du fichier produit specifie
 *	-------------------------------------------
 */
char *  Product_filename (t_data_source source, t_file_ident ident)
{
	t_prod_desc *	ptr = Get_CIS_file_entry (source, ident);

	if (ptr == NULL) return "UNKNOWN";

	return ptr->filename;
}


/****************************************************************************************************
 *
 *	Redirection stdout sur fichier de trace
 *	---------------------------------------
 */
t_err   Redirect_stdout (t_data_source source)
{
	char *		fonction = FNAME ("Redirect_stdout");
	t_err		error = OK;
	FILE *		sortie = NULL;

	if ((sortie = Open_CIS_file (source, TRACE, "w")) == NULL) {

		Affiche_erreur (fonction, "Redirection sur %s impossible", Product_filename (source, TRACE));
		error = ERR_fopen;
		goto EXIT;
	}

	Affiche_trace (1, fonction, "Redirection stdout sur %s", Product_filename (source, TRACE));

	if (dup2 (fileno (sortie), STDOUT_FILENO) == -1) {

		Affiche_erreur (fonction, "Erreur dup2 () : %s", strerror (errno));
		error = ERROR;
		goto EXIT;
	}

EXIT:	return error;
}


/****************************************************************************************************
 *
 *	Retourne le repertoire correspond au type des donnees
 *	-----------------------------------------------------
 */
t_err	Compute_directory (t_data_source source, int sat, t_date date, char * directory)
{
	char *		fonction = FNAME ("Compute_directory");
	t_err		error = OK;
	char		amj [9];
	int		r;

	sprintf (amj, "%04d%02d%02d", date.annee, date.mois, date.jour);

	switch (source) {

	case L1  :	r = snprintf (directory, FILENAME_LENGTH, "%s/DATA/L1/%s/CLUSTER%d", Get_CIS_root(), amj, sat);
			break;
	case L2  :	r = snprintf (directory, FILENAME_LENGTH, "%s/DATA/L2/%s/CLUSTER%d", Get_CIS_root(), amj, sat);
			break;
	case L3  :	r = snprintf (directory, FILENAME_LENGTH, "%s/DATA/L3/%s/CLUSTER%d", Get_CIS_root(), amj, sat);
			break;
	default  :	error = ERROR;
			strcpy (directory, "");
			goto EXIT;
	}
	assert (r < FILENAME_LENGTH);

EXIT:	return error;
}


/****************************************************************************************************
 *
 *	Creation repertoire destination ou purge si celui-ci n'est pas vide
 *	-------------------------------------------------------------------
 */
t_err	Clear_directory (t_data_source source, int sat, t_date date)
{
	char *		fonction = FNAME ("Clear_directory");
	t_err		error = OK;
	struct stat	info;
	int		i, r, nbre;
	t_filename	directory;
	t_filename	masque;

	if (Erreur (error = Compute_directory (source, sat, date, directory))) goto EXIT;

	if (stat (directory, & info) == -1) {

		if (errno == ENOENT) {

#if defined (_WIN32)
			if (mkdir (directory) == -1) {
#else
			if (mkdir (directory, 0775) == -1) {
#endif

				Affiche_erreur ("Creation repertoire %s impossible", directory);
				Affiche_erreur (fonction, "Erreur stat () : %s", strerror (errno));
				error = ERROR;
				goto EXIT;
			}

			Affiche_trace (1, fonction, "Creation repertoire %s", directory);
			goto EXIT;
		}
		else {	Affiche_erreur (fonction, "Acces repertoire %s impossible", directory);
			Affiche_erreur (fonction, "Erreur stat () : %s", strerror (errno));
			error = ERROR;
			goto EXIT;
		}
	}

	if (S_ISDIR (info.st_mode) == FALSE) {

		Affiche_erreur (fonction, "%s n'est pas un repertoire", directory);
		error = ERROR;
		goto EXIT;
	}

	r = snprintf (masque, FILENAME_LENGTH, "%s/*", directory);

	assert (r < FILENAME_LENGTH);

	if (Erreur (error = Search_files (masque, & nbre))) goto EXIT;
	
	for (i = 0; i < nbre; i ++) {

		if (unlink (Get_filename (i)) == -1) {

			Affiche_erreur (fonction, "Suppression %s impossible", Get_filename (i));
			Affiche_erreur (fonction, "Erreur unlink () : %s", strerror (errno));
			error = ERROR;
			goto EXIT;
		}
	}
	
	Search_free ();

EXIT:	return error;
}


/****************************************************************************************************
 *
 *	Determine le nom complet des fichiers pour une source et une date donnee
 *	------------------------------------------------------------------------
 */
t_err	Compute_CIS_filenames (t_data_source source, int sat, t_date date, int version)
{
	char *		fonction = FNAME ("Compute_CIS_filenames");
	t_err		error = OK;
	t_prod_desc *	ptr;
	t_filename	directory;
	char		amj [9];
	char		vmask [3];
	int		r;

	if (Erreur (error = Compute_directory (source, sat, date, directory))) goto EXIT;

	sprintf (amj, "%04d%02d%02d", date.annee, date.mois, date.jour);

	if (version == VMAX) 
		strcpy (vmask, "??");
	else 
		sprintf (vmask, "%02d", version);

	Affiche_trace (3, fonction, "%s directory : %s", 
		Key_from_val (cis_data_source, source), 
		directory);

	for (ptr = cis_files; ptr->source != EOF; ptr++) {

		t_filename	buffer;
		int		nbre;

		if (ptr->source != source) continue;

		r = snprintf (buffer, FILENAME_LENGTH, "%s/%s", directory, ptr->pattern);

		assert (r < FILENAME_LENGTH);

		r = snprintf (ptr->filename, FILENAME_LENGTH, buffer, sat, amj, vmask);

		assert (r < FILENAME_LENGTH);

		if (version == VMAX) {

			if (Erreur (error = Search_files (ptr->filename, & nbre))) goto EXIT;

			if (nbre > 0) strcpy (ptr->filename, Get_filename (nbre -1));

			Search_free ();
		}
		Affiche_trace (5, fonction, "%s", ptr->filename);
	}

EXIT:	return error;
}
