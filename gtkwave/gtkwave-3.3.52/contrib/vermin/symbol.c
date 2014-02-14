#include "inter.h"
#include "symbol.h"
#include "jrb.h"

struct i_symbol_scope *sym_base=NULL;	/* points to current scope, you look backward (up) from here */


/*
 * Simply look for a symbol's presence in the table...
 */
struct i_symbol *sym_find(char *s)
{
JRB node;
struct i_symbol_scope *ss = sym_base;

if(!ss)	/* this is only to avoid crashes */
	{
        ss = (struct i_symbol_scope *)calloc(1, sizeof(struct i_symbol_scope));
        ss->symtable = make_jrb();
        sym_base = ss;
	}

while(ss)
	{
	node = jrb_find_str(ss->symtable, s);
	if(node)
		{
		return((struct i_symbol *)(node->val.v));
		}
	ss=ss->parent;
	}

return(NULL); /* not found */
}


/*
 * Malloc a symbol and copy the name data into it.
 */
struct i_symbol *sym_make(char *s)
{
struct i_symbol *sym;

sym=(struct i_symbol *)calloc(1,sizeof(struct i_symbol));
sym->firstref=zzline;
strcpy(sym->name=(char *)malloc(strlen(s)+1),s);

return(sym);
}

/*
 * Add a symbol presence in the table...
 */
struct i_symbol *sym_add(char *s)
{
struct i_symbol *sym;
Jval j;

sym = sym_find(s);
if(!sym)
	{
	sym = sym_make(s);
	j.v = (void *)sym;
	jrb_insert_str(sym_base->symtable, sym->name, j);
	}

return(sym); 
}

