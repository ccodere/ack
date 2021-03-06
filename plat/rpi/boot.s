#
/*
 * Raspberry Pi support library for the ACK
 * © 2013 David Given
 * This file is redistributable under the terms of the 3-clause BSD license.
 * See the file 'Copying' in the root of the distribution for the full text.
 */

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

#define gp r15
#define STACKSIZE 1*1024

! MAIN ENTRY POINT

begtext:
	! This empty space is required by the boot loader.

kernel_start:
	! When running as a kernel, we need to preserve all registers. We save
	! them onto the default stack.
	push r0-r24
	b baremetal_start
	.space 506 ! first 512 bytes are ignored by the boot loader
baremetal_start:
	! Wipe the bss (including the new stack).

	lea r6, begbss
	lea r7, endbss
	mov r8, #0
_1:
	stb r8, (r6)
	addcmpb.lt r6, #1, r7, _1

	! Save system registers.

	st fp, .returnfp
	st sp, .returnsp
	st lr, .returnlr

	lea gp, begtext
	lea sp, .stack + STACKSIZE

	! Save the kernel parameters.

	sub r0, gp ! fix up pointer
	sub r1, gp ! fix up pointer
	sub r2, gp ! fix up pointer
	sub r3, gp ! fix up pointer
	push r0-r5
	sub r0, sp, gp
	st r0, _pi_kernel_parameters

	! Push standard parameters onto the stack and go.
	
	mov r0, #0
	push r0                 ! envp
	push r0                 ! argv
	push r0                 ! argc

	! Call the language startup code.

	bl __m_a_i_n

	! Fall through to __exit if this returns.

.define __exit
__exit:
	! It only makes sense to get here if we're in kernel mode. If we're in
	! bare-metal mode, we'll just crash, but that's fine.

	st r0, _pi_kernel_parameters ! save return value
	mov r0, sr
	ld fp, .returnfp
	ld sp, .returnsp
	ld lr, .returnlr
	pop r0-r24
	ld r0, _pi_kernel_parameters ! restore return value
	b lr

! Define symbols at the beginning of our various segments, so that we can find
! them. (Except .text, which has already been done.)

.define begtext, begdata, begbss
.sect .data;       begdata:
.sect .rom;        begrom:
.sect .bss;        begbss:

! Some magic data. All EM systems need these.

.define .trppc, .ignmask, _errno
.comm .trppc, 4
.comm .ignmask, 4
.comm _errno, 4

! We store the stack pointer and return address on entry so that we can
! cleanly exit.

.comm .returnfp, 4
.comm .returnsp, 4
.comm .returnlr, 4

.define _pi_kernel_parameters
.comm _pi_kernel_parameters, 4

.define .linenumber, .filename
.comm .linenumber, 4         ! current linenumber (used for debugging)
.comm .filename, 4           ! ptr to current filename (used for debugging)

! User stack.

.comm .stack, STACKSIZE

