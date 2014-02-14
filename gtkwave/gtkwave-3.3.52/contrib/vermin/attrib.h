#ifndef ATTRIB_H
#define ATTRIB_H

#define ZZA_STACKSIZE 100000
#define ZZAST_STACKSIZE 100000

extern int zzline, zzrewind;
extern int zzerrors;
extern int zzcomment_start, zzcomment_depth, zzcomment_entry;
extern char *zzfilename;
extern char *lineroot, *linepos;
extern char *prevlineroot;
extern int linemaxlen;

typedef union
{
        int ival;
        double rval;
        char *text;
        struct i_oper *oper;
        struct i_primary *prim;
        struct i_number *num;
	struct i_symbol *symbol;
	struct i_explist *explist;
} Attrib;

#define USER_ZZSYN

extern int do_not_translate, do_not_translate_mask;
#define STMODE do{if(do_not_translate)zzskip();}while(0)
#define STMODE_XLATEOFF_SYNOPSYS  1
#define STMODE_XLATEOFF_SYNTHESIS 2
#define STMODE_XLATEOFF_VERILINT  4
#define STMODE_XLATEOFF_VERTEX    8
#define STMODE_XLATEOFF_IFDEF     16

#endif

