#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_ov(void);
extern void handle_reserved(void);

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
#endif
    [12] = handle_ov,
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}

// For lab3 extra
void do_ov(struct Trapframe *tf) {
	curenv->env_ov_cnt++;
	u_int epc = tf->cp0_epc;
	struct Page *pp = page_lookup(curenv->env_pgdir, epc, NULL);
	u_int addr = page2kva(pp) | (epc ^ PTE_ADDR(epc));
	u_int code = *((u_int *)addr);
	
	if (code & 0x20000000) {
		// handle addi
		u_int s = (code >> 21) & 0x1f;
		u_int t = (code >> 16) & 0x1f;
		u_int imm = code & 0xffff;
		tf->regs[t] = (tf->regs[s] >> 1) + (imm >> 1);
		tf->cp0_epc += 4;
		printk("addi ov handled\n");
	} else {
		*((u_int *)addr) |= 0x1;
		if (code & 0x2) {
			// handle sub
			printk("sub ov handled\n");
		} else {
			// handle add
			printk("add ov handled\n");
		}
	}
}
