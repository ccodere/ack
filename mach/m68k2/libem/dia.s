.define	.diagnos

space	= 040
del	= 0177

	.text
.diagnos:
	move.w	hol0,-(sp)
	move.l	hol0+FILN_AD,d2
	beq	1f
	move.l	d2,a0
	move.l	#40,d0
3:
	move.b	(a0)+,d1
	beq	2f
	cmp.b	#del,d1
	bge	1f
	cmp.b	#space,d1
	blt	1f
	sub	#1,d0
	bgt	3b
	clr.b	(a1)
2:
	move.l	d2,-(sp)
	pea	fmt
	jsr	_printf
	add	#10,sp
	jmp	_printf

1:
	move.l	#unknwn,d2
	bra	2b

	.data
fmt:	.asciz "%s, line %d: "
unknwn:	.asciz "unknown file"
.align 2
