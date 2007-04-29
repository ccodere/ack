/* $Header$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* private inclusion file */

#include <em_arith.h>
#include <em_label.h>

/* include the EM description files */
#include	<em_spec.h>
#include	<em_pseu.h>
#include	<em_mnem.h>
#include	<em_reg.h>

/* macros used in the definitions of the interface functions C_* */
#define	OP(x)		C_pt_op(x)
#define	CST(x)		C_pt_cst(x)
#define	DCST(x)		C_pt_cst(x)
#define	SCON(x,y)	C_pt_scon((x), (y))
#define	PS(x)		C_pt_ps(x)
#define	DLB(x)		C_pt_dlb(x)
#define DFDLB(x)	C_pt_dlb(x)
#define	ILB(x)		C_pt_ilb(x)
#define	DFILB(x)	C_pt_dfilb(x)
#define	NOFF(x,y)	C_pt_noff((x), (y))
#define	DOFF(x,y)	C_pt_doff((x), (y))
#define	PNAM(x)		C_pt_pnam(x)
#define	DNAM(x)		C_pt_dnam(x)
#define	DFDNAM(x)	C_pt_dnam(x)
#define	CEND()
#define CCEND()		C_pt_ccend()
#define	WCON(x,y,z)	C_pt_wcon((x), (y), (z))
#define COMMA()		C_pt_comma()
#define NL()		C_pt_nl()
#define CILB(x)		C_pt_ilb(x)