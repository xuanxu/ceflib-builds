#
#	Fichier	: $RCSfile: Makefile,v $, v $Revision: 1.16 $
#
#	Date	: $Date: 2012/11/30 09:50:29 $
#
#	Auteur	: $Author: barthe $
#
#	Version	: %Z% version %I% de %M% du %G%
#
#	================================================================================
#
#	Fichier Makefile de la librairie CEFLIB
#
##

#		Composition des options de compilations
#		---------------------------------------
#
CFLAGS =	$(CDEBUG) $(COPTI) $(CSHARED) -Iinc -I$(CISLIB_DIR)/inc
FFLAGS =
LDFLAGS =	$(CISLIB) $(ZLIB)

#		Repertoire de recherche des sources
#		----------------------------------
#
VPATH =		src

#		Nom de la librairie
#		-------------------
#
LIB =		bin/libCEF_$(CC).a

#		Liste des modules de compilation
#		--------------------------------
#
MODULES =	SUBST UNITS ATTRIBUTE METADATA CEF


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
