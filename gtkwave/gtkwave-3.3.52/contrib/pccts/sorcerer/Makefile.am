## -*- makefile -*-
##

SUBDIRS= h lib test testcpp

#PCCTS=..
#BIN = $(PCCTS)/bin
#ANTLR = $(BIN)/antlr
#DLG = $(BIN)/dlg
#PCCTS_H = $(PCCTS)/h
#SET = $(PCCTS)/support/set

AM_CFLAGS =  -I$(srcdir)/h -I$(srcdir) -I$(srcdir)/../h \
	-I$(srcdir)/../support/set -DZZLEXBUFSIZE=32000
#AFLAGS = -gh -k 2 -gt
#DFLAGS = -C2
#GRM = sor.g
#GENHFILES = tokens.h mode.h stdpccts.h

noinst_PROGRAMS= sor

sor_SOURCES= \
	cpp.c err.c gen.c globals.c hash.c hash.h look.c \
	main.c mode.h proto.h scan.c $(srcdir)/../support/set/set.c \
	sor.c sor.h stdpccts.h sym.h tokens.h

#sor.c stdpccts.h parser.dlg tokens.h err.c : sor.g
#	$(ANTLR) $(AFLAGS) sor.g

#scan.c mode.h : parser.dlg
#	$(DLG) $(DFLAGS) parser.dlg scan.c

#scrub:
#	rm -f *.o core $(GENHFILES) parser.dlg scan.c err.c sor.c sor


EXTRA_DIST= \
	README SorcererMSVC50.dsp SorcererMSVC50.dsw SorcererMSVC50.mak \
	SorcererMSVC60.dsp SorcererMSVC60.dsw UPDATES makefile.VMS \
	makefile1 msvc.dsw parser.dlg sor.g sor.r sor68K.make \
	sorPPC.make

