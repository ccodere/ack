/*
 * pattern contains the optimization patterns in an apparently
 * unordered fashion. All patterns follow each other unaligned.
 * Each pattern looks as follows:
 *   Byte 0:	high byte of hash value associated with this pattern.
 *   Byte 1-2:	index of next pattern with same low byte of hash value.
 *   Byte 3- :	pattern and replacement.
 *                First comes the pattern length
 *                then the pattern opcodes,
 *		  then a boolean expression,
 *		  then the one-byte replacement length
 *		  then the intermixed pattern opcodes and operands or
 *		  0 followed by the one-byte special optimization expression.
 *   If the DIAGOPT option is set, the optimization is followed
 *   by the line number in the tables.
 */

/* #define ALLOWSPECIAL /* Special optimizations allowed */

#define PO_HASH		0
#define PO_NEXT		1
#define PO_MATCH	3

struct exprnode {
	short ex_operator;
	short ex_lnode;
	short ex_rnode;
};
typedef struct exprnode expr_t;
typedef struct exprnode *expr_p;

/*
 * contents of .ex_operator
 */

#define EX_CON		0
#define EX_ARG		1
#define EX_CMPEQ	2
#define EX_CMPNE	3
#define EX_CMPGT	4
#define EX_CMPGE	5
#define EX_CMPLT	6
#define EX_CMPLE	7
#define EX_OR2		8
#define EX_AND2		9
#define EX_OR1		10
#define EX_XOR1		11
#define EX_AND1		12
#define EX_PLUS		13
#define EX_MINUS	14
#define EX_TIMES	15
#define EX_DIVIDE	16
#define EX_MOD		17
#define EX_LSHIFT	18
#define EX_RSHIFT	19
#define EX_UMINUS	20
#define EX_NOT		21
#define EX_COMP		22
#define EX_ROM		23
#define EX_NOTREG	24
#define EX_POINTERSIZE	25
#define EX_WORDSIZE	26
#define EX_DEFINED	27
#define EX_SAMESIGN	28
#define EX_SFIT		29
#define EX_UFIT		30
#define EX_ROTATE	31
#define N_EX_OPS	32	/* must be one higher then previous */


/*
 * Definition of special opcodes used in patterns
 */

#define op_pfirst op_LLP
#define op_LLP	(op_last+1)
#define op_LEP	(op_last+2)
#define op_SLP	(op_last+3)
#define op_SEP	(op_last+4)
#define op_plast op_SEP

/*
 * Definition of the structure in which instruction operands
 * are kept during pattern matching.
 */

typedef struct eval eval_t;
typedef struct eval *eval_p;

struct eval {
	short	e_typ;
	union {
		offset	e_con;
		num_p	e_np;
	} e_v;
};

/*
 * contents of .e_typ
 */
#define EV_UNDEF	0
#define EV_CONST	1
#define EV_NUMLAB	2
#define EV_FRAG		3	/* and all higher numbers */

typedef struct iarg iarg_t;
typedef struct iarg *iarg_p;

struct iarg {
	eval_t	ia_ev;
	sym_p	ia_sp;
};

/*
 * The next extern declarations refer to data generated by mktab
 */

extern byte pattern[];
extern short  lastind;
extern iarg_t iargs[];
extern byte nparam[];
extern bool nonumlab[];
extern bool onlyconst[];
extern expr_t enodes[];
