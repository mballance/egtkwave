#ifndef INTER_H
#define INTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#if !defined __MINGW32__ && !defined _MSC_VER
#include <sys/errno.h>
#endif
#include "attrib.h"
#include "antlr.h"   
#include "keyword_tokens.h"

struct i_explist 
{
struct i_explist *next;
struct i_primary *item;
};


enum SymTypeBits { SYMT_INPUT_B, SYMT_OUTPUT_B, SYMT_INOUT_B, SYMT_ARRAY_B, SYMT_PARAM_B, 
			SYMT_WIRE_B, SYMT_REG_B, SYMT_TIME_B, SYMT_IVAL_B, SYMT_RVAL_B, SYMT_EVENT_B };

#define SYMT_INPUT (1UL<<SYMT_INPUT_B)
#define SYMT_OUTPUT (1UL<<SYMT_OUTPUT_B)
#define SYMT_INOUT (1UL<<SYMT_INOUT_B)
#define SYMT_ARRAY (1UL<<SYMT_ARRAY_B)
#define SYMT_PARAM (1UL<<SYMT_PARAM_B)
#define SYMT_WIRE (1UL<<SYMT_WIRE_B)
#define SYMT_REG (1UL<<SYMT_REG_B)
#define SYMT_TIME (1UL<<SYMT_TIME_B)
#define SYMT_IVAL (1UL<<SYMT_IVAL_B)
#define SYMT_RVAL (1UL<<SYMT_RVAL_B)
#define SYMT_EVENT (1UL<<SYMT_EVENT_B)

struct i_symbol
{
char *name;             /* actual name */

char *fileref;		/* filename of first reference */
int firstref;           /* linenumber of first reference */

unsigned int type;

int lowindex, highindex;		/* for arrays */
union
	{
	struct
		{
		unsigned int num_bits;	/* set this == 0 for unindexed netnames */
		int msb, lsb;
		} vecinfo;
	struct i_primary *value;	/* for parameter substitution */
	} u;
};


struct i_oper
{
int prec;
int token;
};

enum NumValues { NV_IVAL, NV_RVAL, NV_DBASE, NV_BBASE, NV_OBASE, NV_HBASE, NV_STRING };

struct i_number
{
int numtype;

union
	{
	int ival;
	double rval;
	struct { int len; char *val; unsigned specified_width_flag:1; } base;
	} num;
};


struct i_function_call
{
int i_bogus;
};

struct i_mintypmax_exp
{
int i_bogus;
};


enum PrimValues { PRIM_NUMBER, PRIM_SYMBOL, PRIM_SYMBIT, PRIM_SYMRANGE,
			PRIM_CONCAT, PRIM_FUNC, PRIM_MINTYPMAX, PRIM_EXP,
			PRIM_NAMEDPARAM 
		};

struct i_primary
{
int primtype;

union
	{
	struct i_number *number;
	struct i_symbol *symbol;
	struct i_function_call *func;
	struct i_mintypmax_exp *min;
	void *generic;

	struct	{
		struct i_primary *prim1;	/* null for unary */
		struct i_oper    *oper;
		struct i_primary *prim2;
		} binexp;

	struct
		{				/* (a:b:c) operator */
		struct i_primary *m1;
		struct i_primary *m2;
		struct i_primary *m3;
		} mintypmax;

	struct
		{
		struct i_symbol *sym;  	  	/* reference to symbol table name */
		struct i_primary *bitnum; 	/* which bit */
		} symbit;

	struct
		{
		struct i_symbol *sym;  	  	/* reference to symbol table name */
		struct i_primary *msb; 		/* leftmost bit */
		struct i_primary *lsb; 		/* rightmost bit */
		} symrange;

	struct
		{
		struct i_primary *count;	/* repeat count (NULL='1') */
		struct i_explist *exp;		/* list of primaries */
		} concat;

	struct
		{				/* function call */
		struct i_symbol *name;
		struct i_explist *args;
		} funcall;

	struct					/* named parameter */
		{
		struct i_symbol *sym;
		struct i_primary *exp;		
		} named_param;

	} primval;
};


struct i_oper *i_oper_make(int token, int prec);
struct i_number *i_number_make(int ival);
struct i_number *i_number_fmake(double rval);
struct i_number *i_number_basemake(int typ, char *text);
struct i_primary *i_primary_make(int typ, void *vpnt);
struct i_primary *i_primary_mintypmax_make(struct i_primary *m1, struct i_primary *m2, struct i_primary *m3);
struct i_primary *i_bin_expr_make(struct i_primary *p1, struct i_oper *o1, struct i_primary *p2);
struct i_primary *i_primary_symbit_make(struct i_symbol *sym, struct i_primary *pri);
struct i_primary *i_primary_symrange_make(struct i_symbol *sym, struct i_primary *msb, struct i_primary *lsb);
struct i_explist *i_explist_make(struct i_explist *explist);
struct i_primary *i_primary_concat_make(struct i_primary *count, struct i_explist *exp);
struct i_primary *i_primary_funcall_make(struct i_symbol *name, struct i_explist *args);


void print_primary(struct i_primary *pri);
void print_explist(struct i_explist *e);
void print_concat(struct i_primary *c, struct i_explist *e);

#endif

