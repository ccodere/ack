.define .mli

        ! $Header$
	! #bytes in ax
.mli:
	pop     bx              ! return address
	cmp     ax,2
	jne     1f
	pop     ax
	pop     cx
	mul     cx
	push    ax
	jmp     bx
1:
	mov     dx,bx
	cmp     ax,4
	jne     9f
	pop     si
	pop     di
	pop     bx
	pop     ax
	push    dx
	jmp    .mli4
9:
	mov     ax,EODDZ
	push    ax
	jmp     .trp
