#include "inter.h"
#include "symbol.h"

/*
 * malloc() and fill in various i_structs..
 */
struct i_oper *i_oper_make(int token, int prec)
{
struct i_oper *pnt;

pnt=(struct i_oper *)malloc(sizeof(struct i_oper));
pnt->prec=prec;
pnt->token=token;

return(pnt);
}

struct i_number *i_number_make(int ival)
{
struct i_number *pnt;

pnt=(struct i_number *)calloc(1,sizeof(struct i_number));
pnt->numtype=NV_IVAL;
pnt->num.ival=ival;
return(pnt);
}

struct i_number *i_number_fmake(double rval)
{
struct i_number *pnt;

pnt=(struct i_number *)calloc(1,sizeof(struct i_number));
pnt->numtype=NV_RVAL;
pnt->num.rval=rval;
return(pnt);
}


struct i_number *i_number_basemake(int typ, char *text)
{
struct i_number *pnt;
char *s;

pnt=(struct i_number *)calloc(1,sizeof(struct i_number));
pnt->numtype=typ;

if(typ != NV_STRING)
	{
	int specified_width = text[0]!='\'';

	pnt->num.base.specified_width_flag=specified_width;	/* when set, says that .len was specified in the verilog, else val is synthesized (32) */
	pnt->num.base.len= specified_width ? atoi_with_underscores(text) : 32;

	s=text;
	while(*(s++)!='\'');
	s++;
	pnt->num.base.val=strdup(s);

	if(!specified_width)
		{
		warn("** Warning: constant %s width unspecified in file '%s' line %d, defaulting to 32 bits\n", text, zzfilename, zzline);
		}
	
	free(text);
	}
	else
	{
	char *p = text;
	char *p2 = text;
	int octval;

	while(*p)
		{
		if(*p!='\\')
			{
			*p2=*p;
			}
			else
			{
			p++;
			switch(*p)
				{
				case 't': *p2 = '\t'; break;
				case 'n': *p2 = '\n'; break;
				case '\\': *p2 = '\\'; break;

				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7': 
					octval = *p - '0';
					if((*(p+1)>='0')&&(*(p+1)<='7'))
						{
						p++;
						octval = 8*octval + (*p - '0');
						if((*(p+1)>='0')&&(*(p+1)<='7'))
							{
							p++;
							octval = 8*octval + (*p - '0');
							}
						}

					*p2 = octval;
					break;

				default:  	*(p2++) = '\\';
					  	*p2 = *p;
						break;
				}

			}

		p++;
		p2++;
		}
	*p2=0;	

	pnt->num.base.len=8*(p2-text);
	pnt->num.base.val=text;
	/* warn("** Info: bits %d, string constant '%s'\n",  pnt->num.base.len, pnt->num.base.val); */
	}
	
return(pnt);
}


struct i_primary *i_primary_make(int typ, void *vpnt)
{
struct i_primary *pnt;

pnt=(struct i_primary *)calloc(1,sizeof(struct i_primary));
pnt->primtype=typ;
pnt->primval.generic=vpnt;

return(pnt);
}

struct i_primary *i_primary_mintypmax_make(struct i_primary *m1, struct i_primary *m2, struct i_primary *m3)
{
struct i_primary *pnt;

pnt=(struct i_primary *)calloc(1,sizeof(struct i_primary));
pnt->primtype=PRIM_MINTYPMAX;
pnt->primval.mintypmax.m1 = m1;
pnt->primval.mintypmax.m2 = m2;
pnt->primval.mintypmax.m3 = m3;

return(pnt);
}

struct i_primary *i_bin_expr_make(struct i_primary *p1, struct i_oper *o1, struct i_primary *p2)
{
struct i_primary *prdest;

prdest=(struct i_primary *)calloc(1,sizeof(struct i_primary));
prdest->primtype=PRIM_EXP;
prdest->primval.binexp.prim1 = p1;
prdest->primval.binexp.prim2 = p2;
prdest->primval.binexp.oper  = o1;
return(prdest);
}

struct i_primary *i_primary_symbit_make(struct i_symbol *sym, struct i_primary *pri)
{
struct i_primary *pnt;
pnt=(struct i_primary *)calloc(1,sizeof(struct i_primary));
pnt->primtype=PRIM_SYMBIT;
pnt->primval.symbit.sym=sym;
pnt->primval.symbit.bitnum=pri;
return(pnt);
}

struct i_primary *i_primary_symrange_make(struct i_symbol *sym, struct i_primary *msb, struct i_primary *lsb)
{
struct i_primary *pnt;
pnt=(struct i_primary *)calloc(1,sizeof(struct i_primary));
pnt->primtype=PRIM_SYMRANGE;
pnt->primval.symrange.sym=sym;
pnt->primval.symrange.msb=msb;
pnt->primval.symrange.lsb=lsb;
return(pnt);
}

struct i_explist *i_explist_make(struct i_explist *explist)
{
return(explist);  /* for now..later this will traverse through and 
			print the individual structs out to the 
			intermediate file
		   */
}


struct i_primary *i_primary_concat_make(struct i_primary *count, 
		struct i_explist *exp)
{
struct i_primary *pnt;
pnt=(struct i_primary *)calloc(1,sizeof(struct i_primary));
pnt->primtype=PRIM_CONCAT;
pnt->primval.concat.count=count;
pnt->primval.concat.exp=exp;
return(pnt);
}

struct i_primary *i_primary_funcall_make(struct i_symbol *name, 
		struct i_explist *args)
{
struct i_primary *pnt;
pnt=(struct i_primary *)calloc(1,sizeof(struct i_primary));
pnt->primtype=PRIM_FUNC;
pnt->primval.funcall.name=name;
pnt->primval.funcall.args=args;
return(pnt);
}




/*
 * print various i_structs out..
 */
void print_operator(struct i_oper *o)
{
int op;

op=o->token;

switch(op)
	{
	case V_QUEST:	printf("?\n"); break;
	case V_COLON:	printf(":\n"); break;
	case V_PLUS:	printf("+\n"); break;
	case V_MINUS:	printf("-\n"); break;
	case V_BANG: 	printf("!\n"); break;
	case V_TILDE:	printf("~\n"); break;
	case V_AND:	printf("&\n"); break;
	case V_NAND:	printf("~&\n"); break;
	case V_OR:	printf("|\n"); break;
	case V_NOR:	printf("~|\n"); break;
	case V_XOR:	printf("^\n"); break;
	case V_XNOR:	printf("~^\n"); break;
	case V_STAR:	printf("*\n"); break;
	case V_SLASH:	printf("/\n"); break;
	case V_MOD:	printf("%%\n"); break;
	case V_SHL:	printf("<<\n"); break;
	case V_SHR:	printf(">>\n"); break;
	case V_SSHL:	printf("<<<\n"); break;
	case V_SSHR:	printf(">>>\n"); break;
	case V_POW:	printf("**\n"); break;
	case V_LT:	printf("<\n"); break;
	case V_LEQ:	printf("<=\n"); break;
	case V_GT:	printf(">\n"); break;
	case V_GEQ:	printf(">=\n"); break;
	case V_EQ2:	printf("==\n"); break;
	case V_NEQ:	printf("!=\n"); break;
	case V_EQ3:	printf("===\n"); break;
	case V_NEQ2:	printf("!==\n"); break;
	case V_AND2:	printf("&&\n"); break;
	case V_OR2:	printf("||\n"); break;
	case V_EOF:	printf("EOF\n"); break;
	default:	printf("Unknown: %d\n",op); break;
	}
}


void print_primary(struct i_primary *pri)
{
if(pri)
	{
	if(pri->primtype==PRIM_FUNC)
		{
		printf("PRIM: FUNCALL (%s)\n",pri->primval.funcall.name->name);
		printf("ARGS vvvvvvvvvvvv\n");
		print_explist(pri->primval.funcall.args);
		printf("ARGS ^^^^^^^^^^^^\n");
		}
	else
	if(pri->primtype==PRIM_CONCAT)
		{
		print_concat(pri->primval.concat.count,
			     pri->primval.concat.exp);
		}
	else
	if(pri->primtype==PRIM_SYMBIT)
		{
		printf("PRIM: SYMBIT (%s)\n",pri->primval.symbit.sym->name);
		printf("vvvvvvvvvvvv\n");
		print_primary(pri->primval.symbit.bitnum);
		printf("^^^^^^^^^^^^\n");
		}
	else
	if(pri->primtype==PRIM_SYMRANGE)
		{
		printf("PRIM: RANGE (%s)\n",pri->primval.symrange.sym->name);
		printf("msb vvvvvvvvvvvv\n");
		print_primary(pri->primval.symrange.msb);
		printf("msb ^^^^^^^^^^^^\n");
		printf("lsb vvvvvvvvvvvv\n");
		print_primary(pri->primval.symrange.lsb);
		printf("lsb ^^^^^^^^^^^^\n");
		}
	else
	if(pri->primtype==PRIM_SYMBOL)
		{
		printf("PRIM: SYMBOL (%s)\n",pri->primval.symbol->name);
		}
	else
	if(pri->primtype==PRIM_EXP)
		{
		printf("PRIM: EXPRESSION\n");
		}
	else
	if(pri->primtype==PRIM_MINTYPMAX)
		{
		printf("PRIM: MINTYPMAX\n");
		}
	else
	if(pri->primtype==PRIM_NUMBER)
		{
		if(pri->primval.number)
		{
		if(pri->primval.number->numtype==NV_IVAL)
			{
			printf("PRIM: %d\n",pri->primval.number->num.ival);
			}
			else if(pri->primval.number->numtype==NV_RVAL)
			{
			printf("PRIM: %.16g\n",pri->primval.number->num.rval);
			}
			else
			{
			printf("PRIM: %d %s\n",pri->primval.number->num.base.len,
				pri->primval.number->num.base.val);
			}
		}
		else
		{
		printf("NON-NUMBER PRIMARY\n");
		};
		}
		else
		{
		printf("YET UNSUPPORTED PRIMARY\n");
		}
	}
	else
	{
	printf("NULL PRIMARY\n");
	}
}


void print_explist(struct i_explist *e)
{
printf("EXPLIST VVV ---------------------------------------------\n");
while(e)
	{
	print_primary(e->item);
	e=e->next;
	}
printf("EXPLIST ^^^ ---------------------------------------------\n");
}

void print_concat(struct i_primary *c, struct i_explist *e)
{
printf("CONCAT VVV ---------------------------------------------\n");
printf("-- COUNT VVV -------------------------------------------\n");
print_primary(c);
printf("-- COUNT ^^^ -------------------------------------------\n");
print_explist(e);
printf("CONCAT ^^^ ---------------------------------------------\n");
}

