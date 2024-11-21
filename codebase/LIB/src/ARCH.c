/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: ARCH.c,v $
 *
 *	Version	: $Revision: 1.2 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2011/04/22 12:09:02 $
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

#define	MODULE_NAME	"ARCH"

#include "CISLIB.h"

#if defined(__BYTE_ORDER) && (__BYTE_ORDER == __LITTLE_ENDIAN)
#define	HAVE_TO_SWAP_BYTES
#endif

#define	BUF_SIZE	32768


/****************************************************************************************************
 *
 *	Ecriture de valeurs de type quelconque dans un fichier, au format BIG_ENDIAN
 *	----------------------------------------------------------------------------
 */
int	Endian_write (void * ptr, size_t size, size_t nitems, FILE * stream)
{
#ifdef	HAVE_TO_SWAP_BYTES

	byte_t 		buffer [BUF_SIZE];
	int		item, i;
	byte_t *	src;
	byte_t *	dst;

	assert (BUF_SIZE >= size * nitems);

	dst = (byte_t *) buffer;

	for (item = 0; item < nitems; item ++) {

		src = (byte_t *) ptr + (item + 1) * size;

		for (i = 0; i < size; i++) * dst ++ = * -- src ;
	}

	return fwrite (buffer, size, nitems, stream);
#else
	assert (BUF_SIZE >= size * nitems);

	return fwrite (ptr, size, nitems, stream);
#endif	
}


/****************************************************************************************************
 *
 *	Lecture de valeurs de type quelconque dans un fichier, au format BIG_ENDIAN
 *	---------------------------------------------------------------------------
 */
int	Endian_read (void * ptr, size_t size, size_t nitems, FILE * stream)
{
#ifdef	HAVE_TO_SWAP_BYTES

	byte_t	buffer [BUF_SIZE];
	int		count, item, i;
	byte_t *	src;
	byte_t *	dst;

	assert (BUF_SIZE >= size * nitems);

	count = fread (buffer, size, nitems, stream);

	dst = (byte_t *) ptr;

	for (item = 0; item < nitems; item ++) {

		src = (byte_t *) buffer + (item + 1) * size;

		for (i = 0; i < size; i++) * dst ++ = * -- src ;
	}

	return count;
#else
	assert (BUF_SIZE >= size * nitems);

	return fread (ptr, size, nitems, stream);
#endif
}
