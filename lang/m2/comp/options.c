/* U S E R   O P T I O N - H A N D L I N G */

#include	"idfsize.h"
#include	"ndir.h"

#include	<em_arith.h>
#include	<em_label.h>

#include	"type.h"
#include	"main.h"
#include	"warning.h"

extern int	idfsize;
static int	ndirs;
int		warning_classes;

DoOption(text)
	register char *text;
{
	switch(*text++)	{

	default:
		options[text[-1]]++;	/* flags, debug options etc.	*/
		break;
					/* recognized flags:
						-L: don't generate fil/lin
						-p: generate procentry/procexit
						-w: no warnings
						-n: no register messages
					   and many more if DEBUG
					*/


	case 'w':
		if (*text) {
			while (*text) {
				switch(*text++) {
				case 'O':
					warning_classes &= ~W_OLDFASHIONED;
					break;
				case 'R':
					warning_classes &= ~W_STRICT;
					break;
				case 'W':
					warning_classes &= ~W_ORDINARY;
					break;
				}
			}
		}
		else warning_classes = 0;
		break;

	case 'W':
		while (*text) {
			switch(*text++) {
			case 'O':
				warning_classes |= W_OLDFASHIONED;
				break;
			case 'R':
				warning_classes |= W_STRICT;
				break;
			case 'W':
				warning_classes |= W_ORDINARY;
				break;
			}
		}
		break;

	case 'M': {	/* maximum identifier length */
		char *t = text;		/* because &text is illegal */

		idfsize = txt2int(&t);
		if (*t || idfsize <= 0)
			fatal("malformed -M option");
		if (idfsize > IDFSIZE)
			fatal("maximum identifier length is %d", IDFSIZE);
		}
		break;

	case 'I' :
		if (++ndirs >= NDIRS) {
			fatal("too many -I options");
		}
		DEFPATH[ndirs] = text;
		break;

	case 'V' :	/* set object sizes and alignment requirements	*/
	{
		arith size;
		int align;
		char c;
		char *t;

		while (c = *text++)	{
			t = text;
			size = txt2int(&t);
			align = 0;
			if (*(text = t) == '.')	{
				t = text + 1;
				align = txt2int(&t);
				text = t;
			}
			switch (c)	{

			case 'w':	/* word		*/
				if (size != (arith)0) {
					word_size = size;
					dword_size = 2 * size;
				}
				if (align != 0) word_align = align;
				break;
			case 'i':	/* int		*/
				if (size != (arith)0) int_size = size;
				if (align != 0) int_align = align;
				break;
			case 'l':	/* longint	*/
				if (size != (arith)0) long_size = size;
				if (align != 0) long_align = align;
				break;
			case 'f':	/* real		*/
				if (size != (arith)0) float_size = size;
				if (align != 0) float_align = align;
				break;
			case 'd':	/* longreal	*/
				if (size != (arith)0) double_size = size;
				if (align != 0) double_align = align;
				break;
			case 'p':	/* pointer	*/
				if (size != (arith)0) pointer_size = size;
				if (align != 0) pointer_align = align;
				break;
			case 'S':	/* initial record alignment	*/
				if (align != (arith)0) struct_align = align;
				break;
			default:
				error("-V: bad type indicator %c\n", c);
			}
		}
		break;
	}
	}
}

int
txt2int(tp)
	register char **tp;
{
	/*	the integer pointed to by *tp is read, while increasing
		*tp; the resulting value is yielded.
	*/
	register int val = 0;
	register int ch;
	
	while (ch = **tp, ch >= '0' && ch <= '9')	{
		val = val * 10 + ch - '0';
		(*tp)++;
	}
	return val;
}
