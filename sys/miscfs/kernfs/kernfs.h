/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software donated to Berkeley by
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
 *	@(#)kernfs.h	8.6 (Berkeley) 3/29/95
 * $Id: kernfs.h,v 1.8 1997/09/07 05:25:58 bde Exp $
 */

#define	_PATH_KERNFS	"/kern"		/* Default mountpoint */

#ifdef KERNEL
struct kernfs_mount {
	struct vnode	*kf_root;	/* Root node */
};

struct kernfs_node {
	struct kern_target *kf_kt;
};

#define VFSTOKERNFS(mp)	((struct kernfs_mount *)((mp)->mnt_data))
#define	VTOKERN(vp) ((struct kernfs_node *)(vp)->v_data)

#define kernfs_fhtovp ((int (*) __P((struct mount *, struct fid *, \
	    struct sockaddr *, struct vnode **, int *, struct ucred **)))eopnotsupp)
#define kernfs_quotactl ((int (*) __P((struct mount *, int, uid_t, caddr_t, \
	    struct proc *)))eopnotsupp)
#define kernfs_sync ((int (*) __P((struct mount *, int, struct ucred *, \
	    struct proc *)))nullop)
#define kernfs_sysctl ((int (*) __P((int *, u_int, void *, size_t *, void *, \
	    size_t, struct proc *)))eopnotsupp)
#define kernfs_vget ((int (*) __P((struct mount *, ino_t, struct vnode **))) \
	    eopnotsupp)
#define kernfs_vrele ((int (*) __P((struct mount *, struct vnode *))) \
	    eopnotsupp)
#define kernfs_vptofh ((int (*) __P((struct vnode *, struct fid *)))eopnotsupp)
extern vop_t **kernfs_vnodeop_p;
extern dev_t rrootdev;
#endif /* KERNEL */
