/* $Header$ */
/*	C O D E - G E N E R A T I N G   R O U T I N E S		*/

#include	"nofloat.h"
#include	<em.h>
#include	"dataflow.h"
#include	"use_tmp.h"
#include	"botch_free.h"
#include	"arith.h"
#include	"type.h"
#include	"idf.h"
#include	"label.h"
#include	"code.h"
#include	"stmt.h"
#include	"alloc.h"
#include	"def.h"
#include	"expr.h"
#include	"sizes.h"
#include	"stack.h"
#include	"level.h"
#include	"decspecs.h"
#include	"declar.h"
#include	"Lpars.h"
#include	"mes.h"
#include	"LLlex.h"
#include	"specials.h"
#include	"storage.h"
#include	"atw.h"
#include	"assert.h"

label lab_count = 1;
label datlab_count = 1;

#ifndef NOFLOAT
int fp_used;
#endif NOFLOAT

extern char options[];
char *symbol2str();

init_code(dst_file)
	char *dst_file;
{
	/*	init_code() initialises the output file on which the
		compact EM code is written
	*/
	C_init(word_size, pointer_size); /* initialise EM module */
	if (C_open(dst_file) == 0)
		fatal("cannot write to %s\n", dst_file);
#ifndef	USE_TMP
	famous_first_words();
#endif	USE_TMP
}

famous_first_words()
{
	C_magic();
	C_ms_emx(word_size, pointer_size);
}

static struct string_cst *str_list = 0;

code_string(val, len, dlb)
	char *val;
	int len;
	label dlb;
{
	register struct string_cst *sc = new_string_cst();

	C_ina_dlb(dlb);
	sc->next = str_list;
	str_list = sc;
	sc->sc_value = val;
	sc->sc_len = len;
	sc->sc_dlb = dlb;
}

def_strings(sc)
	register struct string_cst *sc;
{
	if (sc) {
		def_strings(sc->next);
		C_df_dlb(sc->sc_dlb);
		str_cst(sc->sc_value, sc->sc_len);
		free_string_cst(sc);
	}
}

end_code()
{
	/*	end_code() performs the actions to be taken when closing
		the output stream.
	*/
	def_strings(str_list);
	str_list = 0;
	C_ms_src((int)(LineNumber - 2), FileName);
	C_close();
}

#ifdef	USE_TMP
prepend_scopes(dst_file)
	char *dst_file;
{
	/*	prepend_scopes() runs down the list of global idf's
		and generates those exa's, exp's, ina's and inp's
		that superior hindsight has provided, on the file dst_file.
	*/
	register struct stack_entry *se = local_level->sl_entry;

	if (C_open(dst_file) == 0)
		fatal("cannot create %s", dst_file ? dst_file : "stdout");
	famous_first_words();
	while (se != 0)	{
		register struct idf *id = se->se_idf;
		register struct def *df = id->id_def;
		
		if (df && (df->df_initialized || df->df_used || df->df_alloc))
			code_scope(id->id_text, df);
		se = se->next;
	}
	C_close();
}
#endif	USE_TMP

code_scope(text, def)
	char *text;
	register struct def *def;
{
	/*	generates code for one name, text, of the storage class
		as given by def, if meaningful.
	*/
	int fund = def->df_type->tp_fund;
	
	switch (def->df_sc)	{
	case EXTERN:
	case GLOBAL:
	case IMPLICIT:
		if (fund == FUNCTION)
			C_exp(text);
		else
			C_exa_dnam(text);
		break;
	case STATIC:
		if (fund == FUNCTION)
			C_inp(text);
		else
			C_ina_dnam(text);
		break;
	}
}

static label return_label;
static char return_expr_occurred;
static struct type *func_tp;
static label func_res_label;
static char *last_fn_given = "";
static label file_name_label;

begin_proc(name, def)	/* to be called when entering a procedure	*/
	char *name;
	register struct def *def;
{
	/*	begin_proc() is called at the entrance of a new function
		and performs the necessary code generation:
		-	a scope indicator (if needed) exp/inp
		-	the procedure entry pro $name
		-	reserves some space if the result of the function
			does not fit in the return area
		-	a fil pseudo instruction
	*/
	arith size;
	register struct type *tp = def->df_type;

#ifndef	USE_TMP
	code_scope(name, def);
#endif	USE_TMP
#ifdef	DATAFLOW
	if (options['d'])
		DfaStartFunction(name);
#endif	DATAFLOW

	if (tp->tp_fund != FUNCTION) {
		error("making function body for non-function");
		tp = error_type;
	}
	else
		tp = tp->tp_up;
	func_tp = tp;
	size = ATW(tp->tp_size);
	C_pro_narg(name);
	if (is_struct_or_union(tp->tp_fund))	{
		C_df_dlb(func_res_label = data_label());
		C_bss_cst(size, (arith)0, 1);
	}
	else
		func_res_label = 0;
	/*	Special arrangements if the function result doesn't fit in
		the function return area of the EM machine.  The size of
		the function return area is implementation dependent.
	*/
	lab_count = (label) 1;
	return_label = text_label();
	return_expr_occurred = 0;
	if (! options['L'])	{	/* profiling */
		if (strcmp(last_fn_given, FileName) != 0)	{
			/* previous function came from other file */
			C_df_dlb(file_name_label = data_label());
			C_con_scon(last_fn_given = FileName,
				(arith)(strlen(FileName) + 1));
		}
		/* enable debug trace of EM source */
		C_fil_dlb(file_name_label, (arith)0);
		C_lin((arith)LineNumber);
	}
}

end_proc(fbytes, nbytes)
	arith fbytes, nbytes;
{
	/*	end_proc() deals with the code to be generated at the end of
		a function, as there is:
		-	the EM ret instruction: "ret 0"
		-	loading of the function result in the function
			result area if there has been a return <expr>
			in the function body (see do_return_expr())
		-	indication of the use of floating points
		-	indication of the number of bytes used for
			formal parameters
		-	use of special identifiers such as "setjmp"
		-	"end" + number of bytes used for local variables
	*/
	static int mes_flt_given = 0;	/* once for the whole program */

#ifdef	DATAFLOW
	if (options['d'])
		DfaEndFunction();
#endif	DATAFLOW
	C_ret((arith)0);
	if (return_expr_occurred != 0)	{
		C_df_ilb(return_label);
		if (func_res_label != 0)	{
			C_lae_dlb(func_res_label, (arith)0);
			store_block(func_tp->tp_size, func_tp->tp_align);
			C_lae_dlb(func_res_label, (arith)0);
			C_ret(pointer_size);
		}
		else
			C_ret(ATW(func_tp->tp_size));
	}
#ifndef NOFLOAT
	if (fp_used && mes_flt_given == 0)	{
		/* floating point used	*/
		C_ms_flt();
		mes_flt_given++;
	}
#endif NOFLOAT
	C_ms_par(fbytes);		/* # bytes for formals		*/
	if (sp_occurred[SP_SETJMP]) {	/* indicate use of "setjmp"	*/
		C_ms_gto();
		sp_occurred[SP_SETJMP] = 0;
	}
	C_end(ATW(nbytes));
}

do_return()
{
	/*	do_return generates a direct return */
	/*	isn't a jump to the return label smarter ??? */
	C_ret((arith)0);
}

do_return_expr(expr)
	struct expr *expr;
{
	/*	do_return_expr() generates the expression and the jump for
		a return statement with an expression.
	*/
	ch7cast(&expr, RETURN, func_tp);
	code_expr(expr, RVAL, TRUE, NO_LABEL, NO_LABEL);
	C_bra(return_label);
	return_expr_occurred = 1;
}

code_declaration(idf, expr, lvl, sc)
	register struct idf *idf;	/* idf to be declared	*/
	struct expr *expr;	/* initialisation; NULL if absent	*/
	int lvl;		/* declaration level	*/
	int sc;			/* storage class, as in the declaration */
{
	/*	code_declaration() does the actual declaration of the
		variable indicated by "idf" on declaration level "lvl".
		If the variable is initialised, the expression is given
		in "expr".
		There are some cases to be considered:
		-	filter out typedefs, they don't correspond to code;
		-	global variables, coded only if initialized;
		-	local static variables;
		-	local automatic variables;
		Since the expression may be modified in the process,
		code_declaration() frees it after use, as the caller can
		no longer do so.
		
		If there is a storage class indication (EXTERN/STATIC),
		code_declaration() will generate an exa or ina.
		The sc is the actual storage class, as given in the
		declaration.  This is to allow:
			extern int a;
			int a = 5;
		while at the same time forbidding
			extern int a = 5;
	*/
	char *text = idf->id_text;
	register struct def *def = idf->id_def;
	register arith size = def->df_type->tp_size;
	int def_sc = def->df_sc;
	
	if (def_sc == TYPEDEF)	/* no code for typedefs		*/
		return;
	if (sc == EXTERN && expr && !is_anon_idf(idf))
		error("%s is extern; cannot initialize", text);
	if (lvl == L_GLOBAL)	{	/* global variable	*/
		/* is this an allocating declaration? */
		if (	(sc == 0 || sc == STATIC)
			&& def->df_type->tp_fund != FUNCTION
			&& size >= 0
		)
			def->df_alloc = ALLOC_SEEN;
		if (expr) {	/* code only if initialized */
#ifndef	USE_TMP
			code_scope(text, def);
#endif	USE_TMP
			def->df_alloc = ALLOC_DONE;
			C_df_dnam(text);
			do_ival(&(def->df_type), expr);
			free_expression(expr);
		}
	}
	else
	if (lvl >= L_LOCAL)	{	/* local variable	*/
		/* STATIC, EXTERN, GLOBAL, IMPLICIT, AUTO or REGISTER */
		switch (def_sc)	{
		case STATIC:
			if (def->df_type->tp_fund == FUNCTION) {
				/* should produce "inp $function" ??? */
				break;
			}
			/*	they are handled on the spot and get an
				integer label in EM.
			*/
			C_df_dlb((label)def->df_address);
			if (expr) { /* there is an initialisation */
				do_ival(&(def->df_type), expr);
				free_expression(expr);
			}
			else {	/* produce blank space */
				if (size <= 0) {
					error("size of %s unknown", text);
					size = (arith)0;
				}
				C_bss_cst(align(size, word_align), (arith)0, 1);
			}
			break;
		case EXTERN:
		case GLOBAL:
		case IMPLICIT:
			/* we are sure there is no expression */
#ifndef	USE_TMP
			code_scope(text, def);
#endif	USE_TMP
			break;
		case AUTO:
		case REGISTER:
			if (expr)
				loc_init(expr, idf);
			break;
		default:
			crash("bad local storage class");
			break;
		}
	}
}

loc_init(expr, id)
	struct expr *expr;
	struct idf *id;
{
	/*	loc_init() generates code for the assignment of
		expression expr to the local variable described by id.
		It frees the expression afterwards.
	*/
	register struct type *tp = id->id_def->df_type;
	register struct expr *e = expr;
	
	ASSERT(id->id_def->df_sc != STATIC);
	switch (tp->tp_fund)	{
	case ARRAY:
	case STRUCT:
	case UNION:
		error("no automatic aggregate initialisation");
		free_expression(e);
		return;
	}
	if (ISCOMMA(e))	{	/* embraced: int i = {12};	*/
		if (options['R'])	{
			if (ISCOMMA(e->OP_LEFT)) /* int i = {{1}} */
				expr_error(e, "extra braces not allowed");
			else
			if (e->OP_RIGHT != 0) /* int i = {1 , 2} */
				expr_error(e, "too many initializers");
		}
		while (e)	{
			loc_init(e->OP_LEFT, id);
			e = e->OP_RIGHT;
		}
	}
	else	{	/* not embraced	*/
		struct value vl;

		ch7cast(&expr, '=', tp);	/* may modify expr */
		EVAL(expr, RVAL, TRUE, NO_LABEL, NO_LABEL);
		free_expression(expr);
		vl.vl_class = Name;
		vl.vl_data.vl_idf = id;
		vl.vl_value = (arith)0;
		store_val(&vl, tp);
	}
}

bss(idf)
	register struct idf *idf;
{
	/*	bss() allocates bss space for the global idf.
	*/
	arith size = idf->id_def->df_type->tp_size;
	
#ifndef	USE_TMP
	code_scope(idf->id_text, idf->id_def);
#endif	USE_TMP
	/*	Since bss() is only called if df_alloc is non-zero, and
		since df_alloc is only non-zero if size >= 0, we have:
	*/
	/*	but we already gave a warning at the declaration of the
		array. Besides, the message given here does not apply to
		voids
	
	if (options['R'] && size == 0)
		warning("actual array of size 0");
	*/
	C_df_dnam(idf->id_text);
	C_bss_cst(align(size, word_align), (arith)0, 1);
}

formal_cvt(df)
	register struct def *df;
{
	/*	formal_cvt() converts a formal parameter of type char or
		short from int to that type.
	*/
	register struct type *tp = df->df_type;

	if (tp->tp_size != int_size &&
		(tp->tp_fund == CHAR || tp->tp_fund == SHORT)
	) {
		C_lol(df->df_address);
		/* conversion(int_type, df->df_type); ???
		   No, you can't do this on the stack! (CJ)
		*/
		C_lal(df->df_address);
		C_sti(tp->tp_size);
		df->df_register = REG_NONE;
	}
}

code_expr(expr, val, code, tlbl, flbl)
	struct expr *expr;
	label tlbl, flbl;
{
	/*	code_expr() is the parser's interface to the expression code
		generator.  If line number trace is wanted, it generates a
		lin instruction.  EVAL() is called directly.
	*/
	if (! options['L'])	/* profiling	*/
		C_lin((arith)LineNumber);
	EVAL(expr, val, code, tlbl, flbl);
}

/*	The FOR/WHILE/DO/SWITCH stacking mechanism:
	stack_stmt() has to be called at the entrance of a
	for, while, do or switch statement to indicate the
	EM labels where a subsequent break or continue causes
	the program to jump to.
*/
static struct stmt_block *stmt_stack;	/* top of statement stack */

/*	code_break() generates EM code needed at the occurrence of "break":
	it generates a branch instruction to the break label of the
	innermost statement in which break has a meaning.
	As "break" is legal in any of 'while', 'do', 'for' or 'switch',
	which are the only ones that are stacked, only the top of
	the stack is interesting.
*/
code_break()
{
	register struct stmt_block *stmt_block = stmt_stack;

	if (stmt_block)
		C_bra(stmt_block->st_break);
	else
		error("break not inside for, while, do or switch");
}

/*	code_continue() generates EM code needed at the occurrence of
	"continue":
	it generates a branch instruction to the continue label of the
	innermost statement in which continue has a meaning.
*/
code_continue()
{
	register struct stmt_block *stmt_block = stmt_stack;

	while (stmt_block)	{
		if (stmt_block->st_continue)	{
			C_bra(stmt_block->st_continue);
			return;
		}
		stmt_block = stmt_block->next;
	}
	error("continue not inside for, while or do");
}

stack_stmt(break_label, cont_label)
	label break_label, cont_label;
{
	register struct stmt_block *stmt_block = new_stmt_block();

	stmt_block->next = stmt_stack;
	stmt_block->st_break = break_label;
	stmt_block->st_continue = cont_label;
	stmt_stack = stmt_block;
}

unstack_stmt()
{
	/*	unstack_stmt() unstacks the data of a statement
		which may contain break or continue
	*/
	register struct stmt_block *sbp = stmt_stack;
	stmt_stack = stmt_stack->next;
	free_stmt_block(sbp);
}
