/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: ARCH.h,v $
 *
 *	Version	: $Revision: 1.2 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2008/02/26 17:20:51 $
 *
 *	==========================================================================================
 *
 *	Module regroupant les definitions propres a l'architecture de la machine.
 *
 *	Principales architectures supportees :
 *
 *	- compilateur CC de Solaris sur architecture SUN
 *	- compilateur gcc du GNU sur architecture SUN
 *	- compilateur gcc du GNU sur architecture LINUX
 */

#ifndef	__ARCH_H
#define	__ARCH_H

typedef	uint8_t		byte_t;		

/*	Ecriture de valeurs de type quelconque dans un fichier, au format BIG_ENDIAN
 *	----------------------------------------------------------------------------
 */
int	Endian_write (void * ptr, size_t size, size_t nitems, FILE * stream);


/*	Lecture de valeurs de type quelconque dans un fichier, au format BIG_ENDIAN
 *	---------------------------------------------------------------------------
 */
int	Endian_read (void * ptr, size_t size, size_t nitems, FILE * stream);


#endif
