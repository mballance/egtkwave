## -*- makefile -*-
##

noinst_PROGRAMS= dlg

AM_CFLAGS= -I. -I$(srcdir)/../support/set \
	-I$(srcdir)/../h -DUSER_ZZSYN -DZZLEXBUFSIZE=32000

dlg_SOURCES= \
	automata.c dlg.h dlg_p.c dlg_a.c err.c main.c mode.h \
	output.c relabel.c $(srcdir)/../support/set/set.c  \
	stdpccts.h support.c  tokens.h


# hopefully this won't produce some sort of chicken and egg problem

ANTLR= ../antlr/antlr
# Don't worry about the ambiguity messages coming from antlr
# for making antlr.c etc...  [should be 10 of them, I think]
dlg_p.c parser.dlg err.c tokens.h : $(srcdir)/dlg_p.g
	@if test -x ${ANTLR} ; then \
		echo "${ANTLR} $(srcdir)/dlg_p.g" ; \
		${ANTLR} $(srcdir)/dlg_p.g ; \
	else \
		echo "WARNING:  $(srcdir)/dlg_p.g has changed making dlg_p.c parser.dlg err.c tokens.h" ; \
		echo "          out of date." ; \
		echo "          However you have not finished building $(ANTLR) yet.  So you may want to" ; \
		echo "          build again a 2nd time after your build (hopefully) finishes." ; \
	fi

DLG= ./dlg
dlg_a.c mode.h: $(srcdir)/parser.dlg
	@if test -x ${DLG} ; then \
		echo "${DLG} -C2 $(srcdir)/parser.dlg scan.c" ; \
		${DLG} -C2 $(srcdir)/parser.dlg dlg_a.c ; \
	else \
		echo "WARNING:  $(srcdir)/parser.g has changed making dlg_a.c and mode.h out of date." ; \
		echo "          However you have not finished building $(DLG) yet.  So you may want to" ; \
		echo "          build again a 2nd time after your build (hopefully) finishes." ; \
	fi


EXTRA_DIST= \
	DlgMS.mak DlgMSVC50.dsp DlgMSVC50.dsw DlgMSVC60.dsp \
	DlgMSVC60.dsw dlg.1 dlg.r dlg1.txt dlg68K.make \
	dlgPPC.make dlg_p.g makefile.VMS makefile1 parser.dlg \
	watdlg.mak

