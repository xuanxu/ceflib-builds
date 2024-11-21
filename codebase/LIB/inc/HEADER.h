/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: HEADER.h,v $
 *
 *	Version	: $Revision: 1.4 $
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
 
#ifndef	__HEADER_H
#define	__HEADER_H

typedef	struct {				/* Informations sur un fichier produit		*/

	long		file_size;		/* - taille totale du fichier			*/
	int		record_size;		/* - taille d'un enregistrement			*/
	int		header_size;		/* - taille entete enregistrement		*/
	int		data_size;		/* - taille donnees enregistrement		*/
	int		record_number;		/* - nombre d'enregistrements			*/

}	t_prod_info;

#define	LOST_PACKET

typedef	struct {				/* Structure de donnees d'une entete de produit */

	uint16_t	indice;			/* - numero enregistremet fichier ENVIRONMENT	*/
	double		time_in_ms;		/* - date acquisision en milli-secondes		*/
	uint16_t	scet_day;		/* - Spacecraft Event Time de la trame		*/
	uint32_t	scet_milli;		/*						*/
	uint16_t	scet_micro;		/*						*/
	uint16_t	cis_day;		/* - Temps format CSDS recalcule		*/
	uint32_t	cis_milli;		/*						*/
	uint16_t	cis_micro;		/*						*/
	uint16_t	spin_frame;		/*						*/
	uint16_t	sp_phase_pulse;		/*						*/
	uint16_t	spin_packet;		/*						*/
	uint16_t	produit;		/*						*/
	uint16_t	instrument;		/*						*/
	uint16_t	sensitivity;		/*						*/
	uint16_t	tlm_rate;		/*						*/
	uint16_t	op_mode;		/*						*/
	uint16_t	burst_mode;		/*						*/
#ifdef	LOST_PACKET
	uint8_t		received;
	uint8_t		change_mode;
#else
	uint16_t	change_mode;		/*						*/
#endif
	float		pspin;			/*						*/
	uint16_t	phase_codif;		/*						*/
	uint16_t	phase_instr;		/*						*/
	uint16_t	taille_produit;		/*						*/

}	t_prod_header;


/*	Obtention informations sur le fichier produit
 *	---------------------------------------------
 */
t_err	Get_product_info (t_prod_info * info, FILE * entree);


/*	Ecriture d'une entete de produit sur fichier
 *	--------------------------------------------
 */
t_err	Write_product_header (t_prod_header * ptr, FILE * sortie);


/*	Lecture d'une entete de produit sur fichier
 *	-------------------------------------------
 */
t_err	Read_product_header (t_prod_header * ptr, FILE * entree);


/*	Sauter la lecture d'un enregistrement
 *	-------------------------------------
 */
t_err	Skip_record (t_prod_header * ptr, FILE * entree);


#endif
