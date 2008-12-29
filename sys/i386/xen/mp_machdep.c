/*-
 * Copyright (c) 1996, by Steve Passe
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the developer may NOT be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include "opt_apic.h"
#include "opt_cpu.h"
#include "opt_kdb.h"
#include "opt_kstack_pages.h"
#include "opt_mp_watchdog.h"
#include "opt_sched.h"

#if !defined(lint)
#if !defined(SMP)
#error How did you get here?
#endif

#ifndef DEV_APIC
#error The apic device is required for SMP, add "device apic" to your config file.
#endif
#if defined(CPU_DISABLE_CMPXCHG) && !defined(COMPILING_LINT)
#error SMP not supported with CPU_DISABLE_CMPXCHG
#endif
#endif /* not lint */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/cons.h>	/* cngetc() */
#ifdef GPROF 
#include <sys/gmon.h>
#endif
#include <sys/kernel.h>
#include <sys/ktr.h>
#include <sys/lock.h>
#include <sys/malloc.h>
#include <sys/memrange.h>
#include <sys/mutex.h>
#include <sys/pcpu.h>
#include <sys/proc.h>
#include <sys/smp.h>
#include <sys/sysctl.h>

#include <vm/vm.h>
#include <vm/vm_param.h>
#include <vm/pmap.h>
#include <vm/vm_kern.h>
#include <vm/vm_extern.h>
#include <vm/vm_page.h>

#include <machine/apicreg.h>
#include <machine/clock.h>
#include <machine/md_var.h>
#include <machine/mp_watchdog.h>
#include <machine/pcb.h>
#include <machine/pcpu.h>
#include <machine/psl.h>
#include <machine/smp.h>
#include <machine/smptests.h>	/** COUNT_XINVLTLB_HITS */
#include <machine/specialreg.h>

#include <xen/hypervisor.h>
#include <xen/xen_intr.h>
#include <xen/evtchn.h>
#include <xen/xen_intr.h>
#include <xen/hypervisor.h>
#include <xen/interface/vcpu.h>

/*
 * this code MUST be enabled here and in mpboot.s.
 * it follows the very early stages of AP boot by placing values in CMOS ram.
 * it NORMALLY will never be needed and thus the primitive method for enabling.
 *
#define CHECK_POINTS
 */

/* lock region used by kernel profiling */
int	mcount_lock;

/** XXX FIXME: where does this really belong, isa.h/isa.c perhaps? */
int	current_postcode;

int	mp_naps;		/* # of Applications processors */
int	boot_cpu_id = -1;	/* designated BSP */
extern	int nkpt;

extern	struct pcpu __pcpu[];

/*
 * CPU topology map datastructures for HTT.
 */
static struct cpu_group mp_groups[MAXCPU];
static struct cpu_top mp_top;

/* AP uses this during bootstrap.  Do not staticize.  */
char *bootSTK;
static int bootAP;
static union descriptor *bootAPgdt;

static char resched_name[NR_CPUS][15];
static char callfunc_name[NR_CPUS][15];

/* Free these after use */
void *bootstacks[MAXCPU];

/* Hotwire a 0->4MB V==P mapping */
extern pt_entry_t *KPTphys;

struct pcb stoppcbs[MAXCPU];

/* Variables needed for SMP tlb shootdown. */
vm_offset_t smp_tlb_addr1;
vm_offset_t smp_tlb_addr2;
volatile int smp_tlb_wait;

typedef void call_data_func_t(uintptr_t , uintptr_t);


#ifdef COUNT_IPIS
/* Interrupt counts. */
#ifdef IPI_PREEMPTION
static u_long *ipi_preempt_counts[MAXCPU];
#endif
static u_long *ipi_ast_counts[MAXCPU];
u_long *ipi_invltlb_counts[MAXCPU];
u_long *ipi_invlrng_counts[MAXCPU];
u_long *ipi_invlpg_counts[MAXCPU];
u_long *ipi_invlcache_counts[MAXCPU];
u_long *ipi_rendezvous_counts[MAXCPU];
u_long *ipi_lazypmap_counts[MAXCPU];
#endif

/*
 * Local data and functions.
 */

static u_int logical_cpus;

/* used to hold the AP's until we are ready to release them */
static struct mtx ap_boot_mtx;

/* Set to 1 once we're ready to let the APs out of the pen. */
static volatile int aps_ready = 0;

/*
 * Store data from cpu_add() until later in the boot when we actually setup
 * the APs.
 */
struct cpu_info {
	int	cpu_present:1;
	int	cpu_bsp:1;
	int	cpu_disabled:1;
} static cpu_info[MAX_APIC_ID + 1];
static int cpu_apic_ids[MAXCPU];

/* Holds pending bitmap based IPIs per CPU */
static volatile u_int cpu_ipi_pending[MAXCPU];

static void	assign_cpu_ids(void);
static void	set_interrupt_apic_ids(void);
static int	start_all_aps(void);
static int	start_ap(int apic_id);
static void	release_aps(void *dummy);

static u_int	hyperthreading_cpus;
static cpumask_t	hyperthreading_cpus_mask;
extern void Xhypervisor_callback(void);
extern void failsafe_callback(void);
extern void pmap_lazyfix_action(void);

void
mp_topology(void)
{
	struct cpu_group *group;
	int logical_cpus;
	int apic_id;
	int groups;
	int cpu;

	/* Build the smp_topology map. */
	/* Nothing to do if there is no HTT support. */
	if ((cpu_feature & CPUID_HTT) == 0)
		return;
	logical_cpus = (cpu_procinfo & CPUID_HTT_CORES) >> 16;
	if (logical_cpus <= 1)
		return;
	group = &mp_groups[0];
	groups = 1;
	for (cpu = 0, apic_id = 0; apic_id <= MAX_APIC_ID; apic_id++) {
		if (!cpu_info[apic_id].cpu_present)
			continue;
		/*
		 * If the current group has members and we're not a logical
		 * cpu, create a new group.
		 */
		if (group->cg_count != 0 && (apic_id % logical_cpus) == 0) {
			group++;
			groups++;
		}
		group->cg_count++;
		group->cg_mask |= 1 << cpu;
		cpu++;
	}

	mp_top.ct_count = groups;
	mp_top.ct_group = mp_groups;
	smp_topology = &mp_top;
}


/*
 * Calculate usable address in base memory for AP trampoline code.
 */
u_int
mp_bootaddress(u_int basemem)
{

	return (basemem);
}

void
cpu_add(u_int apic_id, char boot_cpu)
{

	if (apic_id > MAX_APIC_ID) {
		panic("SMP: APIC ID %d too high", apic_id);
		return;
	}
	KASSERT(cpu_info[apic_id].cpu_present == 0, ("CPU %d added twice",
	    apic_id));
	cpu_info[apic_id].cpu_present = 1;
	if (boot_cpu) {
		KASSERT(boot_cpu_id == -1,
		    ("CPU %d claims to be BSP, but CPU %d already is", apic_id,
		    boot_cpu_id));
		boot_cpu_id = apic_id;
		cpu_info[apic_id].cpu_bsp = 1;
	}
	if (mp_ncpus < MAXCPU)
		mp_ncpus++;
	if (bootverbose)
		printf("SMP: Added CPU %d (%s)\n", apic_id, boot_cpu ? "BSP" :
		    "AP");
}

void
cpu_mp_setmaxid(void)
{

	mp_maxid = MAXCPU - 1;
}

int
cpu_mp_probe(void)
{

	/*
	 * Always record BSP in CPU map so that the mbuf init code works
	 * correctly.
	 */
	all_cpus = 1;
	if (mp_ncpus == 0) {
		/*
		 * No CPUs were found, so this must be a UP system.  Setup
		 * the variables to represent a system with a single CPU
		 * with an id of 0.
		 */
		mp_ncpus = 1;
		return (0);
	}

	/* At least one CPU was found. */
	if (mp_ncpus == 1) {
		/*
		 * One CPU was found, so this must be a UP system with
		 * an I/O APIC.
		 */
		return (0);
	}

	/* At least two CPUs were found. */
	return (1);
}

/*
 * Initialize the IPI handlers and start up the AP's.
 */
void
cpu_mp_start(void)
{
	int i;

	/* Initialize the logical ID to APIC ID table. */
	for (i = 0; i < MAXCPU; i++) {
		cpu_apic_ids[i] = -1;
		cpu_ipi_pending[i] = 0;
	}

	/* Set boot_cpu_id if needed. */
	if (boot_cpu_id == -1) {
		boot_cpu_id = PCPU_GET(apic_id);
		cpu_info[boot_cpu_id].cpu_bsp = 1;
	} else
		KASSERT(boot_cpu_id == PCPU_GET(apic_id),
		    ("BSP's APIC ID doesn't match boot_cpu_id"));
	cpu_apic_ids[0] = boot_cpu_id;

	assign_cpu_ids();

	/* Start each Application Processor */
	start_all_aps();

	/* Setup the initial logical CPUs info. */
	logical_cpus = logical_cpus_mask = 0;
	if (cpu_feature & CPUID_HTT)
		logical_cpus = (cpu_procinfo & CPUID_HTT_CORES) >> 16;

	set_interrupt_apic_ids();
}


static void
iv_rendezvous(uintptr_t a, uintptr_t b)
{
	smp_rendezvous_action();
}

static void
iv_invltlb(uintptr_t a, uintptr_t b)
{
	xen_tlb_flush();
}

static void
iv_invlpg(uintptr_t a, uintptr_t b)
{
	xen_invlpg(a);
}

static void
iv_invlrng(uintptr_t a, uintptr_t b)
{
	vm_offset_t start = (vm_offset_t)a;
	vm_offset_t end = (vm_offset_t)b;

	while (start < end) {
		xen_invlpg(start);
		start += PAGE_SIZE;
	}
}


static void
iv_invlcache(uintptr_t a, uintptr_t b)
{

	wbinvd();
}

static void
iv_lazypmap(uintptr_t a, uintptr_t b)
{
	pmap_lazyfix_action();
}


static void
iv_noop(uintptr_t a, uintptr_t b)
{
}

static call_data_func_t *ipi_vectors[IPI_BITMAP_VECTOR] = 
{
  iv_noop,
  iv_noop,
  iv_rendezvous,
  iv_invltlb,
  iv_invlpg,
  iv_invlrng,
  iv_invlcache,
  iv_lazypmap,
};

/*
 * Reschedule call back. Nothing to do,
 * all the work is done automatically when
 * we return from the interrupt.
 */
static void
smp_reschedule_interrupt(void *unused)
{
	int cpu = PCPU_GET(cpuid);
	u_int ipi_bitmap;

	ipi_bitmap = atomic_readandclear_int(&cpu_ipi_pending[cpu]);

#ifdef IPI_PREEMPTION
	if (ipi_bitmap & (1 << IPI_PREEMPT)) {
#ifdef COUNT_IPIS
		*ipi_preempt_counts[cpu]++;
#endif
		mtx_lock_spin(&sched_lock);
		/* Don't preempt the idle thread */
		if (curthread != PCPU_GET(idlethread)) {
			struct thread *running_thread = curthread;
			if (running_thread->td_critnest > 1) 
				running_thread->td_owepreempt = 1;
			else 		
				mi_switch(SW_INVOL | SW_PREEMPT, NULL);
		}
		mtx_unlock_spin(&sched_lock);
	}
#endif

	if (ipi_bitmap & (1 << IPI_AST)) {
#ifdef COUNT_IPIS
		*ipi_ast_counts[cpu]++;
#endif
		/* Nothing to do for AST */
	}
}

struct _call_data {
	uint16_t func_id;
	uint16_t wait;
	uintptr_t arg1;
	uintptr_t arg2;
	atomic_t started;
	atomic_t finished;
};

static struct _call_data *call_data;

static void
smp_call_function_interrupt(void *arg)
{	
	call_data_func_t *func;
	uintptr_t arg1 = call_data->arg1;
	uintptr_t arg2 = call_data->arg2;
	int wait = call_data->wait;
	atomic_t *started = &call_data->started;
	atomic_t *finished = &call_data->finished;

	if (call_data->func_id > IPI_BITMAP_VECTOR)
		panic("invalid function id %u", call_data->func_id);
	
	func = ipi_vectors[call_data->func_id];
	/*
	 * Notify initiating CPU that I've grabbed the data and am
	 * about to execute the function
	 */
	mb();
	atomic_inc(started);
	/*
	 * At this point the info structure may be out of scope unless wait==1
	 */
	(*func)(arg1, arg2);

	if (wait) {
		mb();
		atomic_inc(finished);
	}
	atomic_add_int(&smp_tlb_wait, 1);
}

/*
 * Print various information about the SMP system hardware and setup.
 */
void
cpu_mp_announce(void)
{
	int i, x;

	/* List CPUs */
	printf(" cpu0 (BSP): APIC ID: %2d\n", boot_cpu_id);
	for (i = 1, x = 0; x <= MAX_APIC_ID; x++) {
		if (!cpu_info[x].cpu_present || cpu_info[x].cpu_bsp)
			continue;
		if (cpu_info[x].cpu_disabled)
			printf("  cpu (AP): APIC ID: %2d (disabled)\n", x);
		else {
			KASSERT(i < mp_ncpus,
			    ("mp_ncpus and actual cpus are out of whack"));
			printf(" cpu%d (AP): APIC ID: %2d\n", i++, x);
		}
	}
}

static int
xen_smp_intr_init(unsigned int cpu)
{
	int rc;
	unsigned int irq;
	
	per_cpu(resched_irq, cpu) = per_cpu(callfunc_irq, cpu) = -1;

	sprintf(resched_name[cpu], "resched%u", cpu);
	rc = bind_ipi_to_irqhandler(RESCHEDULE_VECTOR,
				    cpu,
				    resched_name[cpu],
				    smp_reschedule_interrupt,
	    INTR_FAST|INTR_TYPE_TTY|INTR_MPSAFE, &irq);

	printf("cpu=%d irq=%d vector=%d\n",
	    cpu, rc, RESCHEDULE_VECTOR);
	
	per_cpu(resched_irq, cpu) = irq;

	sprintf(callfunc_name[cpu], "callfunc%u", cpu);
	rc = bind_ipi_to_irqhandler(CALL_FUNCTION_VECTOR,
				    cpu,
				    callfunc_name[cpu],
				    smp_call_function_interrupt,
	    INTR_FAST|INTR_TYPE_TTY|INTR_MPSAFE, &irq);
	if (rc < 0)
		goto fail;
	per_cpu(callfunc_irq, cpu) = irq;

	printf("cpu=%d irq=%d vector=%d\n",
	    cpu, rc, CALL_FUNCTION_VECTOR);

	
	if ((cpu != 0) && ((rc = ap_cpu_initclocks(cpu)) != 0))
		goto fail;

	return 0;

 fail:
	if (per_cpu(resched_irq, cpu) >= 0)
		unbind_from_irqhandler(per_cpu(resched_irq, cpu));
	if (per_cpu(callfunc_irq, cpu) >= 0)
		unbind_from_irqhandler(per_cpu(callfunc_irq, cpu));
	return rc;
}

static void
xen_smp_intr_init_cpus(void *unused)
{
	int i;
	    
	for (i = 0; i < mp_ncpus; i++)
		xen_smp_intr_init(i);
}

#define MTOPSIZE (1<<(14 + PAGE_SHIFT))
/*
 * AP CPU's call this to initialize themselves.
 */
void
init_secondary(void)
{
	vm_offset_t addr;
	int	gsel_tss;

	/* bootAP is set in start_ap() to our ID. */
	PCPU_SET(currentldt, _default_ldt);

	gsel_tss = GSEL(GPROC0_SEL, SEL_KPL);
#if 0	
	gdt[myid * NGDT + GPROC0_SEL].sd.sd_type = SDT_SYS386TSS;
#endif	
	PCPU_SET(common_tss.tss_esp0, 0); /* not used until after switch */
	PCPU_SET(common_tss.tss_ss0, GSEL(GDATA_SEL, SEL_KPL));
	PCPU_SET(common_tss.tss_ioopt, (sizeof (struct i386tss)) << 16);
#if 0
	PCPU_SET(tss_gdt, &gdt[myid * NGDT + GPROC0_SEL].sd);
	PCPU_SET(common_tssd, *PCPU_GET(tss_gdt));
	ltr(gsel_tss);
#endif
	PCPU_SET(fsgs_gdt, &gdt[GUFS_SEL].sd);

	/* signal our startup to the BSP. */
	mp_naps++;

	/* Spin until the BSP releases the AP's. */
	while (!aps_ready)
		ia32_pause();

	/* BSP may have changed PTD while we were waiting */
	invltlb();
	for (addr = 0; addr < NKPT * NBPDR - 1; addr += PAGE_SIZE)
		invlpg(addr);

	/* set up FPU state on the AP */
	npxinit(__INITIAL_NPXCW__);

#if 0	
	/* set up SSE registers */
	enable_sse();

	/* A quick check from sanity claus */
	if (PCPU_GET(apic_id) != lapic_id()) {
		printf("SMP: cpuid = %d\n", PCPU_GET(cpuid));
		printf("SMP: actual apic_id = %d\n", lapic_id());
		printf("SMP: correct apic_id = %d\n", PCPU_GET(apic_id));
		panic("cpuid mismatch! boom!!");
	}
#endif
	/* Initialize curthread. */
	KASSERT(PCPU_GET(idlethread) != NULL, ("no idle thread"));
	PCPU_SET(curthread, PCPU_GET(idlethread));

	mtx_lock_spin(&ap_boot_mtx);
#if 0
	/* Init local apic for irq's */
	lapic_setup(1);

	/* Set memory range attributes for this CPU to match the BSP */
	mem_range_AP_init();
#endif
	smp_cpus++;

	CTR1(KTR_SMP, "SMP: AP CPU #%d Launched", PCPU_GET(cpuid));
	printf("SMP: AP CPU #%d Launched!\n", PCPU_GET(cpuid));

	/* Determine if we are a logical CPU. */
	if (logical_cpus > 1 && PCPU_GET(apic_id) % logical_cpus != 0)
		logical_cpus_mask |= PCPU_GET(cpumask);
	
	/* Determine if we are a hyperthread. */
	if (hyperthreading_cpus > 1 &&
	    PCPU_GET(apic_id) % hyperthreading_cpus != 0)
		hyperthreading_cpus_mask |= PCPU_GET(cpumask);

	/* Build our map of 'other' CPUs. */
	PCPU_SET(other_cpus, all_cpus & ~PCPU_GET(cpumask));
#if 0
	if (bootverbose)
		lapic_dump("AP");
#endif
	if (smp_cpus == mp_ncpus) {
		/* enable IPI's, tlb shootdown, freezes etc */
		atomic_store_rel_int(&smp_started, 1);
		smp_active = 1;	 /* historic */
	}

	mtx_unlock_spin(&ap_boot_mtx);

	/* wait until all the AP's are up */
	while (smp_started == 0)
		ia32_pause();

	/* ok, now grab sched_lock and enter the scheduler */
	mtx_lock_spin(&sched_lock);

	/*
	 * Correct spinlock nesting.  The idle thread context that we are
	 * borrowing was created so that it would start out with a single
	 * spin lock (sched_lock) held in fork_trampoline().  Since we've
	 * explicitly acquired locks in this function, the nesting count
	 * is now 2 rather than 1.  Since we are nested, calling
	 * spinlock_exit() will simply adjust the counts without allowing
	 * spin lock using code to interrupt us.
	 */
	spinlock_exit();
	KASSERT(curthread->td_md.md_spinlock_count == 1, ("invalid count"));

	binuptime(PCPU_PTR(switchtime));
	PCPU_SET(switchticks, ticks);

	cpu_throw(NULL, choosethread());	/* doesn't return */

	panic("scheduler returned us to %s", __func__);
	/* NOTREACHED */
}

/*******************************************************************
 * local functions and data
 */

/*
 * We tell the I/O APIC code about all the CPUs we want to receive
 * interrupts.  If we don't want certain CPUs to receive IRQs we
 * can simply not tell the I/O APIC code about them in this function.
 * We also do not tell it about the BSP since it tells itself about
 * the BSP internally to work with UP kernels and on UP machines.
 */
static void
set_interrupt_apic_ids(void)
{
	u_int apic_id;

	for (apic_id = 0; apic_id < MAXCPU; apic_id++) {
		if (!cpu_info[apic_id].cpu_present)
			continue;
		if (cpu_info[apic_id].cpu_bsp)
			continue;
		if (cpu_info[apic_id].cpu_disabled)
			continue;

		/* Don't let hyperthreads service interrupts. */
		if (hyperthreading_cpus > 1 &&
		    apic_id % hyperthreading_cpus != 0)
			continue;

		intr_add_cpu(apic_id);
	}
}

/*
 * Assign logical CPU IDs to local APICs.
 */
static void
assign_cpu_ids(void)
{
	u_int i;

	/* Check for explicitly disabled CPUs. */
	for (i = 0; i <= MAX_APIC_ID; i++) {
		if (!cpu_info[i].cpu_present || cpu_info[i].cpu_bsp)
			continue;

		/* Don't use this CPU if it has been disabled by a tunable. */
		if (resource_disabled("lapic", i)) {
			cpu_info[i].cpu_disabled = 1;
			continue;
		}
	}

	/*
	 * Assign CPU IDs to local APIC IDs and disable any CPUs
	 * beyond MAXCPU.  CPU 0 has already been assigned to the BSP,
	 * so we only have to assign IDs for APs.
	 */
	mp_ncpus = 1;
	for (i = 0; i <= MAX_APIC_ID; i++) {
		if (!cpu_info[i].cpu_present || cpu_info[i].cpu_bsp ||
		    cpu_info[i].cpu_disabled)
			continue;

		if (mp_ncpus < MAXCPU) {
			cpu_apic_ids[mp_ncpus] = i;
			mp_ncpus++;
		} else
			cpu_info[i].cpu_disabled = 1;
	}
	KASSERT(mp_maxid >= mp_ncpus - 1,
	    ("%s: counters out of sync: max %d, count %d", __func__, mp_maxid,
	    mp_ncpus));		
}

/*
 * start each AP in our list
 */
static int
start_all_aps(void)
{
	int apic_id, cpu, i;
	struct pcpu *pc;
	
	mtx_init(&ap_boot_mtx, "ap boot", NULL, MTX_SPIN);

	/* start each AP */
	for (cpu = 1; cpu < mp_ncpus; cpu++) {
		apic_id = cpu_apic_ids[cpu];

		bootstacks[cpu] = (char *)kmem_alloc(kernel_map, KSTACK_PAGES * PAGE_SIZE);

		bootSTK = (char *)bootstacks[cpu] + KSTACK_PAGES * PAGE_SIZE - 4;
		bootAP = cpu;
		bootAPgdt = gdt + (512*cpu);

		/* Get per-cpu data */
		pc = &__pcpu[bootAP];
		pcpu_init(pc, bootAP, sizeof(struct pcpu));
		pc->pc_apic_id = cpu_apic_ids[bootAP];
		pc->pc_prvspace = pc;
		pc->pc_curthread = 0;

		gdt_segs[GPRIV_SEL].ssd_base = (int) pc;
		gdt_segs[GPROC0_SEL].ssd_base = (int) &pc->pc_common_tss;
		
		PT_SET_MA(bootAPgdt, xpmap_ptom(VTOP(bootAPgdt)) | PG_V | PG_RW);
		bzero(bootAPgdt, PAGE_SIZE);
		for (i = 0; i < NGDT; i++)
			ssdtosd(&gdt_segs[i], &bootAPgdt[i].sd);
		PT_SET_MA(bootAPgdt, vtomach(bootAPgdt) | PG_V);
#ifdef notyet
		
                if (HYPERVISOR_vcpu_op(VCPUOP_get_physid, cpu, &cpu_id) == 0) { 
                        apicid = xen_vcpu_physid_to_x86_apicid(cpu_id.phys_id); 
                        acpiid = xen_vcpu_physid_to_x86_acpiid(cpu_id.phys_id); 
#ifdef CONFIG_ACPI 
                        if (acpiid != 0xff) 
                                x86_acpiid_to_apicid[acpiid] = apicid; 
#endif 
                } 
#endif

		/* attempt to start the Application Processor */
		if (!start_ap(apic_id)) {
			printf("AP #%d (PHY# %d) failed!\n", cpu, apic_id);
			/* better panic as the AP may be running loose */
			printf("panic y/n? [y] ");
			if (cngetc() != 'n')
				panic("bye-bye");
		}

		all_cpus |= (1 << cpu);		/* record AP in CPU map */
	}

	/* build our map of 'other' CPUs */
	PCPU_SET(other_cpus, all_cpus & ~PCPU_GET(cpumask));

	pmap_invalidate_range(kernel_pmap, 0, NKPT * NBPDR - 1);

	/* number of APs actually started */
	return mp_naps;
}

extern uint8_t *pcpu_boot_stack;
extern trap_info_t trap_table[];

static void
smp_trap_init(trap_info_t *trap_ctxt)
{
        const trap_info_t *t = trap_table;

        for (t = trap_table; t->address; t++) {
                trap_ctxt[t->vector].flags = t->flags;
                trap_ctxt[t->vector].cs = t->cs;
                trap_ctxt[t->vector].address = t->address;
        }
}

void cpu_initialize_context(unsigned int cpu);

void
cpu_initialize_context(unsigned int cpu)
{
	/* vcpu_guest_context_t is too large to allocate on the stack.
	 * Hence we allocate statically and protect it with a lock */
	vm_page_t m[4];
	static vcpu_guest_context_t ctxt;
	vm_offset_t boot_stack;
	vm_offset_t newPTD;
	vm_paddr_t ma[NPGPTD];
	static int color;
	int i;

	/*
	 * Page 0,[0-3]	PTD
	 * Page 1, [4]	boot stack
	 * Page [5]	PDPT
	 *
	 */
	for (i = 0; i < NPGPTD + 2; i++) {
		m[i] = vm_page_alloc(NULL, color++,
		    VM_ALLOC_NORMAL | VM_ALLOC_NOOBJ | VM_ALLOC_WIRED |
		    VM_ALLOC_ZERO);

		pmap_zero_page(m[i]);

	}
	boot_stack = kmem_alloc_nofault(kernel_map, 1);
	newPTD = kmem_alloc_nofault(kernel_map, NPGPTD);
	ma[0] = xpmap_ptom(VM_PAGE_TO_PHYS(m[0]))|PG_V;

#ifdef PAE	
	pmap_kenter(boot_stack, VM_PAGE_TO_PHYS(m[NPGPTD + 1]));
	for (i = 0; i < NPGPTD; i++) {
		((vm_paddr_t *)boot_stack)[i] =
		ma[i] = 
		    xpmap_ptom(VM_PAGE_TO_PHYS(m[i]))|PG_V;
	}
#endif	

	/*
	 * Copy cpu0 IdlePTD to new IdlePTD - copying only
	 * kernel mappings
	 */
	pmap_qenter(newPTD, m, 4);
	
	memcpy((uint8_t *)newPTD + KPTDI*sizeof(vm_paddr_t),
	    (uint8_t *)PTOV(IdlePTD) + KPTDI*sizeof(vm_paddr_t),
	    nkpt*sizeof(vm_paddr_t));

	pmap_qremove(newPTD, 4);
	kmem_free(kernel_map, newPTD, 4);
	/*
	 * map actual idle stack to boot_stack
	 */
	pmap_kenter(boot_stack, VM_PAGE_TO_PHYS(m[NPGPTD]));


	xen_pgdpt_pin(xpmap_ptom(VM_PAGE_TO_PHYS(m[NPGPTD + 1])));
	vm_page_lock_queues();
	for (i = 0; i < 4; i++) {
		int pdir = (PTDPTDI + i) / NPDEPG;
		int curoffset = (PTDPTDI + i) % NPDEPG;
		
		xen_queue_pt_update((vm_paddr_t)
		    ((ma[pdir] & ~PG_V) + (curoffset*sizeof(vm_paddr_t))), 
		    ma[i]);
	}
	PT_UPDATES_FLUSH();
	vm_page_unlock_queues();
	
	memset(&ctxt, 0, sizeof(ctxt));
	ctxt.flags = VGCF_IN_KERNEL;
	ctxt.user_regs.ds = GSEL(GDATA_SEL, SEL_KPL);
	ctxt.user_regs.es = GSEL(GDATA_SEL, SEL_KPL);
	ctxt.user_regs.fs = GSEL(GPRIV_SEL, SEL_KPL);
	ctxt.user_regs.gs = GSEL(GDATA_SEL, SEL_KPL);
	ctxt.user_regs.cs = GSEL(GCODE_SEL, SEL_KPL);
	ctxt.user_regs.ss = GSEL(GDATA_SEL, SEL_KPL);
	ctxt.user_regs.eip = (unsigned long)init_secondary;
	ctxt.user_regs.eflags = PSL_KERNEL | 0x1000; /* IOPL_RING1 */

	memset(&ctxt.fpu_ctxt, 0, sizeof(ctxt.fpu_ctxt));

	smp_trap_init(ctxt.trap_ctxt);

	ctxt.ldt_ents = 0;
	ctxt.gdt_frames[0] = (uint32_t)((uint64_t)vtomach(bootAPgdt) >> PAGE_SHIFT);
	ctxt.gdt_ents      = 512;

#ifdef __i386__
	ctxt.user_regs.esp = boot_stack + PAGE_SIZE;

	ctxt.kernel_ss = GSEL(GDATA_SEL, SEL_KPL);
	ctxt.kernel_sp = boot_stack + PAGE_SIZE;

	ctxt.event_callback_cs     = GSEL(GCODE_SEL, SEL_KPL);
	ctxt.event_callback_eip    = (unsigned long)Xhypervisor_callback;
	ctxt.failsafe_callback_cs  = GSEL(GCODE_SEL, SEL_KPL);
	ctxt.failsafe_callback_eip = (unsigned long)failsafe_callback;

	ctxt.ctrlreg[3] = xpmap_ptom(VM_PAGE_TO_PHYS(m[NPGPTD + 1]));
#else /* __x86_64__ */
	ctxt.user_regs.esp = idle->thread.rsp0 - sizeof(struct pt_regs);
	ctxt.kernel_ss = GSEL(GDATA_SEL, SEL_KPL);
	ctxt.kernel_sp = idle->thread.rsp0;

	ctxt.event_callback_eip    = (unsigned long)hypervisor_callback;
	ctxt.failsafe_callback_eip = (unsigned long)failsafe_callback;
	ctxt.syscall_callback_eip  = (unsigned long)system_call;

	ctxt.ctrlreg[3] = xen_pfn_to_cr3(virt_to_mfn(init_level4_pgt));

	ctxt.gs_base_kernel = (unsigned long)(cpu_pda(cpu));
#endif

	printf("gdtpfn=%lx pdptpfn=%lx\n",
	    ctxt.gdt_frames[0],
	    ctxt.ctrlreg[3] >> PAGE_SHIFT);

	PANIC_IF(HYPERVISOR_vcpu_op(VCPUOP_initialise, cpu, &ctxt));
	DELAY(1000);
	PANIC_IF(HYPERVISOR_vcpu_op(VCPUOP_up, cpu, NULL));
}

/*
 * This function starts the AP (application processor) identified
 * by the APIC ID 'physicalCpu'.  It does quite a "song and dance"
 * to accomplish this.  This is necessary because of the nuances
 * of the different hardware we might encounter.  It isn't pretty,
 * but it seems to work.
 */
static int
start_ap(int apic_id)
{
	int cpus, ms;

	/* used as a watchpoint to signal AP startup */
	cpus = mp_naps;

	cpu_initialize_context(apic_id);

	/* Wait up to 5 seconds for it to start. */
	for (ms = 0; ms < 5000; ms++) {
		if (mp_naps > cpus)
			return 1;	/* return SUCCESS */
		DELAY(1000);
	}
	return 0;		/* return FAILURE */
}

#ifdef COUNT_XINVLTLB_HITS
u_int xhits_gbl[MAXCPU];
u_int xhits_pg[MAXCPU];
u_int xhits_rng[MAXCPU];
SYSCTL_NODE(_debug, OID_AUTO, xhits, CTLFLAG_RW, 0, "");
SYSCTL_OPAQUE(_debug_xhits, OID_AUTO, global, CTLFLAG_RW, &xhits_gbl,
    sizeof(xhits_gbl), "IU", "");
SYSCTL_OPAQUE(_debug_xhits, OID_AUTO, page, CTLFLAG_RW, &xhits_pg,
    sizeof(xhits_pg), "IU", "");
SYSCTL_OPAQUE(_debug_xhits, OID_AUTO, range, CTLFLAG_RW, &xhits_rng,
    sizeof(xhits_rng), "IU", "");

u_int ipi_global;
u_int ipi_page;
u_int ipi_range;
u_int ipi_range_size;
SYSCTL_INT(_debug_xhits, OID_AUTO, ipi_global, CTLFLAG_RW, &ipi_global, 0, "");
SYSCTL_INT(_debug_xhits, OID_AUTO, ipi_page, CTLFLAG_RW, &ipi_page, 0, "");
SYSCTL_INT(_debug_xhits, OID_AUTO, ipi_range, CTLFLAG_RW, &ipi_range, 0, "");
SYSCTL_INT(_debug_xhits, OID_AUTO, ipi_range_size, CTLFLAG_RW, &ipi_range_size,
    0, "");

u_int ipi_masked_global;
u_int ipi_masked_page;
u_int ipi_masked_range;
u_int ipi_masked_range_size;
SYSCTL_INT(_debug_xhits, OID_AUTO, ipi_masked_global, CTLFLAG_RW,
    &ipi_masked_global, 0, "");
SYSCTL_INT(_debug_xhits, OID_AUTO, ipi_masked_page, CTLFLAG_RW,
    &ipi_masked_page, 0, "");
SYSCTL_INT(_debug_xhits, OID_AUTO, ipi_masked_range, CTLFLAG_RW,
    &ipi_masked_range, 0, "");
SYSCTL_INT(_debug_xhits, OID_AUTO, ipi_masked_range_size, CTLFLAG_RW,
    &ipi_masked_range_size, 0, "");
#endif /* COUNT_XINVLTLB_HITS */

/*
 * Flush the TLB on all other CPU's
 */
static void
smp_tlb_shootdown(u_int vector, vm_offset_t addr1, vm_offset_t addr2)
{
	u_int ncpu;
	struct _call_data data;

	call_data = &data;
	
	ncpu = mp_ncpus - 1;	/* does not shootdown self */
	if (ncpu < 1)
		return;		/* no other cpus */
	if (!(read_eflags() & PSL_I))
		panic("%s: interrupts disabled", __func__);
	mtx_lock_spin(&smp_ipi_mtx);
	call_data->func_id = vector;
	call_data->arg1 = addr1;
	call_data->arg2 = addr2;
	atomic_store_rel_int(&smp_tlb_wait, 0);
	ipi_all_but_self(vector);
	while (smp_tlb_wait < ncpu)
		ia32_pause();
	call_data = NULL;
	mtx_unlock_spin(&smp_ipi_mtx);
}

static void
smp_targeted_tlb_shootdown(u_int mask, u_int vector, vm_offset_t addr1, vm_offset_t addr2)
{
	int ncpu, othercpus;
	struct _call_data data;

	othercpus = mp_ncpus - 1;
	if (mask == (u_int)-1) {
		ncpu = othercpus;
		if (ncpu < 1)
			return;
	} else {
		mask &= ~PCPU_GET(cpumask);
		if (mask == 0)
			return;
		ncpu = bitcount32(mask);
		if (ncpu > othercpus) {
			/* XXX this should be a panic offence */
			printf("SMP: tlb shootdown to %d other cpus (only have %d)\n",
			    ncpu, othercpus);
			ncpu = othercpus;
		}
		/* XXX should be a panic, implied by mask == 0 above */
		if (ncpu < 1)
			return;
	}
	if (!(read_eflags() & PSL_I))
		panic("%s: interrupts disabled", __func__);
	mtx_lock_spin(&smp_ipi_mtx);
	call_data = &data;		
	call_data->func_id = vector;
	call_data->arg1 = addr1;
	call_data->arg2 = addr2;
	atomic_store_rel_int(&smp_tlb_wait, 0);
	if (mask == (u_int)-1)
		ipi_all_but_self(vector);
	else
		ipi_selected(mask, vector);
	while (smp_tlb_wait < ncpu)
		ia32_pause();
	call_data = NULL;
	mtx_unlock_spin(&smp_ipi_mtx);
}

void
smp_cache_flush(void)
{

	if (smp_started)
		smp_tlb_shootdown(IPI_INVLCACHE, 0, 0);
}

void
smp_invltlb(void)
{

	if (smp_started) {
		smp_tlb_shootdown(IPI_INVLTLB, 0, 0);
#ifdef COUNT_XINVLTLB_HITS
		ipi_global++;
#endif
	}
}

void
smp_invlpg(vm_offset_t addr)
{

	if (smp_started) {
		smp_tlb_shootdown(IPI_INVLPG, addr, 0);
#ifdef COUNT_XINVLTLB_HITS
		ipi_page++;
#endif
	}
}

void
smp_invlpg_range(vm_offset_t addr1, vm_offset_t addr2)
{

	if (smp_started) {
		smp_tlb_shootdown(IPI_INVLRNG, addr1, addr2);
#ifdef COUNT_XINVLTLB_HITS
		ipi_range++;
		ipi_range_size += (addr2 - addr1) / PAGE_SIZE;
#endif
	}
}

void
smp_masked_invltlb(u_int mask)
{

	if (smp_started) {
		smp_targeted_tlb_shootdown(mask, IPI_INVLTLB, 0, 0);
#ifdef COUNT_XINVLTLB_HITS
		ipi_masked_global++;
#endif
	}
}

void
smp_masked_invlpg(u_int mask, vm_offset_t addr)
{

	if (smp_started) {
		smp_targeted_tlb_shootdown(mask, IPI_INVLPG, addr, 0);
#ifdef COUNT_XINVLTLB_HITS
		ipi_masked_page++;
#endif
	}
}

void
smp_masked_invlpg_range(u_int mask, vm_offset_t addr1, vm_offset_t addr2)
{

	if (smp_started) {
		smp_targeted_tlb_shootdown(mask, IPI_INVLRNG, addr1, addr2);
#ifdef COUNT_XINVLTLB_HITS
		ipi_masked_range++;
		ipi_masked_range_size += (addr2 - addr1) / PAGE_SIZE;
#endif
	}
}

/*
 * send an IPI to a set of cpus.
 */
void
ipi_selected(uint32_t cpus, u_int ipi)
{
	int cpu;
	u_int bitmap = 0;
	u_int old_pending;
	u_int new_pending;
	
	if (IPI_IS_BITMAPED(ipi)) { 
		bitmap = 1 << ipi;
		ipi = IPI_BITMAP_VECTOR;
	} 

	CTR3(KTR_SMP, "%s: cpus: %x ipi: %x", __func__, cpus, ipi);
	while ((cpu = ffs(cpus)) != 0) {
		cpu--;
		cpus &= ~(1 << cpu);

		KASSERT(cpu_apic_ids[cpu] != -1,
		    ("IPI to non-existent CPU %d", cpu));

		if (bitmap) {
			do {
				old_pending = cpu_ipi_pending[cpu];
				new_pending = old_pending | bitmap;
			} while  (!atomic_cmpset_int(&cpu_ipi_pending[cpu],old_pending, new_pending));	

			if (!old_pending)
				ipi_pcpu(cpu, RESCHEDULE_VECTOR);
			continue;
			
		}
		
		KASSERT(call_data != NULL, ("call_data not set"));

		ipi_pcpu(cpu, CALL_FUNCTION_VECTOR);
	}

}

/*
 * send an IPI INTerrupt containing 'vector' to all CPUs, including myself
 */
void
ipi_all(u_int ipi)
{

	CTR2(KTR_SMP, "%s: ipi: %x", __func__, ipi);
	ipi_selected(all_cpus, ipi);
}

/*
 * send an IPI to all CPUs EXCEPT myself
 */
void
ipi_all_but_self(u_int ipi)
{

	CTR2(KTR_SMP, "%s: ipi: %x", __func__, ipi);
	ipi_selected(PCPU_GET(other_cpus), ipi);
}

/*
 * send an IPI to myself
 */
void
ipi_self(u_int ipi)
{

	CTR2(KTR_SMP, "%s: ipi: %x", __func__, ipi);
	ipi_selected(PCPU_GET(cpumask), ipi);
}

/*
 * This is called once the rest of the system is up and running and we're
 * ready to let the AP's out of the pen.
 */
static void
release_aps(void *dummy __unused)
{

	if (mp_ncpus == 1) 
		return;
	mtx_lock_spin(&sched_lock);
	atomic_store_rel_int(&aps_ready, 1);
	while (smp_started == 0)
		ia32_pause();
	mtx_unlock_spin(&sched_lock);
}
SYSINIT(start_aps, SI_SUB_SMP, SI_ORDER_FIRST, release_aps, NULL);
SYSINIT(start_ipis, SI_SUB_INTR, SI_ORDER_ANY, xen_smp_intr_init_cpus, NULL);

#ifdef COUNT_IPIS
/*
 * Setup interrupt counters for IPI handlers.
 */
static void
mp_ipi_intrcnt(void *dummy)
{
	char buf[64];
	int i;

	for (i = 0; i < mp_maxid; i++) {
		if (CPU_ABSENT(i))
			continue;
		snprintf(buf, sizeof(buf), "cpu%d: invltlb", i);
		intrcnt_add(buf, &ipi_invltlb_counts[i]);
		snprintf(buf, sizeof(buf), "cpu%d: invlrng", i);
		intrcnt_add(buf, &ipi_invlrng_counts[i]);
		snprintf(buf, sizeof(buf), "cpu%d: invlpg", i);
		intrcnt_add(buf, &ipi_invlpg_counts[i]);
#ifdef IPI_PREEMPTION
		snprintf(buf, sizeof(buf), "cpu%d: preempt", i);
		intrcnt_add(buf, &ipi_preempt_counts[i]);
#endif
		snprintf(buf, sizeof(buf), "cpu%d: ast", i);
		intrcnt_add(buf, &ipi_ast_counts[i]);
		snprintf(buf, sizeof(buf), "cpu%d: rendezvous", i);
		intrcnt_add(buf, &ipi_rendezvous_counts[i]);
		snprintf(buf, sizeof(buf), "cpu%d: lazypmap", i);
		intrcnt_add(buf, &ipi_lazypmap_counts[i]);
	}		
}
SYSINIT(mp_ipi_intrcnt, SI_SUB_INTR, SI_ORDER_MIDDLE, mp_ipi_intrcnt, NULL)
#endif
