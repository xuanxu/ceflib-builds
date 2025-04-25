/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: COMPAT.h,v $
 *
 *	Version	: $Revision: 1.5 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2025/04/22 09:15:49 $
 *
 *	==========================================================================================
 *
 *	Module regroupant les specificites des divers OS
 *
 *	Remarque :
 *
 *	_WIN32 : macro qui permet de detecter si on se trouve sous OS Windows 23 OU 64 bits
 *	
 */

#ifndef	__COMPAT_H
#define	__COMPAT_H

#if defined (_WIN32)

	#include <windows.h>

	#define	PATH_SEPARATOR	";"
	#define	DIR_SEPARATOR	"\\"

	//	Necessaire pour inhiber une ligne ERROR.h dans wingdi.h, inclus par windows.h
	#undef	ERROR 

	#define GLOB_DOOFFS (1 << 3)
	#define GLOB_APPEND (1 << 5)

	typedef struct {
		char 	**gl_pathv;
		int	gl_pathc;
		int	gl_offs;
	}	glob_t;

	void	globfree (glob_t *buffer);

	int	glob (char *masque, int flags, int (*errfunc) (const char *, int), glob_t *buffer);

#else
	#include <glob.h>

	#define	PATH_SEPARATOR	":"
	#define	DIR_SEPARATOR	"/"
#endif

#ifdef	__APPLE__

	#include <float.h>

	#define	MAXDOUBLE	__DBL_MAX__
	#define	MINDOUBLE	__DBL_MIN__
#else
	#include <values.h>
#endif

#endif
