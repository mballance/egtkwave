#ifndef SHRED_H
#define SHRED_H

#include "inter.h"

#ifdef DEBUG_SHRED
#define DEBUG   printf
#endif

#ifndef DEBUG_SHRED
#define DEBUG   blackhole_shred
void    DEBUG(char *, ...);
#endif

#define SHRED_ROOT_SIZE 16384
#define EXP_ROOT_SIZE 65536
#define EXP_NOW_SIZE 4096

extern void **shred_root, **shred_pnt;
extern void **exp_root, **exp_pnt;
extern void ***exp_now_root, ***exp_now_pnt;

struct i_primary *shred_expression(void);

void shred_alloc(void);
void shred_free(void);
void push_primary(struct i_primary *pri);
void push_oper(struct i_oper *oper);
struct i_primary *pop_primary(void);
struct i_oper *pop_oper(void);
void push_exp_now(void);
void pop_exp_now(void);

#endif

