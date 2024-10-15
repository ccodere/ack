/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* make dependencies; Date: jan 07, 1986; Author: Erik Baalbergen */
/* Log:
 [Thu Oct  6 09:56:30 MET 1988; erikb]
 Added option '-d' which suppresses "file.c :" be printed
 [2024-09-27; ceco]
 Added options to add suffixes and prefixes. Added help.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BSIZ 1024
char *prog;
/* object file suffix */
char* osuffix;
/* object file prefix */
char* oprefix;
/** search system directory includes, those that start with < */
int searchsys;
/** The command to execute for each target, NULL if no command should be output */
char* command;

/** Namelist */
struct namelist *nl = NULL;
/** Include list */
struct namelist* includes = NULL;

int dflag = 0; /* suppress "file.c :" */

/** Linked list of strings. */
struct namelist
{
	struct namelist *next;
	char *name;
};

struct namelist *new_namelist();

char *Malloc(unsigned int);

char *include_line(char *);
int dofile(char *);
int dosrcfile(char *fn);

/** Similar to POSIX call, returns
 *  a pointer to the location of the
 *  filename part of the path. This
 *  function is portable across different platforms.
 *
 */
static char *basename(char *path)
{
	char* result;
	if (path == NULL)
	{
		return NULL;
	}
	/* UNIX path specification */
	result = strrchr(path,'/');
	if (result != NULL)
	{
		result++;
		return result;
	}
	/* Windows path specification */
	result = strrchr(path,'\\');
	if (result != NULL)
	{
		result++;
		return result;
	}
	/* Windows drive specification / AmigaOS / others */
	result = strrchr(path,':');
	if (result != NULL)
	{
		result++;
		return result;
	}
	return path;
}


char *Malloc(unsigned int u)
{
	char *sp;

	if ((sp = malloc(u)) == 0)
	{
		fprintf(stderr, "%s:", " out of space\n");
		exit(1);
	}
	return sp;
}

char *strdup(const char *s1)
{
	char* newstr;
	newstr = Malloc(strlen(s1)+1);
	strcpy(newstr,s1);
	return newstr;
}

/** Copies up to n characters, and adds
 *  a null character at end.
 */
char *strndup(const char *s1, size_t n)
{
	char* newstr;
	newstr = Malloc(n+1);
	strncpy(newstr,s1,n);
	newstr[n] = 0;
	return newstr;
}


struct namelist *new_namelist()
{
	return (struct namelist *) Malloc(sizeof(struct namelist));
}

void free_namelist(struct namelist *nlp)
{
	if (nlp)
	{
		free_namelist(nlp->next);
		nlp->next = NULL;
		free(nlp);
	}
}

void add_name(struct namelist **list,char *nm)
{
	struct namelist *nlp = *list, *lnlp = 0, *nnlp;

	/* search through the linked list. */
	while (nlp)
	{
		register int i = strcmp(nm, nlp->name);
		if (i < 0)
			break;
		if (i == 0) /* already present */
			return;
		lnlp = nlp;
		nlp = nlp->next;
	}

	nnlp = new_namelist();
	nnlp->name = strcpy(Malloc((unsigned) strlen(nm) + 1),
			nm);

	if (lnlp)
	{
		nnlp->next = lnlp->next;
		lnlp->next = nnlp;
	}
	else
	{
		nnlp->next = *list;
		*list = nnlp;
	}
}

void print_namelist(char *nm, struct namelist *nlp)
{
	char* filename;
	char* suffix;
	char* finalname;
		/* Get the filename only */
		filename = basename(nm);
		/* Get the filename without the suffix */
		suffix = strrchr(filename, '.');
		if (suffix == NULL)
		{
			if (!dflag)
				printf("%s%s%s: %s", oprefix,filename,osuffix,filename);
		} else
		{
			finalname = strndup(filename,strlen(filename)-strlen(suffix));
			if (!dflag)
				printf("%s%s%s: %s", oprefix,finalname,osuffix,filename);
			free(finalname);
		}
		while (nlp)
		{
		  printf(" %s", nlp->name);
		  nlp = nlp->next;
		}
		printf("\n");
		if (command != NULL)
		{
		  printf("\t%s\n", command);
		}
}

static void print_help()
{
	printf("usage: mkdep [-oobjsuffix] [-oobjprefix] [-ccommand] [-d] file ...\n");
	printf("-ccommand\n");
	printf("  Set the command to execute for each target\n");
	printf("-d\n");
	printf("  Do not print target\n");
	printf("-Iincludedir\n");
	printf("  Include directories to search in for include files\n");
	printf("-oobjsuffix\n");
	printf("  Object file suffix override. Default value is '.o'\n");
	printf("-pobjprefix\n");
	printf("  Object file prefix override. The prefix is prepended to the name of the object file. Default value is an empty string.\n");
	printf("-Y\n");
	printf("  Exclude system header files from the header file search.\n");
	exit(EXIT_FAILURE);
}

/*ARGSUSED*/
int main(int argc, char *argv[])
{
	int err = 0;
	int index =0;
	/** searchsystem include directories */
	searchsys = 1;
	command = NULL;

	if (argc == 1)
	{
		print_help();
	}

	osuffix = NULL;
	oprefix = NULL;
	prog = argv[index++];

	while (index < argc)
	{
		char* arg = argv[index];
		if (arg[0] == '-')
		{
			switch (arg[1])
			{
				case 'd':
					dflag = 1;
					break;
				case 'c':
				    command = strdup(&arg[2]);
					break;
				case 'o':
					osuffix = strdup(&arg[2]);
					break;
				case 'I':
					add_name(&includes,strdup(&arg[2]));
					break;
				/* disable searching system directories */
				case 'Y':
					searchsys = 0;
					break;
				case 'p':
					oprefix = strdup(&arg[2]);
					break;
				default:
					printf("Invalid option : %s\n",arg);
					print_help();
			}
		} else
		{
			break;
		}
		index++;
	}

	if (osuffix == NULL)
	{
		osuffix = strdup(".o");
	}
	if (oprefix == NULL)
	{
		oprefix = strdup("");
	}
	while (argv[index]!=NULL)
	{
		free_namelist(nl);
		nl = 0;
		if (dosrcfile(argv[index]) == 0)
			++err;
		print_namelist(argv[index], nl);
		index++;
	}
	free(osuffix);
	free(oprefix);
	free_namelist(includes);
	exit(err ? EXIT_FAILURE : EXIT_SUCCESS);
}

/** Try to open the specified file. Also looks
 *  into all specified include directories to search
 *  for the files.
 *
 *
 */
static FILE* try_open(char *fn)
{
	FILE *fp = NULL;
	char* name;
	struct namelist *nlp = includes;
	fp = fopen(fn, "r");
	if (fp != NULL)
		return fp;

	name = Malloc(strlen(fn)+1);
	/* Search for all include file directories */
	while (nlp)
	{
		name = realloc(name,strlen(fn)+strlen(nlp->name)+1+1);
		sprintf(name,"%s/%s",nlp->name,fn);
		fp = fopen(name, "r");
		if (fp != NULL)
			break;

		nlp = nlp->next;
	}
    free(name);
    return fp;
}


int dosrcfile(char *fn)
{
	char buf[BSIZ];
	FILE *fp;
	char *nm;

	if ((fp = fopen(fn, "r")) == NULL)
	{
		fprintf(stderr, "%s: cannot read %s\n", prog, fn);
		return 0;
	}

	while (fgets(buf, BSIZ, fp) != NULL)
		if ((nm = include_line(buf)))
		{
			add_name(&nl,nm);
			if (dofile(nm))
				;
		}

	fclose(fp);
	return 1;
}


int dofile(char *fn)
{
	char buf[BSIZ];
	FILE *fp;
	char *nm;

	if ((fp = try_open(fn)) == NULL)
	{
		fprintf(stderr, "%s: cannot read %s\n", prog, fn);
		return 0;
	}

	while (fgets(buf, BSIZ, fp) != NULL)
		if ((nm = include_line(buf)))
		{
			add_name(&nl,nm);
			if (dofile(nm))
				;
		}

	fclose(fp);
	return 1;
}

char *include_line(char *s)
{
	while ((*s == '\t') || (*s == ' '))
		s++;

	if (*s++ == '#')
	{
		while ((*s == '\t') || (*s == ' '))
			s++;
		if ((*s++ == 'i') && (*s++ == 'n') && (*s++ == 'c') && (*s++ == 'l')
				&& (*s++ == 'u') && (*s++ == 'd') && (*s++ == 'e'))
		{
			while ((*s == '\t') || (*s == ' '))
				s++;
			if (*s == '"')
			{
				s++;
				char *nm = s;

				while (*s != 0 && *s != '"')
					s++;
				*s = '\0';
				return nm;
			}
			if (searchsys==1)
			{
				if (*s == '<')
				{
					s++;
					char *nm = s;

					while (*s != 0 && *s != '>')
						s++;
					*s = '\0';
					return nm;
				}
			}
		}
	}
	return NULL;
}
