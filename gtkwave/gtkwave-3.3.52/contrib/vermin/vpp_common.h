#ifndef VPP_COMMON_H
#define VPP_COMMON_H

/*
 * first, the include files...
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>


/*
 * then, some globally accessible defines...
 */
#define VERNAME "Vermin: Verilog Parser v0.3.1 (w)1999-2008 BSI"


/*
 * structs
 */
struct vpp_filename
{
struct vpp_filename *next;
char *name;

int numchildren;
char **children;
};

struct stringchain_t
{
struct stringchain_t *next;
char *name;
};

struct include_stack_t
{
struct include_stack_t *next;
char *zzfilename;
int zzline;
FILE *fh;
struct preproc_buff_t *preproc_buff_head, *preproc_buff_current;
};


struct preproc_buff_t
{
struct preproc_buff_t *next;

char *str;
int len, pos;
};


struct ifdef_stack_t
{
struct ifdef_stack_t *next;

char *zzfilename;
int zzline;
char *deftext;

int do_not_translate;

unsigned polarity : 1; /* 1 = ifdef, 0 = ifndef (for error reporting in verilog.g) */
unsigned else_branch : 1; /* 0 = don't translate else, 1 = translate else */
unsigned blocked : 1; /* blocks propagate to child ifdefs */
};


struct macro_stack_t
{
struct macro_stack_t *next;

int remaining;
char *macname;
char *deftext;
};


#define TRUE  1
#define FALSE 0

extern struct vpp_filename *vlog_filenames; 	/* list of verilog files to process */
extern struct vpp_filename *incchain;		/* list of include directories */
extern int include_stack_depth;

extern int zzLexErrCount; 			/* inside dlg */

extern FILE *mgetchar_fout;			/* for mgetchar_outhandle */

/*
 * prototypes...
 */
void do_comment_count(int force_print, int count);
int mprintf(const char *fmt, ... );
int mgetstr(char *buf, int maxsiz);
int mgetchar(void);
int mgetchar_outhandle(void);
void mesg(const char *format, ...);
void warn(const char *format, ...);
int vpp_main(FILE *f, const char *name);
void warn_about_translation_off(void);

void store_define(char *def_text);
void remove_define(char *def_text);
void handle_ifdef(char *def_text);
void handle_ifndef(char *def_text);
void handle_else(void);
void handle_elsif(char *def_text);
void handle_endif(void);
void handle_include(char *def_text);
int pop_include(void);

int is_builtin_define(const char *str, unsigned int len);       /* from gperf */
unsigned int atoi_with_underscores(char *s);			/* gives saturation warnings for >= (1<<32) */

int compar_cstarstar_bsearch(const void *v1, const void *v2);
int dirscan(char *path, struct vpp_filename *fn);		/* do dir read on given directory */

void addtoken(int is_newline);
int verilog_keyword_code(const char *s, unsigned int len);
void parsecomment(void);

#endif

