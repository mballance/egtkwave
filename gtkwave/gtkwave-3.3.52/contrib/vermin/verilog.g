#header << 

#include "attrib.h"
#include "vpp_common.h"

#if defined __MINGW32__ || defined _MSC_VER
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)
#endif

int zzcr_attr(Attrib *attr, int token, char *text);
void vpp_update_yyfilename(const char *str);
void vpp_update_yylineno(const char *str);

>>

#tokdefs "keyword_tokens.h"

<<
#include "../../config.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>

#include "wave_locale.h"

#include "inter.h"
#include "tokens.h"
#include "shred.h"
#include "symbol.h"
#include "jrb.h"
#include "vpp_common.h"

#ifndef PATH_MAX
#define PATH_MAX (4096)
#endif

int verilog_keyword_code(const char *s, unsigned int len); /* returns token value from gperf */
void args_expand(int argc, char **argv, int *new_argc, char ***new_argv);

int zzerrors=0;
int zzcomment_start=0, zzcomment_depth=0, zzcomment_entry=0;
char *zzfilename=NULL;	

JRB modname_tree=NULL;
int module_is_duplicate=0;

char *mod_current_name = NULL;
char *mod_current_filename = NULL;
int   mod_start_line = 0;

char *udp_current_name = NULL;
char *udp_current_filename = NULL;
int   udp_start_line = 0;

char *comp_type_name = NULL;

int emit_stems = 0, emit_vars = 0;

static void add_string_to_tree(JRB tree, char *s, int exists)
{
Jval val;
JRB node;

if(!(node=jrb_find_str(tree, s)))
	{
	val.i = (exists!=0);
	jrb_insert_str(tree, s, val);
	}
	else
	{
	node->val.i |= (exists!=0);
	}
}

char *prevlineroot=NULL;
char *prevprevlineroot=NULL;
char *lineroot=NULL, *linepos=NULL;
int linemaxlen=0;
int zzrewind=0;
                 
/*
 * add token to the analyzed verilog buffer..
 */
void addtoken(int is_newline)
{
if(!is_newline) 
	{
	zzrewind = strlen(zztext);

	if((linepos-lineroot)+zzrewind >= linemaxlen)
		{
		char *t=realloc(lineroot, linemaxlen = 2*(linemaxlen+zzrewind)+1);
		prevlineroot=realloc(prevlineroot, linemaxlen);
		prevprevlineroot=realloc(prevprevlineroot, linemaxlen);
		linepos = (linepos-lineroot) + t;
		lineroot = t;
		}
	
	strcpy(linepos,zztext);
	linepos+=zzrewind;
	}
	else
	{ 
	char *t=prevprevlineroot;		/* scroll the buffer ptrs to avoid copying */
	prevprevlineroot = prevlineroot;
	prevlineroot = lineroot;
	linepos = lineroot = t;
	*linepos=0; 
	zzline++; 
	}
}

void report_error_context(int tok, char *badtok)
{ 
char *ch, v;
int col=0;

ch=lineroot;

do
{
v=*(ch++);
switch(v)
	{
	case '\t': col=(col+8)&(~0x07); break;
	case '\n': col=2;		break;
	default:   col++;		break;
	}
} while(v);

col-=zzrewind; if(col<2) col=2;

if(tok!=zzEOF_TOKEN)
	{
	warn("   Local context near token '%s' in '%s' on line %d.%d:\n",badtok,zzfilename, zzline,col-1);
	}
	else
	{
	warn("   Local context preceding EOF in '%s' on line %d.%d:\n",zzfilename, zzline,col-1);
	}
if(zzline>2)
	{
	warn("%6d: %s\n", zzline-2, prevprevlineroot);
	}
if(zzline>1)
	{
	warn("%6d: %s\n", zzline-1, prevlineroot);
	}

if(col>2)
	{
	warn("%6d: %s\n\t%*s^\n", zzline, lineroot, col-2, "");
	}
	else
	{
	warn("%6d: %s\n\t^\n", zzline, lineroot);
	}
}


int do_not_translate = 0, do_not_translate_mask = 0;

void warn_about_translation_off(void)
{
if(do_not_translate)
	{
	warn("** Warning: source code translation off for { %s%s%s%s%s} at EOF in '%s'.\n", 
			(do_not_translate&STMODE_XLATEOFF_IFDEF)     ? "ifdef "     : "",
			(do_not_translate&STMODE_XLATEOFF_SYNOPSYS)  ? "synopsys "  : "",
			(do_not_translate&STMODE_XLATEOFF_SYNTHESIS) ? "synthesis " : "",
			(do_not_translate&STMODE_XLATEOFF_VERILINT)  ? "verilint "  : "",
			(do_not_translate&STMODE_XLATEOFF_VERTEX)    ? "vertex "    : "",
			zzfilename);

	do_not_translate = 0;
	}

if(ifdef_stack_top)
		{
		struct ifdef_stack_t *is;

		while(ifdef_stack_top)
			{
			is=ifdef_stack_top;

			warn("** Warning: pending `if%sdef %s at EOF, start is file '%s' line %d.\n",
				is->polarity ? "" : "n", 
				is->deftext,
				is->zzfilename, is->zzline);

		        free(is->zzfilename);
		        free(is->deftext);
		        ifdef_stack_top = is->next;
		        free(is);
			}

		include_stack_depth = 0;
		}

if(zzcomment_depth)
	{
	mesg("** Error: Unterminated comment starts at line %d in '%s'.\n", zzcomment_start, zzfilename);
	zzcomment_depth = 0;
	}
}


void parsecomment(void)
{
char *tok=strdup(zztext);
char *tok_sav=tok;
strcpy(tok, zztext);

tok = strtok(tok,"/ \t");
if (tok != 0) {
if (!strcmp("vpp_file", tok))
	{
        tok = strtok(NULL, " \t");
        if(tok) vpp_update_yyfilename(tok);
        }
else
if (!strcmp("vpp_line", tok))
	{
        tok = strtok(NULL, "");
        if(tok) vpp_update_yylineno(tok);
        }
else
if(!(do_not_translate&STMODE_XLATEOFF_IFDEF))	/* make sure preprocessed block is active */
	{
	if ((!strcmp("synopsys", tok))&&(do_not_translate_mask & STMODE_XLATEOFF_SYNOPSYS))
		{
	        tok = strtok(NULL, " \t");
	        if(tok) 
			{
			if(!strcmp("translate_on", tok))
				{
				do_not_translate &= ~(STMODE_XLATEOFF_SYNOPSYS);
				}
			else
			if(!strcmp("translate_off", tok))
				{
				do_not_translate |= (do_not_translate_mask & STMODE_XLATEOFF_SYNOPSYS);
				}
			else
				{
				warn("** Warning: unsupported synopsys pragma '%s' on line %d in file '%s', skipping.\n",
					tok, zzline, zzfilename);
				}
			}
	        }
	else
	if ((!strcmp("synthesis", tok))&&(do_not_translate_mask & STMODE_XLATEOFF_SYNTHESIS))
		{
	        tok = strtok(NULL, " \t");
	        if(tok) 
			{
			if(!strcmp("translate_on", tok))
				{
				do_not_translate &= ~(STMODE_XLATEOFF_SYNTHESIS);
				}
			else
			if(!strcmp("translate_off", tok))
				{
				do_not_translate |= (do_not_translate_mask & STMODE_XLATEOFF_SYNTHESIS);
				}
			else
				{
				warn("** Warning: unsupported synthesis pragma '%s' on line %d in file '%s', skipping.\n",
					tok, zzline, zzfilename);
				}
			}
	        }
	else
	if ((!strcmp("verilint", tok))&&(do_not_translate_mask & STMODE_XLATEOFF_VERILINT))
		{
	        tok = strtok(NULL, " \t");
	        if(tok) 
			{
			if(!strcmp("translate", tok))
				{
			        tok = strtok(NULL, " \t");
	        		if(tok)
					{
					if(!strcmp("on", tok))
						{
						do_not_translate &= ~(STMODE_XLATEOFF_VERILINT);
						}
					else
					if(!strcmp("off", tok))
						{
						do_not_translate |= (do_not_translate_mask & STMODE_XLATEOFF_VERILINT);
						}
					else
						{
						warn("** Warning: unsupported translate option '%s' on line %d in file '%s', skipping.\n",
							tok, zzline, zzfilename);
						}
					}
				}
			}
	        }
	else
	if ((!strcmp("vertex", tok))&&(do_not_translate_mask & STMODE_XLATEOFF_VERTEX))
		{
	        tok = strtok(NULL, " \t");
	        if(tok) 
			{
			if(!strcmp("translate", tok))
				{
			        tok = strtok(NULL, " \t");
	        		if(tok)
					{
					if(!strcmp("on", tok))
						{
						do_not_translate &= ~(STMODE_XLATEOFF_VERTEX);
						}
					else
					if(!strcmp("off", tok))
						{
						do_not_translate |= (do_not_translate_mask & STMODE_XLATEOFF_VERTEX);
						}
					else
						{
						warn("** Warning: unsupported translate option '%s' on line %d in file '%s', skipping.\n",
							tok, zzline, zzfilename);
						}
					}
				}
			}
	        }
	}
}

free(tok_sav);
}


void
zzsyn(char *text, int tok, char *egroup, SetWordType *eset, int etok, int k, char *bad_text)
{
	if(tok!=zzEOF_TOKEN)
		{
        	mesg("** Syntax error at \"%s\"", bad_text);
		}
		else
		{
        	mesg("** Syntax error at EOF");
		}
        if ( !etok && !eset ) {warn("\n"); return;}
        if ( k==1 ) warn(" missing");
        else
        {
                warn("; \"%s\" not", bad_text);
                if ( zzset_deg(eset)>1 ) warn(" in");
        }
        if ( zzset_deg(eset)>0 ) zzedecode(eset);
        else warn(" %s", zztokens[etok]);
        if ( strlen(egroup) > 0 ) warn(" in %s", egroup);
        warn("\n");
	report_error_context(tok, bad_text);
}


int zzcr_attr(Attrib *attr, int token, char *text)
{
int len;

switch(token)
	{
	case V_FUNCTION_NAME:
	case V_IDENTIFIER:
	case V_IDENTIFIER2:
	case V_IDENDOT:
		attr->symbol=sym_add(text);
		break;

	case V_DBASE:
	case V_BBASE:
	case V_OBASE:
	case V_HBASE:
		attr->text=strdup(text);
		break;

	case V_STRING:
		len = strlen(text);
		text[len-1]=0;
		strcpy(attr->text = malloc(len-2+1), text+1);
		break;

	case V_DECIMAL_NUMBER:
		attr->ival=atoi_with_underscores(text);
		break;
	case V_FLOAT1:
	case V_FLOAT2:
		sscanf(text, "%lf", &attr->rval);	
		break;

	default: 
		attr->ival=0;
	}
return(0);
}


int main(int argc, char **argv)
{
int v_argc;
char **v_argv;
int i;
struct vpp_filename *head=NULL, *curr=NULL, *t, *e;
struct vpp_filename *lib_head=NULL, *lib_curr=NULL;
struct vpp_filename *libext_head=NULL, *libext_curr=NULL;
JRB node;
int maxarg; /* scan build : was  int maxarg = v_argc - 1; */

WAVE_LOCALE_FIX

args_expand(argc, argv, &v_argc, &v_argv);
maxarg = v_argc - 1;

modname_tree=make_jrb();
define_tree=make_jrb();

lineroot=linepos=(char *)calloc(1, linemaxlen=16);
prevlineroot=(char *)calloc(1, linemaxlen);
prevprevlineroot=(char *)calloc(1, linemaxlen);

if(v_argc==1)
	{
	warn(VERNAME"\n"
		"No files to process (use -h for help), exiting.\n\n");
	exit(0);
	}

warn(VERNAME"\n\n");

for(i=1;i<v_argc;i++)
	{
	int arglen = strlen(v_argv[i]);

	if((!strcmp(v_argv[i], "-y"))||(!strcmp(v_argv[i], "-yi")))
		{
		if(i==maxarg)
			{
			warn("** Missing filename after %s option!\n", v_argv[i]);
			}
			else
			{
			t=calloc(1, sizeof(struct vpp_filename));
			t->name = strdup(v_argv[++i]);

			if(v_argv[i-1][2])		/* isn't nullchar so it's 'i' */
				{
				dirscan(t->name, t);	/* it's a case-insensitive directory, so scan it in */
				}
	
			if(!lib_head)
				{
				lib_head=lib_curr=t;
				}
				else
				{
				lib_curr->next = t;
				lib_curr = t;
				}
			}
		}
	else
	if(!strcmp(v_argv[i], "-emitmono"))
		{
		if(i==maxarg)
			{
			warn("** Missing filename after -emitmono option!\n");
			}
			else
			{
			i++;

			if(mgetchar_fout)
				{
				warn("** Ignoring extra -emitmono specification for '%s'.\n", v_argv[i]);
				}
				else
				{
				mgetchar_fout = fopen(v_argv[i], "wb");
				if(!mgetchar_fout)
					{
					mesg("** Could not open -emitmono file '%s', exiting.\n", v_argv[i]);
					perror("Why");
					exit(255);
					}
				}
			}
		}
	else
	if(!strcmp(v_argv[i], "-pragma"))
		{
		if(i==maxarg)
			{
			warn("** Missing typename after -pragma option!\n");
			}
			else
			{
			i++;
			if(!strcmp(v_argv[i], "synopsys"))
				{
				do_not_translate_mask |= STMODE_XLATEOFF_SYNOPSYS; 
				}
			else
			if(!strcmp(v_argv[i], "synthesis"))
				{
				do_not_translate_mask |= STMODE_XLATEOFF_SYNTHESIS; 
				}
			else
			if(!strcmp(v_argv[i], "verilint"))
				{
				do_not_translate_mask |= STMODE_XLATEOFF_VERILINT; 
				}
			else
			if(!strcmp(v_argv[i], "vertex"))
				{
				do_not_translate_mask |= STMODE_XLATEOFF_VERTEX; 
				}
			else
				{
				warn("** Unknown -pragma type '%s', ignoring.\n", v_argv[i]);
				}
			}
		}
	else
	if((!strcmp(v_argv[i], "-h"))||(!strcmp(v_argv[i], "-help")))
		{
		warn(	"Usage:\n"
			"------\n"
			"%s [input filename(s)] [options]\n\n"
			"-h[elp]         prints this screen\n"
			"-emitmono fname emit monolithic (parser view of) file to fname\n"
			"-emitstems      emit source code stems to stdout\n"
			"-emitvars       emit source code variables to stdout\n"
			"-Dx=y           equivalent to `define X Y in source\n"
			"+define+x=y     equivalent to `define X Y in source\n"
			"+incdir+dirname add dirname to include search path\n"
			"+libext+ext     add ext to filename when searching for files\n"
			"-pragma name    add name (synopsys, synthesis, verilint, vertex) to accepted pragmas\n"
			"-y dirname      add directory to source input path\n"
			"-yi dirname     add directory to source input path (case insensitive search)\n"
			"-f filename     insert args from filename (does not work recursively)\n"
			"\n", v_argv[0]
		);
		exit(0);
		}
	else
	if(!strcmp(v_argv[i], "-f"))
		{
		warn("** Cannot nest -f option inside an args file, exiting.\n");
		exit(255);
		}
	else
	if(!strcmp(v_argv[i], "-emitstems"))
		{
		emit_stems = 1;
		}
	else
	if(!strcmp(v_argv[i], "-emitvars"))
		{
		emit_vars = 1;
		}
	else
        if((arglen>=8)&&(!strncmp(v_argv[i],"+incdir+",8)))
                {
                if(arglen==8)
                        {
                        warn("** Missing path for +incdir+ in command line argument %d, ignoring.\n", i);
                        }   
                        else
                        {
                        char *lname=(char *)malloc(arglen-8+1);
			char *tok;

                        strcpy(lname, v_argv[i]+8);
			tok=strtok(lname,"+");                 

			while(tok)
				{
				int toklen=strlen(tok);
				if(!toklen)
					{
					/* strtok seems to never hit this */
		                        warn("** Missing path for +incdir+ (consecutive ++) in command line argument %d, ignoring.\n", i);
					}
					else
					{
		                        if(!incchain)
		                                {
		                                struct vpp_filename *l;
		                                l=(struct vpp_filename *)calloc(1,sizeof(struct vpp_filename));
		                                strcpy(l->name=malloc(toklen+1), tok);

		                                incchain=l;
		                                }
		                                else
		                                {
		                                struct vpp_filename *l=incchain;
		                                struct vpp_filename *l2;
		
		                                while(l->next) l=l->next;
		
		                                l2=(struct vpp_filename *)calloc(1,sizeof(struct vpp_filename));
		                                strcpy(l2->name=malloc(toklen+1), tok);
		                                l->next=l2;
		                                }
					}

				tok=strtok(NULL, "+");
				}

			free(lname);                        
			}
                }
	else
        if((arglen>=8)&&(!strncmp(v_argv[i],"+define+",8)))
                {
                if(arglen==8)
                        {
                        warn("** Missing value for +define+ in command line argument %d, ignoring.\n", i);
                        }   
                        else
                        {
                        char *lname=(char *)malloc(arglen-8+1);
			char *tok;

                        strcpy(lname, v_argv[i]+8);
			tok=strtok(lname,"+");                 

			while(tok)
				{
				int toklen=strlen(tok);
				if(!toklen)
					{
					/* strtok seems to never hit this */
		                        warn("** Missing value for +define+ (consecutive ++) in command line argument %d, ignoring.\n", i);
					}
					else
					{
		                        char *dname=(char *)malloc(toklen+8+1);
                		        char *pnt;
                		        sprintf(dname, "`define %s", tok);
                		        pnt = dname+8;
                		        while(*pnt)
                		                {
                		                if(*pnt=='=')
                		                        {
                		                        *pnt=' ';
                		                        break;
                		                        }
                		                pnt++;
                		                }
                		        store_define(dname);
                		        free(dname);
					}

				tok=strtok(NULL, "+");
				}

			free(lname);                        
			}
                }
	else
        if((arglen>=8)&&(!strncmp(v_argv[i],"+libext+",8)))
                {
                if(arglen==8)
                        {
                        warn("** Missing extension for +libext+ in command line argument %d, ignoring.\n", i);
                        }   
                        else
                        {
                        char *lname=(char *)malloc(arglen-8+1);
			char *tok;
                        strcpy(lname, v_argv[i]+8);

			tok=strtok(lname,"+");                 
			while(tok)
				{
				int toklen=strlen(tok);
				if(!toklen)
					{
					/* strtok seems to never hit this */
		                        warn("** Missing path for +libext+ (consecutive ++) in command line argument %d, ignoring.\n", i);
					}
					else
					{
		                        if(!libext_curr)
		                                {
		                                struct vpp_filename *l;
		                                l=(struct vpp_filename *)calloc(1,sizeof(struct vpp_filename));
						strcpy(l->name=malloc(toklen+1), tok);
		                                libext_head=libext_curr=l;
		                                }
		                                else
		                                {
		                                struct vpp_filename *l;
		
		                                l=(struct vpp_filename *)calloc(1,sizeof(struct vpp_filename));
						strcpy(l->name=malloc(toklen+1), tok);
		                                libext_curr->next=l;
						libext_curr=l;
		                                }
					}

				tok=strtok(NULL, "+");
				}
			free(lname);                        
                        }
                }
        else
	if((arglen>=2)&&(v_argv[i][0] == '+'))
		{
		warn("** Skipping plusarg '%s' in command line argument %d.\n", v_argv[i], i);
		}
	else
        if((arglen>=2)&&(!strncmp(v_argv[i],"-D",2)))
                {
                if(arglen==2)
                        {
                        warn("** Missing define for -D in command line argument %d, ignoring.\n", i);
                        }
                        else
                        {
                        char *dname=(char *)malloc(arglen-2+8+1);
                        char *pnt;
                        sprintf(dname, "`define %s", v_argv[i]+2);
                        pnt = dname+8;
                        while(*pnt)
                                {
                                if(*pnt=='=')
                                        {
                                        *pnt=' ';
                                        break;
                                        }
                                pnt++;
                                }
                        store_define(dname);
                        free(dname);
                        }
                }
	else	/* filename only.. */
		{
		t=calloc(1, sizeof(struct vpp_filename));
		t->name = strdup(v_argv[i]);
	
		if(!head)
			{
			head=curr=t;
			vlog_filenames = head;
			}
			else
			{
			curr->next = t;
			curr = t;
			}
		}
	}

shred_alloc();

prevprevlineroot[0]=prevlineroot[0]=lineroot[0]=0; linepos=lineroot; do_not_translate = 0;
ANTLRf(v_source_text(), mgetchar_fout ? mgetchar_outhandle : mgetchar); 
warn_about_translation_off();
zzerrors+=zzLexErrCount;
if(zzerrors)
	{
	warn("\n** %d error%s detected, exiting.\n\n",zzerrors,(zzerrors>1)?"s were":" was");
	exit(255);
	}

do	{
	i=0;
	jrb_traverse(node, modname_tree)
		{
		if(node->val.i==0)
			{
			FILE *hand;
			int len = strlen(node->key.s);
			int resolve = 0;
			char *buff;
			t=lib_head;
	
			while(t)
				{
				e=libext_head;
				do
					{
					char *ext=e?e->name:"";
					buff = (char *)malloc(strlen(t->name)+1+len+strlen(ext)+1);
					sprintf(buff, "%s/%s%s", t->name, node->key.s, ext);
					hand = fopen(buff, "r");
					if(hand)
						{
						warn("Processing file '%s' ...\n", buff);
						vpp_main(hand, buff);
						prevprevlineroot[0]=prevlineroot[0]=lineroot[0]=0; linepos=lineroot; do_not_translate = 0;
						ANTLRf(v_source_text(), mgetchar_fout ? mgetchar_outhandle : mgetchar);	/* preproc.c will do fclose() */
						warn_about_translation_off();
						free(buff);
						zzerrors+=zzLexErrCount;
						if(zzerrors)
							{
							warn("\n** %d error%s detected, exiting.\n\n",zzerrors,(zzerrors>1)?"s were":" was");
							exit(255);
							}
						i=1;
						resolve=1;
						goto is_resolved;
						}
					free(buff);
					} while((e)&&(e=e->next));

				t=t->next;
				}

			t=lib_head;
	
			while(t)
				{
				if(t->numchildren)
					{
					e=libext_head;
					do
						{
						char **realname;
						char *ext=e?e->name:"";
						buff = (char *)malloc(len+strlen(ext)+1);
						sprintf(buff, "%s%s", node->key.s, ext);
						realname = bsearch(buff, t->children, t->numchildren, sizeof(char **), compar_cstarstar_bsearch);
						free(buff);
						if(realname)
							{
		                                        buff = (char *)malloc(strlen(t->name)+1+strlen(*realname)+1);
        		                                sprintf(buff, "%s/%s", t->name, *realname);

							hand = fopen(buff, "r");
							if(hand)
								{
								warn("Processing file '%s' ...\n", buff);
								vpp_main(hand, buff);
								prevprevlineroot[0]=prevlineroot[0]=lineroot[0]=0; linepos=lineroot; do_not_translate = 0;
								ANTLRf(v_source_text(), mgetchar_fout ? mgetchar_outhandle : mgetchar);	/* preproc.c will do fclose() */
								warn_about_translation_off();
								free(buff);
								zzerrors+=zzLexErrCount;
								if(zzerrors)
									{
									warn("\n** %d error%s detected, exiting.\n\n",zzerrors,(zzerrors>1)?"s were":" was");
									exit(255);
									}
								i=1;
								resolve=1;
								goto is_resolved;
								}	
							free(buff);
							}
						} while((e)&&(e=e->next));
					}

				t=t->next;
				}
	
is_resolved:		if(!resolve)
				{
				warn("** Error: could not find module '%s'\n", node->key.s);
				}

			node->val.i=1;
			}
		}
	} while(i==1);

zzerrors+=zzLexErrCount;
if(zzerrors)
	{
	warn("\n** %d error%s detected, exiting.\n\n",zzerrors,(zzerrors>1)?"s were":" was");
	exit(255);
	}

shred_free();
return(0);
}



>>

//
// error classes 
//
#errclass "expression"
{ V_DECIMAL_NUMBER V_FLOAT1 V_FLOAT2 V_DBASE V_BBASE V_OBASE V_HBASE V_STRING V_PLUS V_MINUS V_BANG
V_TILDE V_AND V_NAND V_OR V_NOR V_XOR V_XNOR V_XNOR2 V_LP V_LBRACE V_FUNCTION_NAME V_IDENTIFIER
V_IDENTIFIER2 V_IDENDOT }


//
// regular tokens
// 
#token 				"@"				<< /* will set NLA=V_EOF; in generation*/ >>

#token "[\ \t\b\r]+"			
		<< addtoken(0); zzskip(); >>
#token "\n"	
		<< addtoken(1); zzskip(); >>


#token V_DECIMAL_NUMBER 	"[0-9_]+"				<< addtoken(0); STMODE; >>
#token V_FLOAT1 		"[0-9_]+\.[0-9_]+"			<< addtoken(0); STMODE; >>
#token V_FLOAT2			"[0-9_]+(\.[0-9_]+)?[Ee][0-9_]+" 	<< addtoken(0); STMODE; >>
#token V_DBASE			"[0-9_]*\'[dD][0-9_]+"			<< addtoken(0); STMODE; >>
#token V_BBASE			"[0-9_]*\'[bB][xXzZ01_\?]+"		<< addtoken(0); STMODE; >>
#token V_OBASE			"[0-9_]*'[oO][xXzZ0-7_\?]+"		<< addtoken(0); STMODE; >>
#token V_HBASE			"[0-9_]*\'[hH][xXzZ0-9A-Fa-f_\?]+" 	<< addtoken(0); STMODE; >>
#token V_STRING			"\"~[\"\r\n]*\""			<< addtoken(0); STMODE; >>

#token V_EQ			"\="				<< addtoken(0); STMODE; >>
#token V_DOT			"\."				<< addtoken(0); STMODE;>>

#token V_PLUS			"\+"				<< addtoken(0); STMODE; >>
#token V_MINUS			"\-"				<< addtoken(0); STMODE; >>
#token V_BANG			"\!"				<< addtoken(0); STMODE; >>
#token V_TILDE			"\~"				<< addtoken(0); STMODE; >>
#token V_AND			"\&"				<< addtoken(0); STMODE; >>
#token V_NAND			"\~\&"				<< addtoken(0); STMODE; >>
#token V_OR			"\|"				<< addtoken(0); STMODE; >>
#token V_NOR			"\~\|"				<< addtoken(0); STMODE; >>
#token V_XOR			"\^"				<< addtoken(0); STMODE; >>
#token V_XNOR			"\~\^"				<< addtoken(0); STMODE; >>

#token V_STAR			"\*"				<< addtoken(0); STMODE; >>
#token V_SLASH			"\/"				<< addtoken(0); STMODE; >>
#token V_MOD			"\%"				<< addtoken(0); STMODE; >>
#token V_EQ2			"\=\="				<< addtoken(0); STMODE; >>
#token V_NEQ			"\!\="				<< addtoken(0); STMODE; >>
#token V_EQ3			"\=\=\="			<< addtoken(0); STMODE; >>
#token V_NEQ2			"\!\=\="			<< addtoken(0); STMODE; >>
#token V_AND2			"\&\&"				<< addtoken(0); STMODE; >>
#token V_AND3			"\&\&\&"			<< addtoken(0); STMODE; >>
#token V_OR2			"\|\|"				<< addtoken(0); STMODE; >>
#token V_LT			"\<"				<< addtoken(0); STMODE; >>
#token V_LEQ			"\<\="				<< addtoken(0); STMODE; >>
#token V_GT			"\>"				<< addtoken(0); STMODE; >>
#token V_GEQ			"\>\="				<< addtoken(0); STMODE; >>
#token V_XNOR2			"\^\~"				<< addtoken(0); STMODE; >>
#token V_SHL			"\<\<"				<< addtoken(0); STMODE; >>
#token V_SHR			"\>\>"				<< addtoken(0); STMODE; >>
#token V_SSHL			"\<\<\<"			<< addtoken(0); STMODE; >>
#token V_SSHR			"\>\>\>"			<< addtoken(0); STMODE; >>
#token V_POW			"\*\*"				<< addtoken(0); STMODE; >>

#token V_QUEST			"\?"				<< addtoken(0); STMODE; >>
#token V_COLON			"\:"				<< addtoken(0); STMODE; >>

#token V_LP			"\("				<< addtoken(0); STMODE; >>
#token V_RP			"\)"				<< addtoken(0); STMODE; >>

#token V_LBRACK			"\["				<< addtoken(0); STMODE; >>
#token V_RBRACK			"\]"				<< addtoken(0); STMODE; >>

#token V_LBRACE			"\{"				<< addtoken(0); STMODE; >>
#token V_RBRACE			"\}"				<< addtoken(0); STMODE; >>
#token V_COMMA			"\,"				<< addtoken(0); STMODE; >>

#token V_AT			"\@"				<< addtoken(0); STMODE; >>

#token V_SEMI			"\;"				<< addtoken(0); STMODE; >>
#token V_POUND			"\#"				<< addtoken(0); STMODE; >>
#token V_RARROW			"\-\>"				<< addtoken(0); STMODE; >>

#token V_TABLE			"table"				<< addtoken(0); if(!do_not_translate) { zzmode(UDP); } else { zzskip(); } >>

#token V_SPD1			"\=\>"				<< addtoken(0); STMODE; >>
#token V_SPD2			"\*\>"				<< addtoken(0); STMODE; >>


// identifiers/functions/most keywords
#token V_FUNCTION_NAME		"\$[_a-zA-Z][_a-zA-Z\$0-9]*"	<< addtoken(0); STMODE; >>
#token V_IDENTIFIER 		"[_a-zA-Z][_a-zA-Z\$0-9]*" 	<< addtoken(0); NLA = verilog_keyword_code(zztext, zzrewind); STMODE; >>
#token V_IDENTIFIER2		"\\~[\ \t\b\n\r]+"		<< addtoken(0); STMODE; >>
#token V_IDENDOT		"[_a-zA-Z][_a-zA-Z\$0-9]*(\.[_a-zA-Z][_a-zA-Z\$0-9]*)+"  << addtoken(0); STMODE; >>

// comment handling
#token 				"//~[\n]*"			<< addtoken(0); parsecomment(); zzskip(); >>
#token				"/\*"				<< addtoken(0); zzcomment_depth=1; zzcomment_entry=1; zzcomment_start=zzline; zzmode(COMMENT); zzskip(); >>
#token				"\*/"				<< addtoken(0); 
									mesg("** End of comment when not in a comment on line %d in file '%s'.\n", zzline, zzfilename);
									zzskip(); >>
#lexclass COMMENT		
#token				"/\*"				<< addtoken(0); /* zzcomment_depth++; <=- VERILOG DOES NOT ALLOW NESTED COMMENTS */
									zzskip(); >>
#token				"\*/"				<< addtoken(0); zzcomment_depth--; zzskip(); if(!zzcomment_depth) { zzmode(START); } >>
#token				"\n"				<< addtoken(1); zzskip(); >>
#token				"~[/\*\n]+"			<< addtoken(0); if(zzcomment_entry) { parsecomment(); zzcomment_entry=0; } zzskip(); >>
#token				"[/\*]"				<< addtoken(0); zzskip(); >>

// udp handling
#lexclass UDP
#token V_OUTPUT_SYMBOL		"[01xX\?bB]"			<< addtoken(0); STMODE; >>
#token V_LEVEL_SYMBOL_EXTRA	"[\?bB]"			<< addtoken(0); STMODE; >>
#token V_EDGE_SYMBOL		"[rRfFpPnN\*]"			<< addtoken(0); STMODE; >>
#token V_HYPHEN			"\-"				<< addtoken(0); STMODE; >>
#token				"\n"				<< addtoken(1); zzskip(); >>
#token 				"[\ \t\b\r]+"			<< addtoken(0); zzskip(); >>
#token V_COLON			"\:"				<< addtoken(0); STMODE; >>
#token V_SEMI			"\;"				<< addtoken(0); STMODE; >>
#token V_ENDTABLE		"endtable"			<< addtoken(0); if(!do_not_translate) { zzmode(START); } else { zzskip(); } >>
#token V_FUNCTION_NAME		"\$[_a-zA-Z][_a-zA-Z\$0-9]*"	<< addtoken(0); STMODE; >>	/* minimize error messages */
#token V_IDENTIFIER 		"[_a-zA-Z][_a-zA-Z\$0-9]*" 	<< addtoken(0); STMODE; >> 	/* when in udp */
#token V_IDENTIFIER2		"\\~[\ \t\b\n\r]+"		<< addtoken(0); STMODE; >>
#token V_IDENDOT		"[_a-zA-Z][_a-zA-Z\$0-9]*(\.[_a-zA-Z][_a-zA-Z\$0-9]*)+"  << addtoken(0); STMODE; >>
#token V_LP			"\("				<< addtoken(0); STMODE; >>
#token V_RP			"\)"				<< addtoken(0); STMODE; >>

// comment handling when in udp mode..
#token 				"`~[\n]*"			<< addtoken(0); zzskip(); >>		// handle preprocessor stuff later..
#token 				"//~[\n]*"			<< addtoken(0); parsecomment(); zzskip(); >>
#token				"/\*"				<< addtoken(0); zzcomment_depth=1; zzcomment_entry=1; zzcomment_start=zzline; zzmode(COMMENT); zzskip(); >>
#token				"\*/"				<< addtoken(0); 
									mesg("** End of comment when not in a comment on line %d in file '%s'.\n", zzline, zzfilename);
									zzskip(); >>
#lexclass UDPCOMMENT
#token				"/\*"				<< addtoken(0); /* zzcomment_depth++; <=- VERILOG DOES NOT ALLOW NESTED COMMENTS */
									zzskip(); >>
#token				"\*/"				<< addtoken(0); zzcomment_depth--; zzskip(); if(!zzcomment_depth) { zzmode(UDP); } >>
#token				"\n"				<< addtoken(1); zzskip(); >>
#token				"~[/\*\n]+"			<< addtoken(0); if(zzcomment_entry) { parsecomment(); zzcomment_entry=0; } zzskip(); >>
#token				"[/\*]"				<< addtoken(0); zzskip(); >>


// preprocessing I
#lexclass START
#token				"`uselib~[\n]*"			<< addtoken(0); zzskip(); >>
#token				"`default_nettype[\ \t\b]+[a-z]+" << addtoken(0); zzskip(); >>
#token 				"`define~[\n]*"			<< addtoken(0); if(!do_not_translate) store_define(zztext); zzskip(); >>
#token				"`undef[\ \t\b]+[a-zA-Z_][a-zA-Z0-9_$]*" << addtoken(0); if(!do_not_translate) remove_define(zztext); zzskip(); >>
#token				"`unconnected_drive[\ \t\b]+pull[01]" << addtoken(0); zzskip(); >>
#token				"`nounconnected_drive"		<< addtoken(0); zzskip(); >>
#token				"`resetall"			<< addtoken(0); zzskip(); >>
#token				"`timescale[\ \t\b]+1{0}{0}[\ \t\b]*{[munpf]}s[\ \t\b]*/[\ \t\b]*1{0}{0}[\ \t\b]*{[munpf]}s" << addtoken(0); zzskip(); >>

#token				"`ifdef[\ \t\b]+[a-zA-Z_][a-zA-Z0-9_$]*" << addtoken(0); if(!(do_not_translate&~STMODE_XLATEOFF_IFDEF)) handle_ifdef(zztext); zzskip(); >>
#token				"`ifndef[\ \t\b]+[a-zA-Z_][a-zA-Z0-9_$]*" << addtoken(0); if(!(do_not_translate&~STMODE_XLATEOFF_IFDEF)) handle_ifndef(zztext); zzskip(); >>
#token				"`else"				<< addtoken(0); if(!(do_not_translate&~STMODE_XLATEOFF_IFDEF)) handle_else(); zzskip(); >>
#token				"`endif"			<< addtoken(0); if(!(do_not_translate&~STMODE_XLATEOFF_IFDEF)) handle_endif(); zzskip(); >>
#token				"`elsif[\ \t\b]+[a-zA-Z_][a-zA-Z0-9_$]*" << addtoken(0); if(!(do_not_translate&~STMODE_XLATEOFF_IFDEF)) handle_elsif(zztext); zzskip(); >>
#token				"`include[\ \t\b]+\"~[\"\n]*\""	<< addtoken(0); if(!do_not_translate) handle_include(zztext); zzskip(); >>

#token 				"`[a-zA-Z_][a-zA-Z0-9_$]*"	<< 	addtoken(0); 
									if(is_builtin_define(zztext+1, zzrewind-1)) warn("** Warning: ignoring unsupported '%s' directive in file '%s' line %d\n", zztext, zzfilename, zzline);
									else if(!do_not_translate) 
										warn("** Warning: macro '%s' undefined in file '%s' line %d\n", zztext, zzfilename, zzline);
									zzskip(); >>

// preprocessing II (exactly same as tokens for START lexclass above)
#lexclass UDP
#token				"`uselib~[\n]*"			<< addtoken(0); zzskip(); >>
#token				"`default_nettype[\ \t\b]+[a-z]+" << addtoken(0); zzskip(); >>
#token 				"`define~[\n]*"			<< addtoken(0); if(!do_not_translate) store_define(zztext); zzskip(); >>
#token				"`undef[\ \t\b]+[a-zA-Z_][a-zA-Z0-9_$]*" << addtoken(0); if(!do_not_translate) remove_define(zztext); zzskip(); >>
#token				"`unconnected_drive[\ \t\b]+pull[01]" << addtoken(0); zzskip(); >>
#token				"`nounconnected_drive"		<< addtoken(0); zzskip(); >>
#token				"`resetall"			<< addtoken(0); zzskip(); >>
#token				"`timescale[\ \t\b]+1{0}{0}[\ \t\b]*{[munpf]}s[\ \t\b]*/[\ \t\b]*1{0}{0}[\ \t\b]*{[munpf]}s" << addtoken(0); zzskip(); >>

#token				"`ifdef[\ \t\b]+[a-zA-Z_][a-zA-Z0-9_$]*" << addtoken(0); if(!(do_not_translate&~STMODE_XLATEOFF_IFDEF)) handle_ifdef(zztext); zzskip(); >>
#token				"`ifndef[\ \t\b]+[a-zA-Z_][a-zA-Z0-9_$]*" << addtoken(0); if(!(do_not_translate&~STMODE_XLATEOFF_IFDEF)) handle_ifndef(zztext); zzskip(); >>
#token				"`else"				<< addtoken(0); if(!(do_not_translate&~STMODE_XLATEOFF_IFDEF)) handle_else(); zzskip(); >>
#token				"`endif"			<< addtoken(0); if(!(do_not_translate&~STMODE_XLATEOFF_IFDEF)) handle_endif(); zzskip(); >>
#token				"`elsif[\ \t\b]+[a-zA-Z_][a-zA-Z0-9_$]*" << addtoken(0); if(!(do_not_translate&~STMODE_XLATEOFF_IFDEF)) handle_elsif(zztext); zzskip(); >>
#token				"`include[\ \t\b]+\"~[\"\n]*\""	<< addtoken(0); if(!do_not_translate) handle_include(zztext); zzskip(); >>

#token 				"`[a-zA-Z_][a-zA-Z0-9_$]*"	<< 	addtoken(0); 
									if(is_builtin_define(zztext+1, zzrewind-1)) warn("** Warning: ignoring unsupported '%s' directive in file '%s' line %d\n", zztext, zzfilename, zzline);
									else if(!do_not_translate) 
										warn("** Warning: macro '%s' undefined in file '%s' line %d\n", zztext, zzfilename, zzline);
									zzskip(); >>


//
// section 1
//

v_source_text:	(v_description)* V_EOF
		;

v_description:	v_module
		| v_udp
		| v_config // v2k1
		;

// v2k1
v_config: V_CONFIG (~V_ENDCONFIG)* V_ENDCONFIG
		;

// v2k1
v_module_parameters:
		V_POUND	V_LP V_PARAMETER (v_range | ) v_param_assignment (V_COMMA (V_PARAMETER |) (v_range | ) v_param_assignment)* V_RP
		|
		;

v_module:	( V_MODULE | V_MACROMODULE ) 
			<< {
				struct i_symbol_scope *sb = (struct i_symbol_scope *)calloc(1, sizeof(struct i_symbol_scope)); 
			   	sb->symtable = make_jrb(); 
			   	sb->parent = sym_base;
			   	sym_base = sb;

				module_is_duplicate=0; 
				if(mod_current_filename)
					{
					free(mod_current_filename);
					}
				mod_current_filename = strdup(zzfilename);

				if(mod_current_name)
					{
					free(mod_current_name);
					mod_current_name = NULL;
					}

				mod_start_line = zzline;
			  }
			>>
			v_identifier_nodot  
			<< {
				JRB node = jrb_find_str(modname_tree, $2.symbol->name);
				if((!node)||((node)&&(!node->val.i)))
					{
					add_string_to_tree(modname_tree, $2.symbol->name, TRUE);
					mod_current_name = strdup($2.symbol->name);
										
					}
					else
					{
					warn("** Warning: skipping duplicate module '%s' at in file '%s' line %d\n", $2.symbol->name, zzfilename, zzline);
					module_is_duplicate = 1;
					}
			   }
			>>

			v_module_parameters // v2k1

			v_module_body 

			V_ENDMODULE
			<< {
				if(sym_base)
					{
					if(module_is_duplicate)
						{
						JRB symtree = sym_base->symtable;
						struct i_symbol_scope *sb = sym_base->parent;
						JRB node;

						jrb_traverse(node, symtree)
							{
							free(((struct i_symbol *)node->val.v)->name);
							free(node->val.v);	/* free up strings for any stray symbols (should be only modname) */
							}
						jrb_free_tree(symtree);						
						free(sym_base);
						sym_base = sb;
						}
						else
						{
						JRB symtree = sym_base->symtable;
						JRB node;

						sym_base = sym_base->parent;

						if(emit_stems)
							{
							char real_path[PATH_MAX];

							printf("++ module %s file %s lines %d - %d\n",
								mod_current_name, realpath(mod_current_filename, real_path), mod_start_line, zzline);
							}

						if(emit_vars)
							{
							jrb_traverse(node, symtree)						
								{
								printf("++ var %s module %s\n",
									((struct i_symbol *)node->val.v)->name,
									mod_current_name);
								}
							}

						if(mod_current_filename)
							{
							free(mod_current_filename);
							mod_current_filename = NULL;
							}

						if(mod_current_name)
							{
							free(mod_current_name);
							mod_current_name = NULL;
							}
						}
					}
			   }
			>>
		;		

v_module_body:		(v_list_of_ports|v_v2k_list_of_ports) V_SEMI
			(v_module_item)* 
			;

v_v2k_list_of_ports: V_LP v_v2k_iio_declaration (V_COMMA v_v2k_iio_declaration)* V_RP			// v2k1
		;

v_v2k_iio_declaration:	v_v2k_input_declaration | v_v2k_output_declaration | v_v2k_inout_declaration	// v2k1
		;

v_v2k_input_declaration: V_INPUT v_optnettype v_optsigned (v_range | ) v_name_of_variable		// v2k1
                ;
v_v2k_output_declaration: V_OUTPUT v_optnettype v_optsigned (v_range | ) v_name_of_variable		// v2k1
                ;
v_v2k_inout_declaration: V_INOUT v_optnettype v_optsigned (v_range | ) v_name_of_variable		// v2k1
                ;


v_list_of_ports: V_LP v_port ( V_COMMA v_port)* V_RP
		|
		;

v_port:		v_port_expression
		| V_DOT v_identifier_nodot V_LP v_port_expression V_RP
		;

v_port_expression: v_port_reference
		| V_LBRACE v_port_reference (V_COMMA v_port_reference)* V_RBRACE
		|
		;

v_port_reference: v_identifier_nodot
		( (V_LBRACK v_expression (V_COLON v_expression | )
		V_RBRACK) | )
		;

v_module_item:	v_parameter_declaration
		| v_localparam_declaration // v2k1
		| v_input_declaration
		| v_output_declaration
		| v_inout_declaration
		| v_reg_declaration
		| v_time_declaration
		| v_integer_declaration
		| v_genvar_declaration
		| v_real_declaration
		| v_event_declaration
		| v_gate_declaration
		| v_udp
		| v_module_instantiation
		| v_parameter_override
		| v_continuous_assign
		| v_initial_statement
		| v_always_statement
		| v_task
		| v_function
		| v_specify_block
		| v_generate_block
		;

v_udp:		V_PRIMITIVE 
		<< { 
			struct i_symbol_scope *sb = (struct i_symbol_scope *)calloc(1, sizeof(struct i_symbol_scope)); 
		   	sb->symtable = make_jrb(); 
		   	sb->parent = sym_base;
		   	sym_base = sb;

			module_is_duplicate = 0;

			if(udp_current_filename)
				{
				free(udp_current_filename);
				}
			udp_current_filename = strdup(zzfilename);

			if(udp_current_name)
				{
				free(udp_current_name);
				udp_current_name = NULL;
				}

			udp_start_line = zzline;
		   }
		>>
			v_name_of_udp 
			<< 
			   {
				JRB node = jrb_find_str(modname_tree, $2.symbol->name);
				if((!node)||((node)&&(!node->val.i)))
					{
					add_string_to_tree(modname_tree, $2.symbol->name, TRUE);
					udp_current_name = strdup($2.symbol->name);
					}
					else
					{
					warn("** Warning: skipping duplicate UDP '%s' at in file '%s' line %d\n", $2.symbol->name, zzfilename, zzline);
					module_is_duplicate = 1;
					}
			   }
			>>

			V_LP v_name_of_variable
			( V_COMMA v_name_of_variable )* V_RP V_SEMI
			(v_udp_declaration)+
			v_udp_initial_statement
			v_table_definition
			V_ENDPRIMITIVE
			<<
			   {
				if(sym_base)
					{
					if(module_is_duplicate)
						{
						JRB symtree = sym_base->symtable;
						struct i_symbol_scope *sb = sym_base->parent;
						JRB node;

						jrb_traverse(node, symtree)
							{
							free(((struct i_symbol *)node->val.v)->name);
							free(node->val.v);	/* free up strings for any stray symbols (should be only modname) */
							}
						jrb_free_tree(symtree);						
						free(sym_base);
						sym_base = sb;
						}
						else
						{
						sym_base = sym_base->parent;

						if(emit_stems)
							{
							printf("++ udp %s file %s lines %d - %d\n",
								udp_current_name, udp_current_filename, udp_start_line, zzline);
							}

						if(udp_current_filename)
							{
							free(udp_current_filename);
							udp_current_filename = NULL;
							}

						if(udp_current_name)
							{
							free(udp_current_name);
							udp_current_name = NULL;
							}
						}
					}
			   }
			>>
			;

v_udp_declaration: v_output_declaration
		| v_reg_declaration
		| v_input_declaration
		;

v_udp_initial_statement: V_INITIAL v_output_terminal_name V_EQ v_init_val V_SEMI
		|
		;

// v_init_val can ONLY be "1\'[bB]0", "1\'[bB]1", "1\'[bB][xX]", "1", "0" so this needs to be checked!
v_init_val:	V_DECIMAL_NUMBER 
		| V_BBASE << $$.num = i_number_basemake(NV_BBASE, $1.text); >>
		;

v_output_terminal_name: v_name_of_variable
		;

v_table_definition: V_TABLE v_table_entries V_ENDTABLE
		;

v_table_entries: (v_com_seq_entry)+
		;

v_com_seq_entry: v_input_list V_COLON ( v_state V_COLON v_next_state
			| V_OUTPUT_SYMBOL) V_SEMI
		;

v_level_symbol: V_OUTPUT_SYMBOL
                | V_LEVEL_SYMBOL_EXTRA
                ;
 
v_edge:         V_LP v_fake_edge V_RP
                | V_EDGE_SYMBOL
                ;

// XXX: V_IDENTIFIER is to handle cases like X0, etc that can greedy lex as an identifier!
v_fake_edge:	v_level_symbol v_level_symbol
		| V_IDENTIFIER
		;

v_level_symbol2: v_level_symbol
		| v_edge
		;

v_input_list: (v_level_symbol2)*
                ;
 

v_state:        v_level_symbol
                ;

v_next_state:   V_OUTPUT_SYMBOL
                | V_HYPHEN
                ;

v_task:		V_TASK 
		<< { 
			struct i_symbol_scope *sb = (struct i_symbol_scope *)calloc(1, sizeof(struct i_symbol_scope)); 
		   	sb->symtable = make_jrb(); 
		   	sb->parent = sym_base;
		   	sym_base = sb;
		   }
		>>
		v_identifier_nodot V_SEMI
			( v_tf_declaration )* 
			v_statement_or_null V_ENDTASK
		<< if(sym_base) sym_base = sym_base->parent; >>
		;

v_function:	V_FUNCTION 
		<< { 
			struct i_symbol_scope *sb = (struct i_symbol_scope *)calloc(1, sizeof(struct i_symbol_scope)); 
		   	sb->symtable = make_jrb(); 
		   	sb->parent = sym_base;
		   	sym_base = sb;
		   }
		>>
		( v_range_or_type | ) v_identifier_nodot V_SEMI
			( v_tf_declaration )+ 
			v_statement V_ENDFUNCTION
		<< if(sym_base) sym_base = sym_base->parent; >>
		;

v_range_or_type: v_range
		| V_INTEGER
		| V_REAL
		;

v_tf_declaration: v_parameter_declaration
		| v_localparam_declaration // v2k1
		| v_input_declaration
		| v_output_declaration
		| v_inout_declaration
		| v_reg_declaration
		| v_time_declaration
		| v_integer_declaration
		| v_real_declaration
		;

//
// section 2
//

v_parameter_declaration: V_PARAMETER v_optsigned (v_range | ) v_list_of_param_assignments V_SEMI
		;

// v2k1
v_localparam_declaration: V_LOCALPARAM v_optsigned (v_range | ) v_list_of_param_assignments V_SEMI
		;

v_param_assignment: v_identifier V_EQ v_constant_expression 
		;

v_list_of_param_assignments: v_param_assignment 
		(V_COMMA v_param_assignment)*
		;

v_input_declaration: V_INPUT v_optnettype v_optsigned ( v_list_of_variables V_SEMI
		| v_range v_list_of_variables V_SEMI )
		;

v_output_declaration: V_OUTPUT v_optnettype v_optsigned ( v_list_of_variables V_SEMI
		| v_range v_list_of_variables V_SEMI )
		;

v_inout_declaration: V_INOUT v_optnettype v_optsigned (v_list_of_variables V_SEMI
		| v_range v_list_of_variables V_SEMI )
		;

v_net_chg:	v_charge_strength
		|
		;

v_nettype:	V_WIRE
		| V_TRI
		| V_TRI1
		| V_SUPPLY0
		| V_WAND
		| V_TRIAND
		| V_TRI0
		| V_SUPPLY1
		| V_WOR
		| V_TRIOR
		| V_TRIREG
		;

// v2k1
v_optnettype:	v_nettype
		| V_REG
		|
		;

v_expandrange:	v_range
		| V_SCALARED v_range
		| V_VECTORED v_range
		;

v_reg_declaration: V_REG v_optsigned v_reg_range v_list_of_register_variables V_SEMI
		;

v_reg_range:	v_range
		|
		;

v_time_declaration: V_TIME v_optsigned (v_range | ) v_list_of_register_variables V_SEMI
		;

v_integer_declaration: V_INTEGER v_optsigned (v_range | ) v_list_of_register_variables V_SEMI
		;

v_genvar_declaration: V_GENVAR v_optsigned (v_range | ) v_list_of_register_variables V_SEMI
		;

v_real_declaration: V_REAL v_optsigned (v_range | ) v_list_of_register_variables V_SEMI
		;

v_event_declaration: V_EVENT v_name_of_event (V_COMMA v_name_of_event)* V_SEMI
		;

v_continuous_assign: V_ASSIGN v_cont_drv v_cont_dly v_list_of_assignments V_SEMI
		| v_nettype 
		v_optsigned // v2k1
		v_net_chg 
		v_cont_exr v_cont_dly
		(v_list_of_variables | v_list_of_assignments) V_SEMI
		;

v_cont_drv:	v_drive_strength
		|
		;

v_cont_exr:	v_expandrange
		|
		;

v_cont_dly:	v_delay
		|
		;


v_parameter_override: V_DEFPARAM v_list_of_param_assignments V_SEMI
		;

v_list_of_variables: v_name_of_variable v_optrange_list
		(V_COMMA v_name_of_variable v_optrange_list)*
		;

v_optrange_list: (v_range | )
		;

v_name_of_variable: v_identifier
		;

v_list_of_register_variables: v_register_variable
		(V_COMMA v_register_variable)*
		;

v_register_variable: v_name_of_register
		| v_name_of_register V_EQ v_expression // v2k1
		| v_name_of_memory V_LBRACK v_expression V_COLON
			v_expression V_RBRACK
		;

v_name_of_register: v_identifier
		;

v_name_of_memory: v_identifier
		;

v_name_of_event: v_identifier
		;

v_charge_strength: V_LP (V_SMALL 
		| V_MEDIUM
		| V_LARGE
		| v_strength0 V_COMMA v_strength1
		| v_strength1 V_COMMA v_strength0) V_RP
		;

v_drive_strength: V_LP (v_strength0 V_COMMA v_strength1
		|  v_strength1 V_COMMA v_strength0) V_RP
		;

v_strength0:	V_SUPPLY0
		| V_STRONG0
		| V_PULL0
		| V_WEAK0
		| V_HIGHZ0
		;

v_strength1:	V_SUPPLY1
		| V_STRONG1
		| V_PULL1
		| V_WEAK1
		| V_HIGHZ1
		;
	
// v2k1
v_optsigned:	V_SIGNED
		|
		;

v_range:	V_LBRACK v_expression 
			V_COLON v_expression  V_RBRACK
		;

v_list_of_assignments: v_assignment (V_COMMA v_assignment)* 
		;


//
// section 3
//

v_gate_declaration: v_gatetype v_gate_drv v_gate_dly v_gate_instance
			(V_COMMA v_gate_instance)* V_SEMI
			;
v_gatetype: 
		V_ANDLIT | V_NANDLIT | V_ORLIT | V_NORLIT | V_XORLIT | V_XNORLIT
		| V_BUF | V_BUFIF0 | V_BUFIF1 | V_NOTLIT | V_NOTIF0 | V_NOTIF1 | V_PULLDOWN | V_PULLUP
		| V_NMOS | V_RNMOS | V_PMOS | V_RPMOS | V_CMOS | V_RCMOS | V_TRAN | V_RTRAN 
		| V_TRANIF0 | V_RTRANIF0 | V_TRANIF1 | V_RTRANIF1
		;

v_gate_drv: v_drive_strength	
		|
		;

v_gate_dly: v_delay
		|
		;

v_gate_range: v_range
		|
		;


v_gate_instance: v_name_of_gate_instance V_LP v_terminal 
		(V_COMMA v_terminal)* V_RP
		;

v_name_of_gate_instance: v_identifier_nodot v_gate_range
		|
		;

v_terminal: v_expression 
		|
		;

v_udp_instantiation: v_name_of_udp v_gate_drv v_gate_dly v_udp_instance
			(V_COMMA v_udp_instance)* V_SEMI
		;

v_name_of_udp: v_identifier_nodot
		<< $$.symbol = $1.symbol; >>
		;

v_udp_instance: v_name_of_udp_instance V_LP v_terminal
                (V_COMMA v_terminal)* V_RP
		;

v_name_of_udp_instance: v_identifier_nodot v_gate_range
		|
		;


//
// section 4
//
v_module_instantiation: v_name_of_module 
			<< 
				if(!module_is_duplicate)
					{
					add_string_to_tree(modname_tree, $1.symbol->name, FALSE);
					if(comp_type_name)
						{
						free(comp_type_name);
						}
					comp_type_name = strdup($1.symbol->name);
					} /* to keep transitive closure from looking for nonexistant modules if duplicate modules differ! */
			>>
			v_parameter_value_assignment
			v_module_instance
			(V_COMMA v_module_instance)* V_SEMI
			<<
			if(comp_type_name)
				{
				free(comp_type_name);
				comp_type_name = NULL;
				}
			>>
			;

v_name_of_module: v_identifier_nodot << $$.symbol = $1.symbol; >>
		;

v_parameter_value_assignment: 
		V_POUND V_LP v_mexplist V_RP
		| V_POUND v_number
		|
		;

v_module_instance: v_name_of_instance_opt V_LP v_list_of_module_connections V_RP
		;

// XXX: anon module names...
v_name_of_instance_opt: v_name_of_instance
		|
		;

v_name_of_instance: v_identifier_nodot v_mod_range
		<<
			if(!module_is_duplicate)
				{
				if(emit_stems)
					{
					printf("++ comp %s type %s parent %s\n", $1.symbol->name, comp_type_name, mod_current_name);
					}	
				}
		>>
		;

v_mod_range:	v_range
		|
		;

v_list_of_module_connections: v_module_port_connection (V_COMMA v_module_port_connection)*
		| v_named_port_connection (V_COMMA v_named_port_connection)*
		;

v_module_port_connection: v_expression
		|
		;

v_named_port_connection: V_DOT v_identifier_nodot V_LP (v_expression | ) V_RP
		;


//
// section 5
//

v_initial_statement: V_INITIAL v_statement
		;

v_always_statement: V_ALWAYS v_statement
		;

v_statement_or_null: v_statement
		| V_SEMI
		;

v_statement: 	v_block_or_non_assignment V_SEMI
		| V_IF V_LP v_expression V_RP v_statement_or_null 
			( V_ELSE v_statement_or_null | )
		| V_CASE V_LP v_expression V_RP (v_case_item)+ V_ENDCASE
		| V_CASEX V_LP v_expression V_RP (v_case_item)+ V_ENDCASE
		| V_CASEZ V_LP v_expression V_RP (v_case_item)+ V_ENDCASE
		| V_FOREVER v_statement
		| V_REPEAT V_LP v_expression V_RP v_statement
		| V_WHILE V_LP v_expression V_RP v_statement
		| V_FOR V_LP v_assignment V_SEMI v_expression V_SEMI
			v_assignment V_RP v_statement		
		| v_delay_or_event_control_stmt v_statement_or_null
		| V_WAIT V_LP v_expression V_RP v_statement_or_null
		| V_RARROW v_name_of_event V_SEMI
		| v_seq_block
		| v_par_block
		| v_task_enable
		| V_DISABLE (v_name_of_task_or_block) V_SEMI
		| V_ASSIGN v_assignment V_SEMI
		| V_DEASSIGN v_lvalue V_SEMI
		| V_FORCE v_assignment V_SEMI
		| V_RELEASE v_lvalue V_SEMI
		;

v_assignment: 	v_lvalue V_EQ v_expression
		;

v_block_or_non_assignment: v_lvalue ( v_blocking_assignment 
		| v_non_blocking_assignment)
		;

v_blocking_assignment: V_EQ ( v_expression
		| v_delay_or_event_control v_expression )
		;

v_non_blocking_assignment: V_LEQ ( v_expression
		| v_delay_or_event_control v_expression )
		;

v_delay_or_event_control: v_delay_control
		| v_event_control
		| V_REPEAT V_LP v_expression V_RP v_event_control
		;

v_delay_or_event_control_stmt: v_delay_control
		| v_event_control
		;

v_case_item:	v_explist 
			V_COLON v_statement_or_null
		| V_DEFAULT (V_COLON v_statement_or_null
			| v_statement_or_null)
		;

v_seq_block:	V_BEGIN 
		<< { 
			struct i_symbol_scope *sb = (struct i_symbol_scope *)calloc(1, sizeof(struct i_symbol_scope)); 
		   	sb->symtable = make_jrb(); 
		   	sb->parent = sym_base;
		   	sym_base = sb;
		   }
		>>
		( (v_statement)* | 
		V_COLON v_name_of_block (v_block_declaration)*
		(v_statement)* ) 
		V_END
		<< if(sym_base) sym_base = sym_base->parent; >>
		;

v_par_block:	V_FORK 
		<< { 
			struct i_symbol_scope *sb = (struct i_symbol_scope *)calloc(1, sizeof(struct i_symbol_scope)); 
		   	sb->symtable = make_jrb(); 
		   	sb->parent = sym_base;
		   	sym_base = sb;
		   }
		>>
		( (v_statement)* | 
		V_COLON v_name_of_block (v_block_declaration)*
		(v_statement)* ) V_JOIN
		<< if(sym_base) sym_base = sym_base->parent; >>
		;

v_name_of_block: v_identifier_nodot
		;

v_block_declaration: v_parameter_declaration
		| v_localparam_declaration // v2k1
		| v_reg_declaration
		| v_integer_declaration
		| v_real_declaration
		| v_time_declaration
		| v_event_declaration
		;

v_task_enable: 	v_name_of_task V_SEMI
		| v_name_of_task V_LP v_explist V_RP V_SEMI
		;

v_name_of_task: v_identifier
		;

// for disable ()
v_name_of_task_or_block: v_identifier_nodot
		;

v_generate_block: V_GENERATE (~V_ENDGENERATE)* V_ENDGENERATE
                ;


//
// section 6 (specify blocks not supported, just skip over them with negative token matching)
//
v_specify_block: V_SPECIFY (~V_ENDSPECIFY)* V_ENDSPECIFY
		;

v_constant_expression: v_expression
		<< $$.prim = $1.prim; >>
		;

//
// section 7
//
v_lvalue:       v_identifier
                (
                V_LBRACK v_expression
                ( V_COLON v_expression
                << $$.prim = i_primary_symrange_make($1.1.symbol,$2.2.prim,$3.2.prim); >>
                | << $$.prim = i_primary_symbit_make($1.1.symbol,$2.2.prim); >>
                ) V_RBRACK
		v_opt_array_handling // v2k1 FIXME for symmake
		| << $$.prim = i_primary_make(PRIM_SYMBOL,$1.1.symbol); >>
                )
                | v_concatenation << $$.prim = $1.prim; >>
                ;

v_expression:	<< push_exp_now(); >> v_expression2 << 
			if(!zzerrors)
				{
				push_oper(i_oper_make(V_EOF,0));
				push_primary(NULL);
				pop_exp_now(); 
				$$.prim = shred_expression(); 
				}
				else
				{
				$$.prim = NULL;
				}
			>>
		;

v_expression2:	( v_primary	<< push_primary($1.prim); >> 
			| v_unary_operator v_primary 
				<< push_primary(NULL); push_oper($1.oper); push_primary($2.prim); >>
			| V_STRING << push_primary(i_primary_make(PRIM_NUMBER, i_number_basemake(NV_STRING, $1.text))); >> )
		( v_binary_operator << push_oper($1.oper); >> v_expression2 
			| V_QUEST v_expression V_COLON v_expression 
			<< push_oper(i_oper_make(V_QUEST,1));
			    push_primary(i_bin_expr_make($2.prim,i_oper_make(V_COLON, 1),$4.prim)); >>
			| )
		;

v_mintypmax_expression: v_expression 
		(V_COLON v_expression V_COLON v_expression 
	          << $$.prim = i_primary_mintypmax_make($1.1.prim, $2.2.prim, $2.4.prim); >>
		| << $$.prim = $1.1.prim; >> )
		;

v_unary_operator: V_PLUS	<< $$.oper = i_oper_make(V_PLUS, 12); >>
		| V_MINUS	<< $$.oper = i_oper_make(V_MINUS,12); >>
		| V_BANG	<< $$.oper = i_oper_make(V_BANG, 12); >>
		| V_TILDE	<< $$.oper = i_oper_make(V_TILDE,12); >>
		| V_AND		<< $$.oper = i_oper_make(V_AND,  12); >>
		| V_NAND	<< $$.oper = i_oper_make(V_NAND, 12); >>
		| V_OR		<< $$.oper = i_oper_make(V_OR,   12); >>
		| V_NOR		<< $$.oper = i_oper_make(V_NOR,  12); >>
		| V_XOR		<< $$.oper = i_oper_make(V_XOR,  12); >>
		| V_XNOR	<< $$.oper = i_oper_make(V_XNOR, 12); >>
		| V_XNOR2	<< $$.oper = i_oper_make(V_XNOR, 12); >>
		;

v_binary_operator: V_POW	<< $$.oper = i_oper_make(V_POW,  11); >>
		| V_STAR	<< $$.oper = i_oper_make(V_STAR, 10); >>
		| V_SLASH	<< $$.oper = i_oper_make(V_SLASH,10); >>
		| V_MOD		<< $$.oper = i_oper_make(V_MOD,  10); >>
		| V_PLUS	<< $$.oper = i_oper_make(V_PLUS,  9); >>
		| V_MINUS	<< $$.oper = i_oper_make(V_MINUS, 9); >>
		| V_SHL		<< $$.oper = i_oper_make(V_SHL,   8); >>
		| V_SHR		<< $$.oper = i_oper_make(V_SHR,   8); >>
		| V_SSHL	<< $$.oper = i_oper_make(V_SSHL,  8); >>
		| V_SSHR	<< $$.oper = i_oper_make(V_SSHR,  8); >>
		| V_LT		<< $$.oper = i_oper_make(V_LT,    7); >>
		| V_LEQ		<< $$.oper = i_oper_make(V_LEQ,   7); >>
		| V_GT		<< $$.oper = i_oper_make(V_GT,    7); >>
		| V_GEQ		<< $$.oper = i_oper_make(V_GEQ,   7); >>
		| V_EQ2		<< $$.oper = i_oper_make(V_EQ2,   6); >>
		| V_NEQ		<< $$.oper = i_oper_make(V_NEQ,   6); >>
		| V_EQ3		<< $$.oper = i_oper_make(V_EQ3,   6); >>
		| V_NEQ2	<< $$.oper = i_oper_make(V_NEQ2,  6); >>
		| V_AND		<< $$.oper = i_oper_make(V_AND,   5); >>
		| V_NAND	<< $$.oper = i_oper_make(V_NAND,  5); >>
		| V_XOR		<< $$.oper = i_oper_make(V_XOR,   5); >>
		| V_XNOR	<< $$.oper = i_oper_make(V_XNOR,  5); >>
		| V_XNOR2	<< $$.oper = i_oper_make(V_XNOR,  5); >>
		| V_OR		<< $$.oper = i_oper_make(V_OR,    4); >>
		| V_NOR		<< $$.oper = i_oper_make(V_NOR,   4); >>
		| V_AND2	<< $$.oper = i_oper_make(V_AND2,  3); >>
		| V_OR2		<< $$.oper = i_oper_make(V_OR2,   2); >>
		;	

// v2k1
v_opt_array_handling: ( V_LBRACK v_expression (V_COLON v_expression |) V_RBRACK | );

v_primary:	v_number	<< $$.prim = i_primary_make(PRIM_NUMBER,$1.num); >>
		| v_identifier 
		  ( V_LBRACK v_expression 
		  ( V_RBRACK 				
			<< $$.prim= i_primary_symbit_make($1.1.symbol,$2.2.prim); >> 
				v_opt_array_handling // v2k1 FIXME for symmake
			| V_COLON v_expression V_RBRACK 
			<< $$.prim= i_primary_symrange_make($1.1.symbol,$2.2.prim,$3.2.prim); >> ) 
			| << $$.prim= i_primary_make(PRIM_SYMBOL,$1.1.symbol); >> )
		| v_multiple_concatenation		<< $$.prim = $1.prim; >>
		| v_function_call			<< $$.prim = $1.prim; >>
		| V_LP v_mintypmax_expression V_RP	<< $$.prim = $2.prim; >>
		;		


v_number:	V_DECIMAL_NUMBER << $$.num = i_number_make($1.1.ival); >>
		| V_HBASE  << $$.num = i_number_basemake(NV_HBASE, $1.text); >>
		| V_DBASE << $$.num = i_number_basemake(NV_DBASE, $1.text); >>
		| V_BBASE << $$.num = i_number_basemake(NV_BBASE, $1.text); >>
		| V_OBASE << $$.num = i_number_basemake(NV_OBASE, $1.text); >>
		| V_FLOAT1	  << $$.num = i_number_fmake($1.rval); >>
		| V_FLOAT2	  << $$.num = i_number_fmake($1.rval); >>
		;

v_concatenation:
		V_LBRACE v_explist V_RBRACE
		<< $$.prim = i_primary_concat_make(NULL,$2.explist); >>
		;

v_multiple_concatenation:
		<< struct i_explist *lroot=NULL, *lcurrent=NULL; >>
		V_LBRACE v_expression
			<< lroot=lcurrent=(struct i_explist *)calloc(1,sizeof(struct i_explist));
				lcurrent->item=$2.prim; 
			>>
			(
			(V_COMMA v_expression
			<< lcurrent->next=(struct i_explist *)calloc(1,sizeof(struct i_explist));
			lcurrent=lcurrent->next;
			lcurrent->item=$2.prim; 
			>>
			)*
			<< $$.prim = i_primary_concat_make(NULL,i_explist_make(lroot)); >>
			|
			V_LBRACE v_explist V_RBRACE
			<< $$.prim = i_primary_concat_make(lroot->item,$2.explist); 
			free(lroot);
			>>
			)
		V_RBRACE
		;

v_function_call: v_name_of_function V_LP v_explist V_RP
		<< $$.prim=i_primary_funcall_make($1.symbol,$3.explist); >>
		;

v_name_of_function: v_identifier_nodot	<< $$.symbol = $1.symbol; >>
		;

v_explist:	<< struct i_explist *lroot=NULL, *lcurrent=NULL; >>
		v_fn_expression
			<< lroot=lcurrent=(struct i_explist *)calloc(1,sizeof(struct i_explist));
				lcurrent->item=$1.prim; 
			>>
		(V_COMMA v_fn_expression
			<< lcurrent->next=(struct i_explist *)calloc(1,sizeof(struct i_explist));
			lcurrent=lcurrent->next;
			lcurrent->item=$2.prim; 
			>>
		)*
		<< $$.explist=i_explist_make(lroot); >>
		;

v_fn_expression:
		v_expression << $$.prim = $1.prim; >>
		| << $$.prim = NULL; >>
		;

// these are the special versions for named parameters ... which are present in verilog 2001
v_mexplist:	<< struct i_explist *lroot=NULL, *lcurrent=NULL; >>
		v_mfn_expression
			<< lroot=lcurrent=(struct i_explist *)calloc(1,sizeof(struct i_explist));
				lcurrent->item=$1.prim; 
			>>
		(V_COMMA v_mfn_expression
			<< lcurrent->next=(struct i_explist *)calloc(1,sizeof(struct i_explist));
			lcurrent=lcurrent->next;
			lcurrent->item=$2.prim; 
			>>
		)*
		<< $$.explist=i_explist_make(lroot); >>
		;

v_mfn_expression:
		V_DOT v_identifier_nodot V_LP v_expression V_RP 
			<< 
			struct i_primary *ip = i_primary_make(PRIM_NAMEDPARAM,NULL);
			ip->primval.named_param.sym = $2.symbol;
			ip->primval.named_param.exp = $4.prim;			

			$$.prim = ip;
			>>
		| v_expression << $$.prim = $1.prim; >>
		| << $$.prim = NULL; >>
		;

//
// section 8
//

v_identifier:	v_identifier_nodot 	<< $$.symbol = $1.symbol; >>
		| V_IDENDOT 		<< $$.symbol = $1.symbol; >>
		;

v_identifier_nodot:
		V_IDENTIFIER	 	<< $$.symbol = $1.symbol; >>
		| V_IDENTIFIER2 	<< $$.symbol = $1.symbol; >>
		| V_FUNCTION_NAME       << $$.symbol = $1.symbol; >>
		;

v_delay:	V_POUND ( v_number
		| v_identifier
		| V_LP v_mintypmax_expression 
		(V_COMMA v_mintypmax_expression)* V_RP ) // complain if > 3 mintypmax expressions!
		;

v_delay_control: V_POUND ( v_number
		| v_identifier
		| V_LP v_mintypmax_expression V_RP )
		;

v_event_control: V_AT ( v_identifier
		| V_LP v_event_expression V_RP 
		| V_STAR // v2k1
		)
		;

v_event_expression: 
		v_event_expression2 ( v_orcomma v_event_expression2 )*
                ;

v_orcomma:	V_ORLIT
		| V_COMMA // v2k1
		;

v_event_expression2: (v_expression
                | V_POSEDGE v_expression
                | V_NEGEDGE v_expression 
		| V_STAR // v2k1
		)
                ;

