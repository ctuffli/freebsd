/*-
 * Copyright (C) 2011 MARVELL INTERNATIONAL LTD.
 * All rights reserved.
 *
 * Developed by Semihalf.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of MARVELL nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <machine/asm.h>
__FBSDID("$FreeBSD$");

#include <machine/param.h>

.Lpj4b_cache_line_size:
	.word	_C_LABEL(arm_pdcache_line_size)

ENTRY(pj4b_setttb)
	/* Cache synchronization is not required as this core has PIPT caches */
	mcr	p15, 0, r1, c7, c10, 4	/* drain the write buffer */
#ifdef SMP
	orr 	r0, r0, #2		/* Set TTB shared memory flag */
#endif
	mcr	p15, 0, r0, c2, c0, 0	/* load new TTB */
	mcr	p15, 0, r0, c8, c7, 0	/* invalidate I+D TLBs */
	RET

ENTRY_NP(armv6_icache_sync_all)
	/*
	 * We assume that the code here can never be out of sync with the
	 * dcache, so that we can safely flush the Icache and fall through
	 * into the Dcache cleaning code.
	 */
	mov	r0, #0
	mcr	p15, 0, r0, c7, c5, 0	/* Invalidate ICache */
	mcr	p15, 0, r0, c7, c10, 0	/* Clean (don't invalidate) DCache */
	mcr	p15, 0, r0, c7, c10, 4	/* drain the write buffer */
	RET

ENTRY(pj4b_icache_sync_range)
	sub	r1, r1, #1
	add	r1, r0, r1
	mcrr	p15, 0, r1, r0, c5	/* invalidate IC range */
	mcrr	p15, 0, r1, r0, c12	/* clean DC range */
	mcr	p15, 0, r0, c7, c10, 4	/* drain the write buffer */
	RET

ENTRY(pj4b_dcache_inv_range)
	ldr	ip, .Lpj4b_cache_line_size
	ldr	ip, [ip]
	sub	r1, r1, #1		/* Don't overrun */
	sub	r3, ip, #1
	and	r2, r0, r3
	add	r1, r1, r2
	bic	r0, r0, r3

	mcr	p15, 0, r0, c7, c10, 5  /* Data Memory Barrier err:4413 */
1:
	mcr	p15, 0, r0, c7, c6, 1
	add	r0, r0, ip
	subs	r1, r1, ip
	bpl	1b
	mcr	p15, 0, r0, c7, c10, 4	/* drain the write buffer */
	RET

ENTRY(armv6_idcache_wbinv_all)
	mov	r0, #0
	mcr	p15, 0, r0, c7, c5, 0	/* invalidate ICache */
	mcr	p15, 0, r0, c7, c14, 0	/* clean and invalidate DCache */
	mcr	p15, 0, r0, c7, c10, 4	/* drain the write buffer */
	RET

ENTRY(armv6_dcache_wbinv_all)
	mov	r0, #0
	mcr	p15, 0, r0, c7, c14, 0	/* clean and invalidate DCache */
	mcr	p15, 0, r0, c7, c10, 4	/* drain the write buffer */
	RET

ENTRY(pj4b_idcache_wbinv_range)
	ldr	ip, .Lpj4b_cache_line_size
	ldr	ip, [ip]
	sub	r1, r1, #1		/* Don't overrun */
	sub	r3, ip, #1
	and	r2, r0, r3
	add	r1, r1, r2
	bic	r0, r0, r3

	mcr	p15, 0, r0, c7, c10, 5  /* Data Memory Barrier err:4611 */
1:
#ifdef SMP
	/* Request for ownership */
	ldr	r2, [r0]
	str	r2, [r0]
#endif
	mcr	p15, 0, r0, c7, c5, 1
	mcr	p15, 0, r0, c7, c14, 1	/* L2C clean and invalidate entry */
	add	r0, r0, ip
	subs	r1, r1, ip
	bpl	1b
	mcr	p15, 0, r0, c7, c10, 4	/* drain the write buffer */
	RET

ENTRY(pj4b_dcache_wbinv_range)
	ldr	ip, .Lpj4b_cache_line_size
	ldr	ip, [ip]
	sub	r1, r1, #1		/* Don't overrun */
	sub	r3, ip, #1
	and	r2, r0, r3
	add	r1, r1, r2
	bic	r0, r0, r3

	mcr	p15, 0, r0, c7, c10, 5  /* Data Memory Barrier err:4611 */
1:
#ifdef SMP
	/* Request for ownership */
	ldr	r2, [r0]
	str	r2, [r0]
#endif
	mcr	p15, 0, r0, c7, c14, 1
	add	r0, r0, ip
	subs	r1, r1, ip
	bpl	1b
	mcr	p15, 0, r0, c7, c10, 4	/* drain the write buffer */
	RET

ENTRY(pj4b_dcache_wb_range)
	ldr	ip, .Lpj4b_cache_line_size
	ldr	ip, [ip]
	sub	r1, r1, #1		/* Don't overrun */
	sub	r3, ip, #1
	and	r2, r0, r3
	add	r1, r1, r2
	bic	r0, r0, r3

	mcr	p15, 0, r0, c7, c10, 5  /* Data Memory Barrier err:4611 */
1:
#ifdef SMP
	/* Request for ownership */
	ldr	r2, [r0]
	str	r2, [r0]
#endif
	mcr	p15, 0, r0, c7, c10, 1	/* L2C clean single entry by MVA */
	add	r0, r0, ip
	subs	r1, r1, ip
	bpl	1b
	mcr	p15, 0, r0, c7, c10, 4	/* drain the write buffer */
	RET

ENTRY(pj4b_drain_readbuf)
	mcr	p15, 0, r0, c7, c5, 4	/* flush prefetch buffers */
	RET

ENTRY(pj4b_flush_brnchtgt_all)
	mcr	p15, 0, r0, c7, c5, 6	/* flush entrie branch target cache */
	RET

ENTRY(pj4b_flush_brnchtgt_va)
	mcr	p15, 0, r0, c7, c5, 7	/* flush branch target cache by VA */
	RET

ENTRY(get_core_id)
	mrc p15, 0, r0, c0, c0, 5
	RET

ENTRY(pj4b_config)
	/* Set Auxiliary Debug Modes Control 2 register */
	mrc	p15, 1, r0, c15, c1, 2
	bic	r0, r0, #(1 << 23)
	orr	r0, r0, #(1 << 25)
	orr	r0, r0, #(1 << 27)
	orr	r0, r0, #(1 << 29)
	orr	r0, r0, #(1 << 30)
	mcr	p15, 1, r0, c15, c1, 2
#if defined(SMP)
	/* Set SMP mode in Auxiliary Control Register */
	mrc	p15, 0, r0, c1, c0, 1
	orr	r0, r0, #(1 << 5)
	mcr	p15, 0, r0, c1, c0, 1
#endif
	RET
