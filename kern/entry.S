#include <asm/asm.h>
#include <stackframe.h>

.data
cp0_trapframe:
.space 12

.section .text.tlb_miss_entry
tlb_miss_entry:
.set noat
.set noreorder
	la      k1, cp0_trapframe
	mfc0    k0, CP0_EPC
	sw      k0, 0(k1)
	mfc0    k0, CP0_BADVADDR
	sw      k0, 4(k1)
	mfc0    k0, CP0_ENTRYHI
	sw      k0, 8(k1)
	mfc0    k1, CP0_CONTEXT
	lw      k1, 0(k1)
	la      k0, cp0_trapframe
	lw      k0, 8(k0)
	nop
	mtc0    k0, CP0_ENTRYHI
	beqz    k1, 1f
	mtc0    k1, CP0_ENTRYLO0
	lw      k0, cp0_trapframe
	tlbwr
	jr      k0
	rfe
1:
	la      k1, cp0_trapframe
	lw      k0, 0(k1)
	nop
	mtc0    k0, CP0_EPC
	lw      k0, 4(k1)
	nop
	mtc0    k0, CP0_BADVADDR
.set reorder
.set at

.section .text.exc_gen_entry
exc_gen_entry:
	SAVE_ALL
/* Exercise 3.9: Your code here. */
	mfc0    t0, CP0_CAUSE
	andi    t0, 0x7c
	lw      t0, exception_handlers(t0)
	jr      t0
