/*	$NetBSD: db_interface.c,v 1.33 2003/08/25 04:51:10 mrg Exp $	*/

/* 
 * Copyright (c) 1996 Scott K. Stevens
 *
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 *
 *	From: db_interface.c,v 2.4 1991/02/05 17:11:13 mrt (CMU)
 */

/*
 * Interface to new debugger.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");
#include "opt_ddb.h"

#include <sys/param.h>
#include <sys/proc.h>
#include <sys/reboot.h>
#include <sys/systm.h>	/* just for boothowto */
#include <sys/exec.h>

#include <vm/vm.h>
#include <vm/pmap.h>
#include <vm/vm_map.h>
#include <vm/vm_extern.h>

#include <machine/db_machdep.h>
#include <machine/katelib.h>
#include <machine/vmparam.h>
#include <machine/cpu.h>

#include <ddb/ddb.h>
#include <ddb/db_access.h>
#include <ddb/db_command.h>
#include <ddb/db_output.h>
#include <ddb/db_variables.h>
#include <ddb/db_sym.h>
#include <sys/cons.h>

static int nil = 0;

int db_access_und_sp (struct db_variable *, db_expr_t *, int);
int db_access_abt_sp (struct db_variable *, db_expr_t *, int);
int db_access_irq_sp (struct db_variable *, db_expr_t *, int);

#define DB_OFFSET(x)	(db_expr_t *)offsetof(struct trapframe, x)
struct db_variable db_regs[] = {
	{ "spsr", DB_OFFSET(tf_spsr),	FCN_NULL, },
	{ "r0", DB_OFFSET(tf_r0),	FCN_NULL, },
	{ "r1", DB_OFFSET(tf_r1),	FCN_NULL, },
	{ "r2", DB_OFFSET(tf_r2),	FCN_NULL, },
	{ "r3", DB_OFFSET(tf_r3),	FCN_NULL, },
	{ "r4", DB_OFFSET(tf_r4),	FCN_NULL, },
	{ "r5", DB_OFFSET(tf_r5),	FCN_NULL, },
	{ "r6", DB_OFFSET(tf_r6),	FCN_NULL, },
	{ "r7", DB_OFFSET(tf_r7),	FCN_NULL, },
	{ "r8", DB_OFFSET(tf_r8),	FCN_NULL, },
	{ "r9", DB_OFFSET(tf_r9),	FCN_NULL, },
	{ "r10", DB_OFFSET(tf_r10),	FCN_NULL, },
	{ "r11", DB_OFFSET(tf_r11),	FCN_NULL, },
	{ "r12", DB_OFFSET(tf_r12),	FCN_NULL, },
	{ "usr_sp", DB_OFFSET(tf_usr_sp), FCN_NULL, },
	{ "usr_lr", DB_OFFSET(tf_usr_lr), FCN_NULL, },
	{ "svc_sp", DB_OFFSET(tf_svc_sp), FCN_NULL, },
	{ "svc_lr", DB_OFFSET(tf_svc_lr), FCN_NULL, },
	{ "pc", DB_OFFSET(tf_pc), 	FCN_NULL, },
	{ "und_sp", &nil, db_access_und_sp, },
	{ "abt_sp", &nil, db_access_abt_sp, },
	{ "irq_sp", &nil, db_access_irq_sp, },
};

struct db_variable *db_eregs = db_regs + sizeof(db_regs)/sizeof(db_regs[0]);

int
db_access_und_sp(struct db_variable *vp, db_expr_t *valp, int rw)
{

	if (rw == DB_VAR_GET)
		*valp = get_stackptr(PSR_UND32_MODE);
	return(0);
}

int
db_access_abt_sp(struct db_variable *vp, db_expr_t *valp, int rw)
{

	if (rw == DB_VAR_GET)
		*valp = get_stackptr(PSR_ABT32_MODE);
	return(0);
}

int
db_access_irq_sp(struct db_variable *vp, db_expr_t *valp, int rw)
{

	if (rw == DB_VAR_GET)
		*valp = get_stackptr(PSR_IRQ32_MODE);
	return(0);
}

void
db_show_mdpcpu(struct pcpu *pc)
{
}
int
db_validate_address(vm_offset_t addr)
{
	struct proc *p = curproc;
	struct pmap *pmap;

	if (!p || !p->p_vmspace || !p->p_vmspace->vm_map.pmap ||
#ifndef ARM32_NEW_VM_LAYOUT
	    addr >= VM_MAXUSER_ADDRESS
#else
	    addr >= VM_MIN_KERNEL_ADDRESS
#endif
	   )
		pmap = pmap_kernel();
	else
		pmap = p->p_vmspace->vm_map.pmap;

	return (pmap_extract(pmap, addr) == FALSE);
}

/*
 * Read bytes from kernel address space for debugger.
 */
int
db_read_bytes(addr, size, data)
	vm_offset_t	addr;
	size_t	size;
	char	*data;
{
	char	*src = (char *)addr;

	if (db_validate_address((u_int)src)) {
		db_printf("address %p is invalid\n", src);
		return (-1);
	}

	if (size == 4 && (addr & 3) == 0 && ((uintptr_t)data & 3) == 0) {
		*((int*)data) = *((int*)src);
		return (0);
	}

	if (size == 2 && (addr & 1) == 0 && ((uintptr_t)data & 1) == 0) {
		*((short*)data) = *((short*)src);
		return (0);
	}

	while (size-- > 0) {
		if (db_validate_address((u_int)src)) {
			db_printf("address %p is invalid\n", src);
			return (-1);
		}
		*data++ = *src++;
	}
	return (0);
}

/*
 * Write bytes to kernel address space for debugger.
 */
int
db_write_bytes(vm_offset_t addr, size_t size, char *data)
{
	char *dst;
	size_t loop;

	/* If any part is in kernel text, use db_write_text() */
	if (addr >= (vm_offset_t) btext && addr < (vm_offset_t) etext) {
		return (-1);
	}

	dst = (char *)addr;
	if (db_validate_address((u_int)dst)) {
		db_printf("address %p is invalid\n", dst);
		return (0);
	}

	if (size == 4 && (addr & 3) == 0 && ((uintptr_t)data & 3) == 0)
		*((int*)dst) = *((int*)data);
	else
	if (size == 2 && (addr & 1) == 0 && ((uintptr_t)data & 1) == 0)
		*((short*)dst) = *((short*)data);
	else {
		loop = size;
		while (loop-- > 0) {
			if (db_validate_address((u_int)dst)) {
				db_printf("address %p is invalid\n", dst);
				return (-1);
			}
			*dst++ = *data++;
		}
	}

	/* make sure the caches and memory are in sync */
	cpu_icache_sync_range(addr, size);

	/* In case the current page tables have been modified ... */
	cpu_tlb_flushID();
	cpu_cpwait();
	return (0);
}



