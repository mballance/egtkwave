## -*- makefile -*-
##

bin_PROGRAMS= vermin

PCCTS = ../pccts
ANTLR_H = $(srcdir)/../pccts/h
ANTLR = $(PCCTS)/antlr/antlr
DLG = $(PCCTS)/dlg/dlg
AFLAGS = -ga -k 2 -gl
DFLAGS = -ga -C2 -i

AM_CFLAGS = -I. -I$(ANTLR_H) -I$(top_srcdir)
AM_LIBS=	-lm

vermin_SOURCES= \
	args.c attrib.h definehash.c err.c inter.c inter.h jrb.c jrb.h \
	keyword_tokens.h keywordhash.c mode.h preproc.c scan.c \
	symbol.c symbol.h tokens.h verilog.c vpp_common.h shred.h shred.c

verilog.c parser.dlg tokens.h err.c : $(srcdir)/verilog.g
	$(ANTLR) $(AFLAGS) $(srcdir)/verilog.g

scan.c mode.h : parser.dlg
	$(DLG) $(DFLAGS) parser.dlg scan.c

definehash.c: vpp_keyword.gperf
	$(GPERF) -C -I -N is_builtin_define $(srcdir)/vpp_keyword.gperf >definehash.c

keywordhash.c: verilog_keyword.gperf
	printf "$(GPERF) -o -i 1 -C -k 1-3,\044 -L C -H keyword_hash -N check_identifier -tT $(srcdir)/verilog_keyword.gperf >keywordhash.c" | sh

BUILT_SOURCES= definehash.c keywordhash.c tokens.h verilog.c tokens.h err.c parser.dlg

shred.$(OBJEXT) : tokens.h

EXTRA_DIST= \
	parser.dlg verilog.g verilog_keyword.gperf vpp_keyword.gperf

