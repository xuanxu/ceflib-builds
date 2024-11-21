#
#	Fichier	: $RCSfile: Makefile,v $, v $Revision: 1.13 $
#
#	Date	: $Date: 2017/06/27 09:37:17 $
#
#	Auteur	: $Author: barthe $
#
#	Version	: %Z% version %I% de %M% du %G%
#
#	================================================================================
#
#	Fichier Makefile de la librairie CISLIB
#
##

#		Composition des options de compilations
#		---------------------------------------
#
CFLAGS =	$(CDEBUG) $(COPTI) $(CSHARED) -I inc 
FFLAGS =
LDFLAGS =

#		Repertoire de recherche des sources
#		----------------------------------
#
VPATH =		src

#		Nom de la librairie
#		-------------------
#
LIB =		bin/libCIS_$(CC).a

#		Liste des modules de compilation
#		--------------------------------
#
MODULES =	ARCH \
		COMPAT \
		ERRORS \
		HEADER \
		INTERFACE \
		IRAP \
		LISTS \
		MATHS \
		SYMBOLS \
		TIMES \
		FILES \
		ZZLIB


#		Nom des fichiers objets
#		-----------------------
#
OBJS =		$(MODULES:%=obj/%.o) 


#		Diverses cibles de compilation
#		------------------------------
#
all:		init $(LIB)

init:
		mkdir -p obj
		mkdir -p bin

clean:
		rm -rf 	obj
		rm -f	.make.state

raz:		clean
		rm -f	$(LIB)
		

#		PARTIE A NE PAS MODIFIER
#		------------------------
#
.SUFFIXES:
.KEEP_STATE:

$(LIB) :	$(LIB)($(OBJS))
		$(RANLIB) $@

$(LIB)(%.o) :	%.o
		$(AR) -r $@ $%

obj/%.o:	%.c
		$(CC) -c $(CFLAGS) $< -o $@

obj/%.o:	%.for
		$(FC) -c $(FFLAGS) $< -o $@
