/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: HEADER.c,v $
 *
 *	Version	: $Revision: 1.5 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2015/05/04 17:28:35 $
 *
 *	==========================================================================================
 *
 *	Module charge de la gestion des entetes de produits scientifiques
 *
 */
 
#define	MODULE_NAME	"HEADER"

#include "CISLIB.h"

#ifdef	LOST_PACKET
#define	NB_HEADER_FIELDS  23
#else
#define	NB_HEADER_FIELDS  22
#endif


/****************************************************************************************************
 *
 *	Obtention d'informations sur le fichier produit
 *	-----------------------------------------------
 */
t_err	Get_product_info (t_prod_info * info, FILE * entree)
{
	char *		fonction = FNAME ("Get_product_info");
	t_err		error = OK;
	t_prod_header	header;

	if (ftell (entree) != 0) rewind (entree);

	info->file_size     = 0L;
	info->record_size   = 0;
	info->header_size   = 0;
	info->data_size     = 0;
	info->record_number = 0;

	if (Erreur (error = Read_product_header (& header, entree))) goto EXIT;

	if (error == WAR_eof) goto EXIT;

	info->header_size   = (int) ftell (entree);
	info->data_size     = header.taille_produit;
	info->record_size   = info->header_size + info->data_size;

	if (fseek (entree, 0, SEEK_END) == -1) {

		error = ERR_fseek;
		goto EXIT;
	}

	info->file_size     = ftell (entree);
	info->record_number = (int) (info->file_size / info->record_size);
	
	if ((long) info->record_size * info->record_number != info->file_size) {

		Affiche_erreur (fonction, "Structure fichier incorrecte");
		error = ERROR;
		goto EXIT;
	}

	rewind (entree);

EXIT:	return error;
}


/****************************************************************************************************
 *
 *	Ecriture d'une entete de produit sur fichier
 *	--------------------------------------------
 */
t_err	Write_product_header (t_prod_header * ptr, FILE * sortie)
{
	char *		fonction = FNAME ("Write_product_header");
	t_err		error = OK;
	int		count;

	count	= Endian_write (& ptr->indice,		sizeof (uint16_t),	1, sortie)
		+ Endian_write (& ptr->time_in_ms,	sizeof (double),	1, sortie)
		+ Endian_write (& ptr->cis_day,		sizeof (uint16_t),	1, sortie)
		+ Endian_write (& ptr->cis_milli,	sizeof (uint32_t),	1, sortie)
		+ Endian_write (& ptr->cis_micro,	sizeof (uint16_t),	1, sortie)
		+ Endian_write (& ptr->scet_day,	sizeof (uint16_t),	1, sortie)
		+ Endian_write (& ptr->scet_milli,	sizeof (uint32_t),	1, sortie)
		+ Endian_write (& ptr->scet_micro,	sizeof (uint16_t),	1, sortie)
		+ Endian_write (& ptr->spin_frame,	sizeof (uint16_t),	1, sortie)
		+ Endian_write (& ptr->spin_packet,	sizeof (uint16_t),	1, sortie)
		+ Endian_write (& ptr->pspin,		sizeof (float),		1, sortie)
		+ Endian_write (& ptr->sp_phase_pulse,	sizeof (uint16_t),	1, sortie)
		+ Endian_write (& ptr->sensitivity,	sizeof (uint16_t),	1, sortie)
		+ Endian_write (& ptr->tlm_rate,	sizeof (uint16_t),	1, sortie)
		+ Endian_write (& ptr->op_mode,		sizeof (uint16_t),	1, sortie)
		+ Endian_write (& ptr->burst_mode,	sizeof (uint16_t),	1, sortie)
		+ Endian_write (& ptr->produit,		sizeof (uint16_t),	1, sortie)
#ifdef	LOST_PACKET
		+ Endian_write (& ptr->received,	sizeof (uint8_t),	1, sortie) 
		+ Endian_write (& ptr->change_mode,	sizeof (uint8_t),	1, sortie)
#else
		+ Endian_write (& ptr->change_mode,	sizeof (uint16_t),	1, sortie)
#endif
		+ Endian_write (& ptr->phase_instr,	sizeof (uint16_t),	1, sortie)
		+ Endian_write (& ptr->instrument,	sizeof (uint16_t),	1, sortie)
		+ Endian_write (& ptr->phase_codif,	sizeof (uint16_t),	1, sortie)
		+ Endian_write (& ptr->taille_produit,	sizeof (uint16_t),	1, sortie);

	if (count != NB_HEADER_FIELDS) {

		Affiche_erreur (fonction, "Ecriture de %d champs au lieu de %d", count, NB_HEADER_FIELDS);
		error = ERR_fwrite;
		goto EXIT;
	}
	
EXIT:	return error;
}


/****************************************************************************************************
 *
 *	Lecture d'une entete de produit sur fichier
 *	-------------------------------------------
 */
t_err	Read_product_header (t_prod_header * ptr, FILE * entree)
{
	char *		fonction = FNAME ("Read_product_header");
	t_err		error = OK;
	int		count;

	count	= Endian_read (& ptr->indice,		sizeof (uint16_t),	1, entree)
		+ Endian_read (& ptr->time_in_ms,	sizeof (double),	1, entree)
		+ Endian_read (& ptr->cis_day,		sizeof (uint16_t),	1, entree)
		+ Endian_read (& ptr->cis_milli,	sizeof (uint32_t),	1, entree)
		+ Endian_read (& ptr->cis_micro,	sizeof (uint16_t),	1, entree)
		+ Endian_read (& ptr->scet_day,		sizeof (uint16_t),	1, entree)
		+ Endian_read (& ptr->scet_milli,	sizeof (uint32_t),	1, entree)
		+ Endian_read (& ptr->scet_micro,	sizeof (uint16_t),	1, entree)
		+ Endian_read (& ptr->spin_frame,	sizeof (uint16_t),	1, entree)
		+ Endian_read (& ptr->spin_packet,	sizeof (uint16_t),	1, entree)
		+ Endian_read (& ptr->pspin,		sizeof (float),		1, entree)
		+ Endian_read (& ptr->sp_phase_pulse,	sizeof (uint16_t),	1, entree)
		+ Endian_read (& ptr->sensitivity,	sizeof (uint16_t),	1, entree)
		+ Endian_read (& ptr->tlm_rate,		sizeof (uint16_t),	1, entree)
		+ Endian_read (& ptr->op_mode,		sizeof (uint16_t),	1, entree)
		+ Endian_read (& ptr->burst_mode,	sizeof (uint16_t),	1, entree)
		+ Endian_read (& ptr->produit,		sizeof (uint16_t),	1, entree)
#ifdef	LOST_PACKET
		+ Endian_read (& ptr->received,		sizeof (uint8_t),	1, entree)
		+ Endian_read (& ptr->change_mode,	sizeof (uint8_t),	1, entree)
#else
		+ Endian_read (& ptr->change_mode,	sizeof (uint16_t),	1, entree)
#endif
		+ Endian_read (& ptr->phase_instr,	sizeof (uint16_t),	1, entree)
		+ Endian_read (& ptr->instrument,	sizeof (uint16_t),	1, entree)
		+ Endian_read (& ptr->phase_codif,	sizeof (uint16_t),	1, entree)
		+ Endian_read (& ptr->taille_produit,	sizeof (uint16_t),	1, entree);

	if (count != NB_HEADER_FIELDS) {

		if (count == 0 && feof (entree)) {

			error = WAR_eof;
		}
		else {	Affiche_erreur (fonction, "Lecture de %d champs au lieu de %d", count, NB_HEADER_FIELDS);
			error = ERR_fread;
		}
		goto EXIT;
	}

EXIT:	return error;
}


/****************************************************************************************************
 *
 *	Lecture d'une entete de produit sur fichier
 *	-------------------------------------------
 */
t_err	Skip_record (t_prod_header * ptr, FILE * entree)
{
	char *		fonction = FNAME ("Skip_record");
	t_err		error = OK;

	if (fseek (entree, (long) ptr->taille_produit, SEEK_CUR) == -1) {

		Affiche_erreur (fonction, "Erreur fseek ()");
		error = ERR_fseek;
		goto EXIT;
	}

EXIT:	return error;
}
