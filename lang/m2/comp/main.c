/* M A I N   P R O G R A M */

#include	"debug.h"
#include	"ndir.h"

#include	<system.h>
#include	<em_arith.h>
#include	<em_label.h>

#include	"input.h"
#include	"f_info.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"type.h"
#include	"def.h"
#include	"scope.h"
#include	"standards.h"
#include	"tokenname.h"
#include	"node.h"
#include	"warning.h"

int		state;			/* either IMPLEMENTATION or PROGRAM */
char		options[128];
int		DefinitionModule; 
char		*ProgName;
char		*DEFPATH[NDIRS+1];
struct def 	*Defined;
extern int 	err_occurred;
extern int	fp_used;		/* set if floating point used */

main(argc, argv)
	register char **argv;
{
	register int Nargc = 1;
	register char **Nargv = &argv[0];

	ProgName = *argv++;
	warning_classes = W_INITIAL;

	while (--argc > 0) {
		if (**argv == '-')
			DoOption((*argv++) + 1);
		else
			Nargv[Nargc++] = *argv++;
	}
	Nargv[Nargc] = 0;	/* terminate the arg vector	*/
	if (Nargc < 2) {
		fprint(STDERR, "%s: Use a file argument\n", ProgName);
		return 1;
	}
	return !Compile(Nargv[1], Nargv[2]);
}

Compile(src, dst)
	char *src, *dst;
{
	extern struct tokenname tkidf[];

	if (! InsertFile(src, (char **) 0, &src)) {
		fprint(STDERR,"%s: cannot open %s\n", ProgName, src);
		return 0;
	}
	LineNumber = 1;
	FileName = src;
	DEFPATH[0] = "";
	DEFPATH[NDIRS] = 0;
	init_idf();
	InitCst();
	reserve(tkidf);
	InitScope();
	InitTypes();
	AddStandards();
#ifdef DEBUG
	if (options['l']) {
		LexScan();
		return 1;
	}
#endif DEBUG
	open_scope(CLOSEDSCOPE);
	GlobalScope = CurrentScope;
	C_init(word_size, pointer_size);
	if (! C_open(dst)) fatal("could not open output file");
	C_magic();
	C_ms_emx(word_size, pointer_size);
	CompUnit();
	C_ms_src((arith) (LineNumber - 1), FileName);
	close_scope(SC_REVERSE);
	if (!err_occurred) {
		C_exp(Defined->mod_vis->sc_scope->sc_name);
		WalkModule(Defined);
		if (fp_used) C_ms_flt();
	}
	C_close();
#ifdef DEBUG
	if (options['i']) Info();
#endif
	return ! err_occurred;
}

#ifdef DEBUG
LexScan()
{
	register struct token *tkp = &dot;
	extern char *symbol2str();

	while (LLlex() > 0) {
		print(">>> %s ", symbol2str(tkp->tk_symb));
		switch(tkp->tk_symb) {

		case IDENT:
			print("%s\n", tkp->TOK_IDF->id_text);
			break;
		
		case INTEGER:
			print("%ld\n", tkp->TOK_INT);
			break;
		
		case REAL:
			print("%s\n", tkp->TOK_REL);
			break;

		case STRING:
			print("\"%s\"\n", tkp->TOK_STR);
			break;

		default:
			print("\n");
		}
	}
}
#endif

AddStandards()
{
	register struct def *df;
	extern struct def *Enter();
	static struct node nilnode = { 0, 0, Value, 0, { INTEGER, 0}};

	(void) Enter("ABS", D_PROCEDURE, std_type, S_ABS);
	(void) Enter("CAP", D_PROCEDURE, std_type, S_CAP);
	(void) Enter("CHR", D_PROCEDURE, std_type, S_CHR);
	(void) Enter("FLOAT", D_PROCEDURE, std_type, S_FLOAT);
	(void) Enter("HIGH", D_PROCEDURE, std_type, S_HIGH);
	(void) Enter("HALT", D_PROCEDURE, std_type, S_HALT);
	(void) Enter("EXCL", D_PROCEDURE, std_type, S_EXCL);
	(void) Enter("DEC", D_PROCEDURE, std_type, S_DEC);
	(void) Enter("INC", D_PROCEDURE, std_type, S_INC);
	(void) Enter("VAL", D_PROCEDURE, std_type, S_VAL);
	(void) Enter("NEW", D_PROCEDURE, std_type, S_NEW);
	(void) Enter("DISPOSE", D_PROCEDURE, std_type, S_DISPOSE);
	(void) Enter("TRUNC", D_PROCEDURE, std_type, S_TRUNC);
	(void) Enter("SIZE", D_PROCEDURE, std_type, S_SIZE);
	(void) Enter("ORD", D_PROCEDURE, std_type, S_ORD);
	(void) Enter("ODD", D_PROCEDURE, std_type, S_ODD);
	(void) Enter("MAX", D_PROCEDURE, std_type, S_MAX);
	(void) Enter("MIN", D_PROCEDURE, std_type, S_MIN);
	(void) Enter("INCL", D_PROCEDURE, std_type, S_INCL);

	(void) Enter("CHAR", D_TYPE, char_type, 0);
	(void) Enter("INTEGER", D_TYPE, int_type, 0);
	(void) Enter("LONGINT", D_TYPE, longint_type, 0);
	(void) Enter("REAL", D_TYPE, real_type, 0);
	(void) Enter("LONGREAL", D_TYPE, longreal_type, 0);
	(void) Enter("BOOLEAN", D_TYPE, bool_type, 0);
	(void) Enter("CARDINAL", D_TYPE, card_type, 0);
	df = Enter("NIL", D_CONST, address_type, 0);
	df->con_const = &nilnode;
	nilnode.nd_INT = 0;
	nilnode.nd_type = address_type;

	(void) Enter("PROC",
		     D_TYPE,
		     construct_type(T_PROCEDURE, NULLTYPE),
		     0);
	df = Enter("BITSET", D_TYPE, bitset_type, 0);
	df = Enter("TRUE", D_ENUM, bool_type, 0);
	df->enm_val = 1;
	df->enm_next = Enter("FALSE", D_ENUM, bool_type, 0);
	df = df->enm_next;
	df->enm_val = 0;
	df->enm_next = 0;
}

/* How do you like that! Modula-2 in a C-program.
*/
char SYSTEM[] = "\
DEFINITION MODULE SYSTEM;\n\
TYPE	PROCESS = ADDRESS;\n\
PROCEDURE NEWPROCESS(P:PROC; A:ADDRESS; n:CARDINAL; VAR p1:ADDRESS);\n\
PROCEDURE TRANSFER(VAR p1,p2:ADDRESS);\n\
END SYSTEM.\n";

do_SYSTEM()
{
	/*	Simulate the reading of the SYSTEM definition module
	*/
	open_scope(CLOSEDSCOPE);
	(void) Enter("WORD", D_TYPE, word_type, 0);
	(void) Enter("ADDRESS", D_TYPE, address_type, 0);
	(void) Enter("ADR", D_PROCEDURE, std_type, S_ADR);
	(void) Enter("TSIZE", D_PROCEDURE, std_type, S_TSIZE);
	if (!InsertText(SYSTEM, sizeof(SYSTEM) - 1)) {
		fatal("could not insert text");
	}
	DefModule();
	close_scope(SC_CHKFORW);
}

#ifdef DEBUG

int	cntlines;

Info()
{
	extern int cnt_def, cnt_node, cnt_paramlist, cnt_type,
		   cnt_switch_hdr, cnt_case_entry, 
		   cnt_scope, cnt_scopelist, cnt_forwards, cnt_tmpvar;

	print("\
%6d def\n%6d node\n%6d paramlist\n%6d type\n%6d switch_hdr\n\
%6d case_entry\n%6d scope\n%6d scopelist\n%6d forwards\n%6d tmpvar\n",
cnt_def, cnt_node, cnt_paramlist, cnt_type,
cnt_switch_hdr, cnt_case_entry, 
cnt_scope, cnt_scopelist, cnt_forwards, cnt_tmpvar);
print("\nNumber of lines read: %d\n", cntlines);
}
#endif
