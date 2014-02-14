#undef DEBUG_SHRED

#include "shred.h"
#include "attrib.h"
#include "antlr.h"
#include "tokens.h"
#include "symbol.h"

#ifndef DEBUG_SHRED
void DEBUG(char *dummy, ...) { }
#endif

void **shred_root=NULL, **shred_pnt=NULL;
void **exp_root=NULL, **exp_pnt=NULL;
void ***exp_now_root=NULL, ***exp_now_pnt=NULL;


void shred_alloc(void)
{
shred_root=shred_pnt=(void **)calloc(SHRED_ROOT_SIZE,sizeof(void *));	/* with 12+ preclevels this is more than enough */
exp_root=exp_pnt=(void **)calloc(EXP_ROOT_SIZE,sizeof(void *));		/* dictates longest expr */
exp_now_root=exp_now_pnt=(void ***)calloc(EXP_NOW_SIZE,sizeof(void *)); /* levels of parentheses */
}

void shred_free(void)
{
free(exp_root);
free(shred_root);
}

void push_exp_now(void)
{
exp_now_pnt++;
if(exp_now_pnt==(exp_now_root+EXP_NOW_SIZE))
	{
	mesg("\n** Error: overflow of %d element parenthesis stack\n", EXP_NOW_SIZE);
	zzerrors+=zzLexErrCount;
	warn("** %d error%s detected, exiting.\n\n",zzerrors,(zzerrors>1)?"s were":" was");
	exit(255);
	}
*exp_now_pnt=(void **)exp_pnt;
}

void pop_exp_now(void)
{
exp_pnt=(void **)(*exp_now_pnt);
exp_now_pnt--;
}

void push_primary(struct i_primary *pri)
{
exp_pnt++;
if(exp_pnt==(exp_root+EXP_ROOT_SIZE))
	{
	mesg("\n** Error: overflow of %d element expression stack\n", EXP_ROOT_SIZE);
	zzerrors+=zzLexErrCount;
	warn("** %d error%s detected, exiting.\n\n",zzerrors,(zzerrors>1)?"s were":" was");
	exit(255);
	}
*exp_pnt=(void *)pri;
}

void push_oper(struct i_oper *oper)
{
exp_pnt++;
*exp_pnt=(void *)oper;
}

struct i_primary *pop_primary(void)
{
struct i_primary *pri;

pri=(struct i_primary *)(*exp_pnt);
exp_pnt--;
return(pri);
}

struct i_oper *pop_oper(void)
{
struct i_oper *oper;

oper=(struct i_oper *)(*exp_pnt);
exp_pnt--;
return(oper);
}

struct i_primary *shred_expression(void)
{
void **input_stream;
void **shredlocal;
struct i_primary *p1, *p2;
struct i_oper    *o1, *o2;
int prec1, prec2;
struct i_primary *prdest;

DEBUG("Parsing Expression\n");
input_stream=exp_pnt;
shredlocal=shred_pnt;

p1=*(++shredlocal)=*(++input_stream);	/* 1st primary */

shred_main:
o1=*(++shredlocal)=*(++input_stream);	/* 1st op      */
p2=*(++shredlocal)=*(++input_stream);	/* 2nd op      */

shredder:
DEBUG("----------\n");

#ifdef DEBUG_SHRED
print_primary(p1);
print_operator(o1);
#endif

if(o1->token==V_EOF)
	{
	DEBUG("End of expression encountered\n\n");
	return(p1);
	}

o2=*(input_stream+1);

#ifdef DEBUG_SHRED
print_primary(p2);
print_operator(o2);
#endif

prec1=o1->prec; prec2=o2->prec;
DEBUG("O1 prec: %d\n",prec1);
DEBUG("O2 prec: %d\n",prec2);

if(prec2>prec1)
	{
	DEBUG("Shift\n");

	*(++shredlocal)=o2;
	input_stream++;
	p1=p2;
	o1=o2;	
	p2=*(++shredlocal)=*(++input_stream);	/* 2nd op      */
	goto shredder;
	}
	else
	{
	DEBUG("Reduce\n");

	prdest=i_bin_expr_make(p1,o1,p2);

	shredlocal-=2;

	p1=*(shredlocal)=prdest;
	if(shredlocal==shred_pnt+1)
		{
		goto shred_main;		
		}
		else	
		{
		p2=p1;
		p1=*(shredlocal-2);
		o1=*(shredlocal-1);
		goto shredder;
		}
	}
}

