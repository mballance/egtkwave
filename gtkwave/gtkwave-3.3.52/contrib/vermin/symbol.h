#ifndef SYMBOL_H
#define SYMBOL_H

#include "vpp_common.h"
#include "jrb.h"

struct i_symbol_scope
{
struct i_symbol_scope *parent;
JRB symtable;
};


struct i_symbol *sym_find(char *s);
struct i_symbol *sym_make(char *s);
struct i_symbol *sym_add(char *s);

extern struct i_symbol_scope *sym_base;

/*
 * for defines, etc
 */
extern JRB define_tree;
extern struct ifdef_stack_t *ifdef_stack_top;

#endif

