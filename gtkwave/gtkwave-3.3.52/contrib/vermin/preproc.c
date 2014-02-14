#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "vpp_common.h"
#include "attrib.h"
#include "jrb.h"

struct vpp_filename *vlog_filenames=NULL; /* list of verilog files to process */
struct vpp_filename *incchain=NULL;	  /* list of include directories */

extern char *zzfilename;
extern int zzline;
extern int zzerrors;
extern int zzrewind;
static FILE *fh = NULL;
FILE *mgetchar_fout = NULL;

JRB define_tree = NULL;
struct ifdef_stack_t *ifdef_stack_top = NULL;
struct include_stack_t *include_stack_top = NULL;
int include_stack_depth = 0;
struct macro_stack_t *mac_nest_head = NULL;

/*
 * strcmp for char** qsort/bsearches
 */
static int compar_cstarstar(const void *v1, const void *v2)
{
const char **s1 = (const char **)v1, **s2 = (const char **)v2;
return(strcasecmp(*s1, *s2));
}


int compar_cstarstar_bsearch(const void *v1, const void *v2)
{
const char *s1 = (const char *)v1;
const char **s2 = (const char **)v2;
return(strcasecmp(s1, *s2));
}


/*
 * iterate on given directory (for -yi command opt)
 */
int dirscan(char *path, struct vpp_filename *fn)
{
DIR *dp = opendir(path);
struct dirent *dirp;
int pathlen = strlen(path);
int nument = 0;
struct stringchain_t *sc=NULL, *sct;
int i;

if(dp)
	{
	while((dirp=readdir(dp)))
		{
		char *fullpath = malloc(pathlen + 1 + strlen(dirp->d_name) + 1);
		struct stat buf;		

		sprintf(fullpath, "%s/%s", path, dirp->d_name);
		if(!stat(fullpath, &buf))
			{	
			if(S_ISREG(buf.st_mode))
				{
				sct = (struct stringchain_t *)calloc(1, sizeof(struct stringchain_t));
				sct->name = strdup(dirp->d_name);
				sct->next = sc;
				sc = sct;
				nument++;
				}
			}
		free(fullpath);
		}

	if(nument)
		{
		fn->numchildren = nument;
		fn->children = malloc(nument*sizeof(char *));

		for(i=0;i<nument;i++)
			{
			fn->children[i] = sc->name;
			sct = sc->next;
			free(sc);
			sc = sct;
			}

		qsort(fn->children, nument, sizeof(char **), compar_cstarstar);
		}
	}

return(dp!=NULL);
}


/*
 * generic maintenance functions...
 */
void vpp_update_yyfilename(const char *str)
{
if(!zzfilename)
        {
        zzfilename=malloc(strlen(str)+1);
        strcpy(zzfilename, str);
        }
        else
        {
        free(zzfilename);
        zzfilename=malloc(strlen(str)+1);
        strcpy(zzfilename, str);
        }
}


void vpp_update_yylineno(const char *str)
{
zzline = atoi(str) - 1;       /* because \n follows this vpp directive! */
zzrewind=0;
}


unsigned int atoi_with_underscores(char *s)
{
const unsigned long long max32 = 4294967295ULL;

unsigned long long val;
unsigned int chval;

val = 0ULL;
while((*s) && (*s!='\''))
        {
        if(*s!='_')
                {
		chval = ((*s) -'0');
                val *= 10ULL;
                val += ((unsigned long long)chval);
                if(val>max32)
                        {
                        warn("** Warning: Large constant truncated to MAXINT in file '%s' line %d\n", zzfilename, zzline);
                        return((unsigned int)max32);
                        }
                }

	s++;
        }

return((unsigned int)val);
}


/*
 * Print a message on stderr and increment the zzerrors counter.  
 */
void mesg(const char *format, ...)
{
   va_list ap;
   va_start(ap, format);
   vfprintf(stderr, format, ap);
   zzerrors += 1;               
   va_end(ap);
}
        

/*
 * Print a message on stderr but do NOT increment zzerrors
 */
void warn(const char *format, ...)
{
   va_list ap;
   va_start(ap, format);
   vfprintf(stderr, format, ap);
   va_end(ap);
}


/*
 * printf to memory..
 */
struct preproc_buff_t *preproc_buff_head=NULL, *preproc_buff_current=NULL;

int mprintf(const char *fmt, ... )
{
int rc;
va_list args;
struct preproc_buff_t *bt = (struct preproc_buff_t *)calloc(1, sizeof(struct preproc_buff_t));
char buff[65537];

va_start(args, fmt);
rc=vsprintf(buff, fmt, args);
/* vfprintf(stderr, fmt, args); */
bt->len = strlen(buff);
bt->str = malloc(bt->len+1);
strcpy(bt->str, buff);

if(!preproc_buff_current)
	{
	preproc_buff_head = preproc_buff_current = bt;
	}
	else
	{
	preproc_buff_current->next = bt;
	preproc_buff_current = bt;	
	}

va_end(args);
return(rc);
}


/*
 * get maxsiz chars from memory and copy into buf
 * (return actual number of characters copied)
 */
int mgetstr(char *buf, int maxsiz)
{
struct preproc_buff_t *bt = preproc_buff_head;
int cpylen;

if(!bt) return(0);

cpylen = bt->len-bt->pos;
if(cpylen <= maxsiz)
	{
	memcpy(buf, bt->str + bt->pos, cpylen);
	free(bt->str);

	if(bt != preproc_buff_current)
		{
		preproc_buff_head = bt->next;
		}
		else
		{
		preproc_buff_head = preproc_buff_current = NULL;
		}
	free(bt);
	}
	else
	{
	memcpy(buf, bt->str + bt->pos, maxsiz);
	bt->pos += (cpylen=maxsiz);
	}

return(cpylen);
}


/*
 * routines which get a single char and pushback define stuff
 * into the memory buffer as needed
 */
int mgetchar2(void)
{
unsigned char cbuf[2];

if(preproc_buff_head)
	{
	mgetstr((char *)cbuf, 1);
	return(cbuf[0]);
	}

if(fh)
	{
	int rc = fgetc(fh);
	if(rc==EOF)		/* need to do include file handling here */
		{
		fclose(fh);
		fh=NULL;
		}
		else
		{
		return(rc);
		}
	}

if(pop_include())
	{
	return(mgetchar2());
	}
else
while(vlog_filenames)
	{
	struct vpp_filename *vt=vlog_filenames;
	struct stat buf;
	int stat_rc = stat(vt->name, &buf);
	int isdir = (!stat_rc)&&(S_ISDIR(buf.st_mode));

	warn("Processing file '%s' ...\n", vt->name);
	
	fh=fopen(vt->name, "rb");
	if(!fh)
		{
		mesg("** Error: could not open file '%s'\n", vt->name);
		}
		else
		{
		if(!isdir)
			{
			mprintf("// vpp_file %s \n", vt->name);
			mprintf("// vpp_line %d\n", 1);
			}
			else
			{
			/* skip as it's a directory */
			fclose(fh); fh = NULL;
			}
		}

	free(vt->name);
	vlog_filenames = vlog_filenames->next;
	free(vt);

	if(fh) return(mgetchar2());
	}
	
return(EOF);
}

int mgetchar(void)
{
int rc = mgetchar2();
int len;
char buf[1025];
struct preproc_buff_t *bt;
struct macro_stack_t *mac;
static char inside_skip = 0;
static int prevrc = 0;

if((mac_nest_head) /* &&(!(--mac_nest_head->remaining)) */)
	{
	free(mac_nest_head->macname);
	free(mac_nest_head->deftext);
	mac=mac_nest_head;
	mac_nest_head=mac_nest_head->next;
	free(mac);
	}

switch(inside_skip)	/* gate out macro expansion inside comments/strings */
	{
	case 0: if(rc=='/') inside_skip = 1;
		else if(rc=='\"') { prevrc = 0; inside_skip = 5; }
		break;

	case 1: if(rc=='*') inside_skip = 2;
		else if(rc=='/') inside_skip = 3;
		else inside_skip = 0;
		break;

	case 2: if(rc=='*') inside_skip = 4;
		else if(rc==EOF) inside_skip = 0;
		break;

	case 3: if((rc=='\n')||(rc==EOF)) inside_skip = 0;
		break;

	case 4: if((rc=='/')||(rc==EOF)) inside_skip = 0;
		else if(rc!='*') inside_skip = 2;
		break;

	case 5: 
	default:
		if( ((rc=='\"')&&(prevrc!='\\')) 
			||(rc=='\n')||(rc=='\r')||(rc==EOF) ) inside_skip = 0;
		else
		if(rc=='\"')
			{
			bt = (struct preproc_buff_t *)calloc(1, sizeof(struct preproc_buff_t));
			bt->len = 2;
			bt->str = strdup("42");		/* octal for quote */
			bt->next = preproc_buff_head;        
			preproc_buff_head = bt;
			rc = '0';			/* give octal char this pass */
			}

		prevrc = (prevrc!='\\') ? rc : 0;	/* to handle \\" at end of string properly */
		break;
	}


if((rc!='`')||(inside_skip))
	{
	return(rc);
	}
	else
	{
	len=0;

	while((rc=mgetchar2())!=EOF)
		{
		if	(
			((rc>='a')&&(rc<='z')) ||
			((rc>='A')&&(rc<='Z')) ||
			((rc>='0')&&(rc<='9')) ||
			(rc=='_') ||
			(rc=='$')
			)
			{
			buf[len++] = rc;
			continue;
			}
			else
			{
			break;
			}
		}	
	buf[len]=0;

	if(rc!=EOF)
		{
		bt = (struct preproc_buff_t *)calloc(1, sizeof(struct preproc_buff_t));
		bt->len = 1;
		bt->str = calloc(2, sizeof(char));
		*bt->str = rc;
		bt->next = preproc_buff_head;        
		preproc_buff_head = bt;
		}
	
	if(len==0)
		{
		return('`');
		}

	if(!is_builtin_define(buf, len))
		{
		JRB node;                       

		if((node=jrb_find_str(define_tree, buf)))
			{
			if(node->val.s)
				{
				bt = (struct preproc_buff_t *)calloc(1, sizeof(struct preproc_buff_t));
				bt->len = strlen(node->val.s);
				bt->str = strdup(node->val.s);
				bt->next = preproc_buff_head;        
				preproc_buff_head = bt;

				mac = (struct macro_stack_t *)calloc(1, sizeof(struct macro_stack_t));
				mac->remaining = bt->len;
				mac->macname = strdup(buf);
				mac->deftext = strdup(bt->str);
				mac->next = mac_nest_head;
				mac_nest_head = mac;

				mac=mac->next;
				while(mac)
					{
					if(!strcmp(mac->macname, mac_nest_head->macname))
						{
						int depth=1;
						mac = mac_nest_head->next;
						mesg("** Error: recursive macro expansion encountered in file '%s' line %d, reverse trace follows\n", zzfilename, zzline);
						do	{
							warn("%7d : `%s = '%s'\n", depth++, mac->macname, mac->deftext);
							mac=mac->next;
							} while(mac);

						zzerrors+=zzLexErrCount;
						warn("\n** %d error%s detected, exiting.\n\n",zzerrors,(zzerrors>1)?"s were":" was");
						exit(255);
						}
					mac=mac->next;
					}
				}
			return(mgetchar());
			}
		}

	bt = (struct preproc_buff_t *)calloc(1, sizeof(struct preproc_buff_t));

	bt->len = len;
	bt->str = strdup(buf);
	bt->next = preproc_buff_head;        
	preproc_buff_head = bt;
	return('`');
	}
}

int mgetchar_outhandle(void)
{
int rc = mgetchar();

if((mgetchar_fout)&&(rc!=EOF))
	{
	fputc(rc, mgetchar_fout);
	}

return(rc);
}


int vpp_main(FILE *f, const char *name)
{
mprintf("// vpp_file %s\n", name);
mprintf("// vpp_line %d\n", 1);
fh = f;
return(0);
}

/*
 * define handling/expansion
 */
static void add_define_to_tree(JRB tree, char *s, char *sval)
{
Jval val;
JRB node;
char *pnt = s;
char ch;

/* see if s is legal */
while((ch=*(pnt++)))
	{
	if	(
		((ch>='a')&&(ch<='z')) ||
		((ch>='A')&&(ch<='Z')) ||
		((ch>='0')&&(ch<='9')) ||
		(ch=='_') ||
		(ch=='$')
		) continue;

	warn("** Warning: skipping invalid define using name '%s' with value '%s' in file '%s' line %d\n", s, sval, zzfilename, zzline);
	return;
	}

if(!(node=jrb_find_str(tree, s)))
        {
        val.s = sval;
        jrb_insert_str(tree, strdup(s), val);
        }
        else
        {
#if 0
	if(zzfilename)
		{
		warn("** Warning: redefining '%s' to '%s' (prev '%s') in file '%s' line %d\n", s, sval ? sval : "", node->val.s ? node->val.s : "", zzfilename, zzline);
		}
		else
		{
		warn("** Warning: redefining '%s' to '%s' (prev '%s')\n", s, sval ? sval : "", node->val.s ? node->val.s : "");
		}
#endif
	if(node->val.s) free(node->val.s);
	node->val.s = sval;
        }
}


void store_define(char *def_text)
{
char *pnt = def_text+7;
char *s1, *s2;
int s1len, i;

s1=pnt;
s1len = strlen(s1);
for(i=1;i<s1len;i++)
        {
        if((s1[i]=='/') && (s1[i-1]=='/'))
                {
                /* trailing comment is at s1+i-1 */
                mprintf("\n// vpp_line %d\n%s\n", zzline, s1+i-1);
                s1[i-1] = 0;
                break;
                }
        }

s1=strtok(pnt, " \t\b");	
if((!s1)||(!*s1))
	{
	warn("** Warning: malformed `define directive in file '%s' line %d, ignoring.\n", zzfilename, zzline);
	return;
	}

s2=strtok(NULL, "");
if((!s2)||(!*s2)) 
	{
	s2=NULL; 
	}
	else 
	{
	char *s2head = s2;

	while(*s2head)
		{
		if((*s2head==' ')||(*s2head=='\t')||(*s2head=='\b')) { s2head++; continue; }
		break;
		}

	if(!*s2head)
		{
		s2=NULL;
		}
		else
		{
		int s2len = strlen(s2head);

		for(i=s2len-1;i>=0;i--)
			{
			if((s2head[i]!=' ')&&(s2head[i]!='\t')&&(s2head[i]!='\b')) { s2head[i+1]=0; break; }
			}
		s2len = i+1;
		if(s2len)
			{
			s2 = strdup(s2head);
			}
			else
			{	
			s2 = NULL;
			}
		}
	}

if(s2)
	{
	int nquotes=0, len=0;
	pnt=s2;
	while(*pnt) 
		{
		if(*pnt=='\"') nquotes++;
		pnt++;
		len++;
		}

	if(nquotes&1)
		{
		pnt = malloc(len+2);
		strcpy(pnt, s2);
		strcat(pnt+len, "\"");
		free(s2);
		s2 = pnt;

		warn("** Warning: malformed `define directive in file '%s' line %d missing endquote, added.\n", zzfilename, zzline);
		}

	len = strlen(s2);
	pnt = s2+len;
	while(pnt != s2)
		{
		pnt--;
		if(isspace((int)(unsigned char)*pnt)) { *pnt = 0; } else { break; }
		}
	}

add_define_to_tree(define_tree, s1, s2);
}


void remove_define(char *def_text)
{
char *pnt = def_text+6;
char *s1;
JRB node;

s1=strtok(pnt, " \t\b");	
if(!s1)
	{
	warn("** Warning: malformed `undef directive in file '%s' line %d, ignoring.\n", zzfilename, zzline);
	return;
	}

if(!(node=jrb_find_str(define_tree, s1)))
	{
	warn("** Warning: Attempted `undef of undefined '%s' in file '%s' line %d, ignoring.\n", s1, zzfilename, zzline);
	}
	else
	{
	free(node->key.s);
	if(node->val.s) free(node->val.s);
	jrb_delete_node(node);
	}
}


/* 
 * ifdef/ifndef/else/endif handling
 */
void handle_ifdef(char *def_text)
{
char *pnt = def_text+6;
char *s1;
/* JRB node; */
struct ifdef_stack_t *is;

/* warn("** IFDEF in file '%s' line %d\n", zzfilename, zzline); */

s1=strtok(pnt, " \t\b");	
if(!s1)
	{
	warn("** Warning: malformed `ifdef directive in file '%s' line %d, ignoring.\n", zzfilename, zzline);
	return;
	}

is = calloc(1, sizeof(struct ifdef_stack_t));
is->polarity = 1;
is->zzfilename = strdup(zzfilename);
is->zzline = zzline;
is->deftext = strdup(s1);
is->blocked = ifdef_stack_top ? ifdef_stack_top->blocked : 0;
is->next = ifdef_stack_top;
is->do_not_translate = do_not_translate;
ifdef_stack_top = is;

	{
	if(!(/* node= */jrb_find_str(define_tree, s1)))
		{
		/* skip action */
		do_not_translate |= STMODE_XLATEOFF_IFDEF;
		is->blocked = 1;
		is->else_branch = 1;	/* translate in `else */
		}
		else
		{
		/* do */
		is->else_branch = 0; /* do not translate in `else */
		}
	}

/* warn(" do_not_translate = %d\n", do_not_translate); */
}


void handle_ifndef(char *def_text)
{
char *pnt = def_text+7;
char *s1;
/* JRB node; */
struct ifdef_stack_t *is;

/* warn("** IFNDEF in file '%s' line %d\n", zzfilename, zzline); */

s1=strtok(pnt, " \t\b");	
if(!s1)
	{
	warn("** Warning: malformed `ifndef directive in file '%s' line %d, ignoring.\n", zzfilename, zzline);
	return;
	}

is = calloc(1, sizeof(struct ifdef_stack_t));
is->polarity = 0;
is->zzfilename = strdup(zzfilename);
is->zzline = zzline;
is->deftext = strdup(s1);
is->next = ifdef_stack_top;
is->do_not_translate = do_not_translate;
is->blocked = ifdef_stack_top ? ifdef_stack_top->blocked : 0;
ifdef_stack_top = is;

	{
	if((/* node= */jrb_find_str(define_tree, s1)))
		{
		/* skip action */
		do_not_translate |= STMODE_XLATEOFF_IFDEF;
		is->blocked = 1;
		is->else_branch = 1;	/* translate in `else */
		}
		else
		{
		/* do */
		is->else_branch = 0; /* do not translate in `else */
		}
	}

/* warn(" do_not_translate = %d\n", do_not_translate); */
}


void handle_else(void)
{
/* warn("** ELSE in file '%s' line %d\n", zzfilename, zzline); */

if(!ifdef_stack_top)
	{
	mesg("** Error: `else without `ifdef in file '%s' line %d\n", zzfilename, zzline);
	}
	else
	{
	if(ifdef_stack_top->else_branch)
		{
		do_not_translate = ifdef_stack_top->do_not_translate; /* take from parent */
		}
		else
		{
		do_not_translate |= STMODE_XLATEOFF_IFDEF;
		ifdef_stack_top->blocked = 1;
		}
	}

/* warn(" do_not_translate = %d\n", do_not_translate); */
}

void handle_elsif(char *def_text)
{
/* warn("** ELSIF in file '%s' line %d\n", zzfilename, zzline); */

if(!ifdef_stack_top)
	{
	mesg("** Error: `elsif without `ifdef in file '%s' line %d\n", zzfilename, zzline);
	}
	else
	{
	handle_else();

	if(ifdef_stack_top->else_branch)
		{
		char *pnt = def_text+6;
		char *s1;
		/* JRB node; */
		struct ifdef_stack_t *is;

		s1=strtok(pnt, " \t\b");	
		if(!s1)
			{
			warn("** Warning: malformed `elsif directive in file '%s' line %d, ignoring.\n", zzfilename, zzline);
			return;
			}

		is = ifdef_stack_top; 
		is->polarity = 1;
		free(is->zzfilename);
		is->zzfilename = strdup(zzfilename);
		is->zzline = zzline;
		free(is->deftext);
		is->deftext = strdup(s1);

		/* is->blocked = ifdef_stack_top ? ifdef_stack_top->blocked : 0; */
		/* is->next = ifdef_stack_top; */
		/* is->do_not_translate = do_not_translate; */
		/* ifdef_stack_top = is; */
	
			{
			if(!(/* node= */jrb_find_str(define_tree, s1)))
				{
				/* skip action */
				do_not_translate |= STMODE_XLATEOFF_IFDEF;
				is->blocked = 1;
				is->else_branch = 1;	/* translate in `else */
				}
				else
				{
				/* do */
				is->else_branch = 0; /* do not translate in `else */
				}
			}
		}
		else
		{
		do_not_translate |= STMODE_XLATEOFF_IFDEF;
		ifdef_stack_top->blocked = 1;
		}
	}

/* warn(" do_not_translate = %d\n", do_not_translate); */
}


void handle_endif(void)
{
struct ifdef_stack_t *is = ifdef_stack_top;
/* warn("** ENDIF in file '%s' line %d\n", zzfilename, zzline); */

if(!is)
        {
        mesg("** Error: `endif without `ifdef in file '%s' line %d\n", zzfilename, zzline);
        }   
	else
	{
	do_not_translate = is->do_not_translate;

	free(is->zzfilename);
	free(is->deftext);
	ifdef_stack_top = is->next;
	free(is);
	}

/* warn(" do_not_translate = %d\n", do_not_translate); */
}


/*  
 * include file handling
 */
static FILE *incdir_fopen(char *nam, char **backpath)
{
FILE *fp=NULL;
struct vpp_filename *l;
int len;
static int traversed_incchain_for_caseinsens = 0;

if((nam[0]!='/')&&(incchain))
        {
        len = strlen(nam);
        l=incchain;
        while(l)
                {
                char *path = (char *)malloc(strlen(l->name) + 1 + len + 1);
                sprintf(path, "%s/%s", l->name, nam);
                fp=fopen(path, "r");
                if(fp) 
			{
			warn("Processing include file '%s' ...\n", path);
	                *backpath=path;
			break;
			}

                free(path);
                l=l->next;
                }
        }

if(!fp) 
	{
	fp=fopen(nam, "r");
	if(fp)
		{
		warn("Processing include file '%s' ...\n", nam);
		*backpath=strdup(nam);
		}
	else
	if((nam[0]!='/')&&(incchain))
		{
		if(!traversed_incchain_for_caseinsens)
			{
			traversed_incchain_for_caseinsens = 1;
			l=incchain;
			while(l)
				{
				dirscan(l->name, l);
				l=l->next;
				}
			}

		l=incchain;
		while(l)
			{
			char **realname = bsearch(nam, l->children, l->numchildren, sizeof(char **), compar_cstarstar_bsearch);
			if(realname)
				{
				char *buff = (char *)malloc(strlen(l->name)+1+strlen(*realname)+1);
                                sprintf(buff, "%s/%s", l->name, *realname);
				fp=fopen(buff, "r");				
				if(fp)
					{
					warn("Processing include file '%s' ...\n", buff);
					*backpath=buff;
					}
					else
					{
					free(buff);
					}
				break;
				}
			l=l->next;
			}

		}

	}

return(fp);
}


void handle_include(char *def_text)
{
char *pnt = def_text+9, *pnt2;

while(*(pnt++)!='\"');
pnt2=pnt;
while(*(pnt2++)!='\"');
*(pnt2-1)=0;

if(!*pnt)
	{
        warn("** Warning: `include without filename in file '%s' line %d\n", zzfilename, zzline);
	return;
	}
	else
	{
	FILE *fh2;
	char *backpath;

	fh2=incdir_fopen(pnt, &backpath);
	if(!fh2)
		{
		mesg("** Error: could not open include file '%s'\n", pnt);
		}
		else
		{
		struct include_stack_t *ics = calloc(1, sizeof(struct include_stack_t));

		ics->zzfilename=strdup(zzfilename);
		ics->zzline = zzline;
		ics->fh = fh;
		ics->preproc_buff_head = preproc_buff_head;
		ics->preproc_buff_current = preproc_buff_current;
		ics->next = include_stack_top;
		include_stack_top = ics;
	
		preproc_buff_head = NULL;
		preproc_buff_current = NULL;
		fh = fh2;

		if((++include_stack_depth) == 64)
			{
			int depth = 1;
			mesg("** Error: maximum include stack depth of 64 reached, reverse trace follows\n");
			do	{
				warn("%7d : file '%s' line %d\n", depth++, ics->zzfilename, ics->zzline);
				ics=ics->next;
				} while ((depth==2)||(strcmp(ics->zzfilename, include_stack_top->zzfilename))||(ics->zzline!=include_stack_top->zzline));

			zzerrors+=zzLexErrCount;
			warn("\n** %d error%s detected, exiting.\n\n",zzerrors,(zzerrors>1)?"s were":" was");
			exit(255);
			}

		mprintf("// vpp_file %s \n", backpath); free(backpath);
		mprintf("// vpp_line %d\n", 1);
		}
	}
}


int pop_include(void)
{
struct include_stack_t *ics = include_stack_top;	

if(ics)
	{
	include_stack_depth--;
	warn("... returning to file '%s' line %d\n", ics->zzfilename, ics->zzline);
	fh=ics->fh;
	preproc_buff_head = ics->preproc_buff_head;
	preproc_buff_current = ics->preproc_buff_current;

	mprintf("// vpp_file %s \n", ics->zzfilename);
	mprintf("// vpp_line %d\n", ics->zzline);

	free(ics->zzfilename);
	include_stack_top = ics->next;
	free(ics);
	return(1);
	}
	else
	{
	return(0);
	}
}

