/***************************************************************************************************
 *
 *	Fichier	: $RCSfile: UNITS.h,v $
 *
 *	Version	: $Revision: 1.1 $
 *
 *	Auteur	: $Author: barthe $
 *
 *	Date	: $Date: 2008/09/26 11:02:06 $
 *
 *	==========================================================================================
 *
 *	Module dédie a la gestion des unites physiques utilises pour le CAA
 */

#ifndef	__UNITS_H
#define __UNITS_H

#include "CISLIB.h"

typedef	enum { RC, CS, PF, PEF, PSD } t_property;

extern	t_symbol	properties [];

char *	Physical_unit (t_property property);

char *	SI_conversion (t_property property);

#endif
