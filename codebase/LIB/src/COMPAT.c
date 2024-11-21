/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: COMPAT.c,v $
 *
 *	Version	: $Revision: 1.1 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2017/06/27 09:38:51 $
 *
 *	==========================================================================================
 *
 *	Module regroupant les definitions specifiques aux divers OS
 *
 *	Remarque :
 *	
 *	_WIN32 : macro permettant de detecter OS Windows 32 OU 64 bits
 *
 */

#define	MODULE_NAME	"COMPAT"

#include "CISLIB.h"

#if defined (_WIN32)

#define	MAX 100

char *	my_dirname (char *s) 
{
	int	i;
	static	char path[1024];

	for (i=strlen(s) ; i>=0 ; i--) {

		if (s[i]=='/' || s[i]== '\\') {
			strncpy (path, s, i+1);
			path[i+1] = '\0';
			return path;
		}
	}
	path[0] = '\0';

	return path;
}

void	globfree (glob_t *buffer) 
{
	int 	i;

	for (i=0 ; i<buffer->gl_pathc ; i++) 
		free (buffer->gl_pathv[i]);

	if (buffer->gl_pathc) free (buffer->gl_pathv);
}

int	glob (char *masque, int flags, int (*errfunc) (const char *, int), glob_t *buffer) 
{
	int		done;
	WIN32_FIND_DATA	f;
	HANDLE		hfind;
	int		len;
	char *		base;

	base = my_dirname (masque);
	len = strlen(base);

	if ((flags & GLOB_APPEND) == 0) {
		buffer -> gl_pathc = 0;
	}

	hfind = FindFirstFile (masque, &f);
	done = hfind == INVALID_HANDLE_VALUE;

	while (!done)  {

		if (f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		}
		else {
			if (buffer->gl_pathc == 0) {
				buffer -> gl_pathv = (char **) malloc (MAX * sizeof (char **));
			}
			else if ((buffer->gl_pathc % MAX) == 0) {
				buffer -> gl_pathv = (char **) realloc (buffer->gl_pathv, (buffer->gl_pathc + MAX) * sizeof (char **));
			}
			buffer->gl_pathv[buffer->gl_pathc] = (char *) malloc ( 1 + len + strlen(f.cFileName));
			sprintf (buffer->gl_pathv[buffer->gl_pathc], "%s%s",base,f.cFileName);
			buffer -> gl_pathc ++;
		}
		done = FindNextFile (hfind,&f)==0;
	}
	FindClose (hfind);
	return 0;
}
#endif
