/*
 * Copyright (c) 1993 Jan-Simon Pendry
 * Copyright (c) 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Jan-Simon Pendry.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)procfs_ctl.c	8.4 (Berkeley) 6/15/94
 *
 * From:
 * $FreeBSD$
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/vnode.h>
#include <sys/ptrace.h>
#include <sys/signalvar.h>
#include <miscfs/procfs/procfs.h>

#include <vm/vm.h>

#ifndef FIX_SSTEP
#define FIX_SSTEP(p)
#endif

/*
 * True iff process (p) is in trace wait state
 * relative to process (curp)
 */
#define TRACE_WAIT_P(curp, p) \
	((p)->p_stat == SSTOP && \
	 (p)->p_pptr == (curp) && \
	 ((p)->p_flag & P_TRACED))

#define PROCFS_CTL_ATTACH	1
#define PROCFS_CTL_DETACH	2
#define PROCFS_CTL_STEP		3
#define PROCFS_CTL_RUN		4
#define PROCFS_CTL_WAIT		5

static vfs_namemap_t ctlnames[] = {
	/* special /proc commands */
	{ "attach",	PROCFS_CTL_ATTACH },
	{ "detach",	PROCFS_CTL_DETACH },
	{ "step",	PROCFS_CTL_STEP },
	{ "run",	PROCFS_CTL_RUN },
	{ "wait",	PROCFS_CTL_WAIT },
	{ 0 },
};

static vfs_namemap_t signames[] = {
	/* regular signal names */
	{ "hup",	SIGHUP },	{ "int",	SIGINT },
	{ "quit",	SIGQUIT },	{ "ill",	SIGILL },
	{ "trap",	SIGTRAP },	{ "abrt",	SIGABRT },
	{ "iot",	SIGIOT },	{ "emt",	SIGEMT },
	{ "fpe",	SIGFPE },	{ "kill",	SIGKILL },
	{ "bus",	SIGBUS },	{ "segv",	SIGSEGV },
	{ "sys",	SIGSYS },	{ "pipe",	SIGPIPE },
	{ "alrm",	SIGALRM },	{ "term",	SIGTERM },
	{ "urg",	SIGURG },	{ "stop",	SIGSTOP },
	{ "tstp",	SIGTSTP },	{ "cont",	SIGCONT },
	{ "chld",	SIGCHLD },	{ "ttin",	SIGTTIN },
	{ "ttou",	SIGTTOU },	{ "io",		SIGIO },
	{ "xcpu",	SIGXCPU },	{ "xfsz",	SIGXFSZ },
	{ "vtalrm",	SIGVTALRM },	{ "prof",	SIGPROF },
	{ "winch",	SIGWINCH },	{ "info",	SIGINFO },
	{ "usr1",	SIGUSR1 },	{ "usr2",	SIGUSR2 },
	{ 0 },
};

static int	procfs_control __P((struct proc *curp, struct proc *p, int op));

static int
procfs_control(curp, p, op)
	struct proc *curp;
	struct proc *p;
	int op;
{
	int error;

	/*
	 * Authorization check: rely on normal debugging protection, except
	 * allow processes to disengage debugging on a process onto which
	 * they have previously attached, but no longer have permission to
	 * debug.
	 */
	if (op != PROCFS_CTL_DETACH &&
	    ((error = p_can(curp, p, P_CAN_DEBUG, NULL))))
		return (error);

	/*
	 * Attach - attaches the target process for debugging
	 * by the calling process.
	 */
	if (op == PROCFS_CTL_ATTACH) {
		/* check whether already being traced */
		if (p->p_flag & P_TRACED)
			return (EBUSY);

		/* can't trace yourself! */
		if (p->p_pid == curp->p_pid)
			return (EINVAL);

		/*
		 * Go ahead and set the trace flag.
		 * Save the old parent (it's reset in
		 *   _DETACH, and also in kern_exit.c:wait4()
		 * Reparent the process so that the tracing
		 *   proc gets to see all the action.
		 * Stop the target.
		 */
		p->p_flag |= P_TRACED;
		faultin(p);
		p->p_xstat = 0;		/* XXX ? */
		PROCTREE_LOCK(PT_EXCLUSIVE);
		if (p->p_pptr != curp) {
			p->p_oppid = p->p_pptr->p_pid;
			proc_reparent(p, curp);
		}
		PROCTREE_LOCK(PT_RELEASE);
		psignal(p, SIGSTOP);
		return (0);
	}

	/*
	 * Target process must be stopped, owned by (curp) and
	 * be set up for tracing (P_TRACED flag set).
	 * Allow DETACH to take place at any time for sanity.
	 * Allow WAIT any time, of course.
	 */
	switch (op) {
	case PROCFS_CTL_DETACH:
	case PROCFS_CTL_WAIT:
		break;

	default:
		PROCTREE_LOCK(PT_SHARED);
		mtx_enter(&sched_lock, MTX_SPIN);
		if (!TRACE_WAIT_P(curp, p)) {
			mtx_exit(&sched_lock, MTX_SPIN);
			PROCTREE_LOCK(PT_RELEASE);
			return (EBUSY);
		}
		mtx_exit(&sched_lock, MTX_SPIN);
		PROCTREE_LOCK(PT_RELEASE);
	}


#ifdef FIX_SSTEP
	/*
	 * do single-step fixup if needed
	 */
	FIX_SSTEP(p);
#endif

	/*
	 * Don't deliver any signal by default.
	 * To continue with a signal, just send
	 * the signal name to the ctl file
	 */
	p->p_xstat = 0;

	switch (op) {
	/*
	 * Detach.  Cleans up the target process, reparent it if possible
	 * and set it running once more.
	 */
	case PROCFS_CTL_DETACH:
		/* if not being traced, then this is a painless no-op */
		if ((p->p_flag & P_TRACED) == 0)
			return (0);

		/* not being traced any more */
		p->p_flag &= ~P_TRACED;

		/* remove pending SIGTRAP, else the process will die */
		SIGDELSET(p->p_siglist, SIGTRAP);

		/* give process back to original parent */
		PROCTREE_LOCK(PT_EXCLUSIVE);
		if (p->p_oppid != p->p_pptr->p_pid) {
			struct proc *pp;

			pp = pfind(p->p_oppid);
			if (pp)
				proc_reparent(p, pp);
		}
		PROCTREE_LOCK(PT_RELEASE);

		p->p_oppid = 0;
		p->p_flag &= ~P_WAITED;	/* XXX ? */
		wakeup((caddr_t) curp);	/* XXX for CTL_WAIT below ? */

		break;

	/*
	 * Step.  Let the target process execute a single instruction.
	 */
	case PROCFS_CTL_STEP:
		PHOLD(p);
		error = procfs_sstep(p);
		PRELE(p);
		if (error)
			return (error);
		break;

	/*
	 * Run.  Let the target process continue running until a breakpoint
	 * or some other trap.
	 */
	case PROCFS_CTL_RUN:
		break;

	/*
	 * Wait for the target process to stop.
	 * If the target is not being traced then just wait
	 * to enter
	 */
	case PROCFS_CTL_WAIT:
		error = 0;
		if (p->p_flag & P_TRACED) {
			PROCTREE_LOCK(PT_SHARED);
			mtx_enter(&sched_lock, MTX_SPIN);
			while (error == 0 &&
					(p->p_stat != SSTOP) &&
					(p->p_flag & P_TRACED) &&
					(p->p_pptr == curp)) {
				mtx_exit(&sched_lock, MTX_SPIN);
				PROCTREE_LOCK(PT_RELEASE);
				error = tsleep((caddr_t) p,
						PWAIT|PCATCH, "procfsx", 0);
				PROCTREE_LOCK(PT_SHARED);
				mtx_enter(&sched_lock, MTX_SPIN);
			}
			if (error == 0 && !TRACE_WAIT_P(curp, p))
				error = EBUSY;
			mtx_exit(&sched_lock, MTX_SPIN);
			PROCTREE_LOCK(PT_RELEASE);
		} else {
			mtx_enter(&sched_lock, MTX_SPIN);
			while (error == 0 && p->p_stat != SSTOP) {
				mtx_exit(&sched_lock, MTX_SPIN);
				error = tsleep((caddr_t) p,
						PWAIT|PCATCH, "procfs", 0);
				mtx_enter(&sched_lock, MTX_SPIN);
			}
			mtx_exit(&sched_lock, MTX_SPIN);
		}
		return (error);

	default:
		panic("procfs_control");
	}

	mtx_enter(&sched_lock, MTX_SPIN);
	if (p->p_stat == SSTOP)
		setrunnable(p);
	mtx_exit(&sched_lock, MTX_SPIN);
	return (0);
}

int
procfs_doctl(curp, p, pfs, uio)
	struct proc *curp;
	struct pfsnode *pfs;
	struct uio *uio;
	struct proc *p;
{
	int xlen;
	int error;
	char msg[PROCFS_CTLLEN+1];
	vfs_namemap_t *nm;

	if (uio->uio_rw != UIO_WRITE)
		return (EOPNOTSUPP);

	xlen = PROCFS_CTLLEN;
	error = vfs_getuserstr(uio, msg, &xlen);
	if (error)
		return (error);

	/*
	 * Map signal names into signal generation
	 * or debug control.  Unknown commands and/or signals
	 * return EOPNOTSUPP.
	 *
	 * Sending a signal while the process is being debugged
	 * also has the side effect of letting the target continue
	 * to run.  There is no way to single-step a signal delivery.
	 */
	error = EOPNOTSUPP;

	nm = vfs_findname(ctlnames, msg, xlen);
	if (nm) {
		error = procfs_control(curp, p, nm->nm_val);
	} else {
		nm = vfs_findname(signames, msg, xlen);
		if (nm) {
			PROCTREE_LOCK(PT_SHARED);
			mtx_enter(&sched_lock, MTX_SPIN);
			if (TRACE_WAIT_P(curp, p)) {
				p->p_xstat = nm->nm_val;
#ifdef FIX_SSTEP
				FIX_SSTEP(p);
#endif
				setrunnable(p);
				mtx_exit(&sched_lock, MTX_SPIN);
				PROCTREE_LOCK(PT_RELEASE);
			} else {
				mtx_exit(&sched_lock, MTX_SPIN);
				PROCTREE_LOCK(PT_RELEASE);
				psignal(p, nm->nm_val);
			}
			error = 0;
		}
	}

	return (error);
}
