.define __link
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
.extern __link
__link:			trap #0
.data2	0x9
			bcc	1f
			jmp	cerror
1:
			clr.l	d0
			rts
