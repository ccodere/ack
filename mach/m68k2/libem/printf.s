.define _printn
.define _printf
.text
_putchar:
	move.w	#1,-(sp)
	pea	7(sp)
	move.w	#1,-(sp)
	jsr	_write
	add.l	#8,sp
	rts
_printf:
	link	a6,#-12
.data
_12:
	.short	28786
	.short	26990
	.short	29798
	.short	11875
	.short	0
.text
	pea	8+4(a6)
	move.l	(sp)+,-6(a6)
I004:
	move.l	8+0(a6),-(sp)
	move.l	(sp),-(sp)
	move.l	(sp)+,a0
	add	#1,a0
	move.l	a0,-(sp)
	move.l	(sp)+,8+0(a6)
	move.l	(sp)+,a0
	clr	d0
	move.b	(a0),d0
	move.w	d0,-(sp)
	move.w	(sp),-(sp)
	move.w	(sp)+,-2(a6)
	move.w	#37,-(sp)
	move.w	(sp)+,d0
	cmp	(sp)+,d0
	beq	I005
	move.w	-2(a6),-(sp)
	tst	(sp)+
	beq	I002
	move.w	-2(a6),-(sp)
	jsr	_putchar
	add	#2,sp
	jmp	I004
I005:
	move.l	8+0(a6),-(sp)
	move.l	(sp),-(sp)
	move.l	(sp)+,a0
	add	#1,a0
	move.l	a0,-(sp)
	move.l	(sp)+,8+0(a6)
	move.l	(sp)+,a0
	clr	d0
	move.b	(a0),d0
	move.w	d0,-(sp)
	move.w	(sp)+,-2(a6)
	move.w	-2(a6),-(sp)
	move.w	#100,-(sp)
	move.w	(sp)+,d0
	cmp	(sp)+,d0
	beq	I008
	move.w	-2(a6),-(sp)
	move.w	#117,-(sp)
	move.w	(sp)+,d0
	cmp	(sp)+,d0
	bne	I007
I008:
	move.l	-6(a6),-(sp)
	move.l	(sp)+,a0
	add	#2,a0
	move.l	a0,-(sp)
	move.l	(sp),-(sp)
	move.l	(sp)+,-6(a6)
	move.l	(sp)+,a0
	move.w	-2(a0),-(sp)
	move.w	(sp)+,-8(a6)
	move.w	-2(a6),-(sp)
	move.w	#100,-(sp)
	move.w	(sp)+,d0
	cmp	(sp)+,d0
	bne	I009
	move.w	-8(a6),-(sp)
	tst	(sp)+
	bge	I009
	move.w	#0,-(sp)
	move.w	-8(a6),-(sp)
	move.w	(sp)+,d0
	move.w	(sp)+,d1
	sub	d0,d1
	move.w	d1,-(sp)
	move.w	(sp)+,-8(a6)
	move.w	#45,-(sp)
	jsr	_putchar
	add	#2,sp
I009:
	move.w	-8(a6),-(sp)
	jsr	_printn
	add	#2,sp
	jmp	I004
I007:
	move.w	-2(a6),-(sp)
	move.w	#115,-(sp)
	move.w	(sp)+,d0
	cmp	(sp)+,d0
	bne	I004
	move.l	-6(a6),-(sp)
	move.l	(sp)+,a0
	add	#4,a0
	move.l	a0,-(sp)
	move.l	(sp),-(sp)
	move.l	(sp)+,-6(a6)
	move.l	(sp)+,a0
	move.l	-4(a0),-(sp)
	move.l	(sp)+,-12(a6)
I00c:
	move.l	-12(a6),-(sp)
	move.l	(sp),-(sp)
	move.l	(sp)+,a0
	add	#1,a0
	move.l	a0,-(sp)
	move.l	(sp)+,-12(a6)
	move.l	(sp)+,a0
	clr	d0
	move.b	(a0),d0
	move.w	d0,-(sp)
	move.w	(sp),-(sp)
	move.w	(sp)+,-2(a6)
	tst	(sp)+
	beq	I004
	move.w	-2(a6),-(sp)
	jsr	_putchar
	add	#2,sp
	jmp	I00c
I002:
	unlk	a6
	rts
_printn:
	link	a6,#-2
.data
_15:
	.short	12337
	.short	12851
	.short	13365
	.short	13879
	.short	14393
	.short	0
.text
	move.w	8+0(a6),-(sp)
	move.w	#10,-(sp)
	move.w	(sp)+,d0
	clr.l	d1
	move.w	(sp)+,d1
	divu	d0,d1
	move.w	d1,-(sp)
	move.w	(sp),-(sp)
	move.w	(sp)+,-2(a6)
	tst	(sp)+
	beq	I013
	move.w	-2(a6),-(sp)
	jsr	_printn
	add	#2,sp
I013:
	pea	_15
	move.w	8+0(a6),-(sp)
	move.w	#10,-(sp)
	move.w	(sp)+,d0
	clr.l	d1
	move.w	(sp)+,d1
	divu	d0,d1
	swap	d1
	move.w	d1,-(sp)
	move.w	(sp)+,d0
	ext.l	d0
	add.l	(sp)+,d0
	move.l	d0,-(sp)
	move.l	(sp)+,a0
	clr	d0
	move.b	(a0),d0
	move.w	d0,-(sp)
	jsr	_putchar
	add	#2,sp
	unlk	a6
	rts
