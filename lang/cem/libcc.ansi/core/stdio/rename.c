/*
 * rename.c - rename a file to a new name
 *
 * This implementation is a dumb implementation
 * that should be overriden by platform specific
 * code.
 */
/* $Id$ */
#include <stdio.h>

#if ACKCONF_WANT_STDIO

int rename(const char *old, const char *new)
{
	FILE* target;
	int c;
	FILE* source = fopen(old,"rb");
	if (source == NULL)
	{
		return -1;
	}
	target  = fopen(new,"wb");
	if (target == NULL)
	{
		fclose(source);
		return -1;
	}
	while (feof(source)==0)
	{
		int c = getc(source);
		if ((c==EOF) || (putc(c,target)!=c))
		{
			fclose(source);
			fclose(target);
			remove(new);
			return -1;
		}
	}
	if ((fclose(source)==EOF) || (fclose(target)==EOF) || (remove(old)!=0))
	{
		return -1;
	}
	return 0;
}

#endif
