## -*- makefile -*-
##

noinst_PROGRAMS=	antlr

AM_CFLAGS=  -I. -I$(srcdir)/../support/set \
	-I$(srcdir)/../h -DUSER_ZZSYN $(COTHER) -DZZLEXBUFSIZE=32000

#
# SGI Users, use this CFLAGS
#
# FIXME -- what are -DUSER_ZZSYN and -woff 3262 for?  Whats the real
# condition we need to detect to corectly turn those on?
#
# Think feature test, not OS test.
#CFLAGS= -O -I. -I$(SET) -I$(PCCTS_H) -DUSER_ZZSYN -woff 3262

antlr_SOURCES= \
	antlr.c err.c bits.c build.c egman.c fcache.c fset.c fset2.c \
	gen.c generic.h globals.c hash.c hash.h lex.c main.c misc.c mode.h \
	mrhoist.c pred.c proto.h scan.c $(srcdir)/../support/set/set.c  \
	stdpccts.h syn.h tokens.h


# what files does PCCTS generate (both ANTLR and DLG)
PCCTS_GEN=antlr.c scan.c err.c tokens.h mode.h parser.dlg stdpccts.h remap.h


# hopefully this won't produce some sort of chicken and egg problem

ANTLR= ./antlr
# Don't worry about the ambiguity messages coming from antlr
# for making antlr.c etc...  [should be 10 of them, I think]
antlr.c stdpccts.h parser.dlg tokens.h err.c : $(srcdir)/antlr.g
	@if test -x ${ANTLR} ; then \
		echo "${ANTLR} -gh $(srcdir)/antlr.g" ; \
		${ANTLR} -gh $(srcdir)/antlr.g ; \
	else \
		echo "WARNING:  $(srcdir)/antlr.g has changed making antlr.c stdpccts.h parser.dlg tokens.h " ; \
		echo "          and err.c out of date." ; \
		echo "          However you have not finished building $(ANTLR) yet.  So you may want to" ; \
		echo "          build again a 2nd time after your build (hopefully) finishes." ; \
	fi

DLG= ../dlg/dlg
scan.c mode.h: $(srcdir)/parser.dlg
	@if test -x ${DLG} ; then \
		echo "${DLG} -C2 $(srcdir)/parser.dlg scan.c" ; \
		${DLG} -C2 $(srcdir)/parser.dlg scan.c ; \
	else \
		echo "WARNING:  $(srcdir)/parser.g has changed making scan.c and mode.h out of date." ; \
		echo "          However you have not finished building $(DLG) yet.  So you may want to" ; \
		echo "          build again a 2nd time after your build (hopefully) finishes." ; \
	fi


EXTRA_DIST= \
	AntlrMS.mak AntlrMSVC50.dsp AntlrMSVC50.dsw AntlrMSVC50.mak \
	AntlrMSVC60.dsp AntlrMSVC60.dsw README antlr.1 antlr.g \
	antlr.r antlr1.txt antlr68K.make antlrPPC.make makefile.VMS \
	makefile1 parser.dlg watantlr.mak

