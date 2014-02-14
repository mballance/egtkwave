#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vpp_common.h"

struct arglines_t
{
struct arglines_t *next;
char *s;
int count;
};

static struct arglines_t *read_file(char *name)
{
FILE *f = fopen(name, "rb");
struct arglines_t *head = NULL, *curr = NULL;
char buf[8193];

if(!f)
	{
	warn("** Problem reading args file '%s'.\n", name);
	perror("Why");
	return(NULL);
	}

while(!feof(f))
	{
	int len;
	int i;
	char *srt;

	if(!fgets(buf, 8192, f)) buf[0] = 0;
	len = strlen(buf);
	if(!len) break;
	buf[len-1] = 0;

	for(i=len-2;i>=0;i--)
		{
		if(isspace((int)(unsigned char)buf[i]))
			{
			buf[i] = 0;
			continue;
			}
		break;
		}

	srt = NULL;
	for(i=0;i<len;i++)
		{
		if(buf[i] && !isspace((int)(unsigned char)buf[i]))
			{
			srt = buf+i;
			break;
			}
		}

	if(!srt) continue;
	len = strlen(srt);
	if(!len) continue;

	if((srt[0]=='+')||((srt[0]=='-')&&(srt[1]=='D'))) /* do not parse spaces out of front/back of plusargs and defines */
		{
		if(!head)
			{
			head = curr = calloc(1, sizeof(struct arglines_t));
			head->s = strdup(srt);
			head->count = 1;
			}
			else
			{
			struct arglines_t *l = calloc(1, sizeof(struct arglines_t));
			l->s = strdup(srt);
			curr->next = l;
			curr = l;
			head->count++;
			}
		}
		else
		{
		char *srt_new;
		int first = 1;

		while((srt_new = strtok(first ? srt : NULL, " \t")))
			{
			first = 0;

			if(!head)
				{
				head = curr = calloc(1, sizeof(struct arglines_t));
				head->s = strdup(srt_new);
				head->count = 1;
				}
				else
				{
				struct arglines_t *l = calloc(1, sizeof(struct arglines_t));
				l->s = strdup(srt_new);
				curr->next = l;
				curr = l;
				head->count++;
				}
			}
		}
	}

fclose(f);
return(head);
}

void args_expand(int argc, char **argv, int *new_argc, char ***new_argv)
{
int i;
int has_dashf = 0;
int total_args;
struct arglines_t **shadow_list;

for(i=1;i<argc;i++)
	{
	if(!strcmp(argv[i], "-f"))
		{
		if(i==(argc-1))
			{
			warn("** Skipping -f missing filename as a final argument.\n");
			argc--;
			break;
			}

		has_dashf = 1;
		}	
	}

if(!has_dashf)
	{
	*new_argc = argc;
	*new_argv = argv;
	}

shadow_list = calloc(argc, sizeof(struct arglines_t *));
total_args = 1; /* argv[0] */
for(i=1;i<argc;i++)
	{
	if(!strcmp(argv[i], "-f"))
		{
		struct arglines_t *al = read_file(argv[i+1]);

		if(al)
			{
			shadow_list[i] = al;
			total_args += al->count;
			}
		i++;
		}
		else
		{
		total_args++;
		}
	}

*new_argc = total_args;
*new_argv = calloc(total_args, sizeof(char *));

total_args = 0;
for(i=0;i<argc;i++)
	{
	if(!strcmp(argv[i], "-f"))
		{
		struct arglines_t *al = shadow_list[i];
		struct arglines_t *al_nxt;
		while(al)
			{
			al_nxt = al->next;
			(*new_argv)[total_args++] = al->s;
			free(al);
			al = al_nxt;
			}
		i++;
		}
		else
		{
		(*new_argv)[total_args] = strdup(argv[i]);
		total_args++;
		}
	}
}

