/*
 * Copyright (c) 1990,1991 Regents of The University of Michigan.
 * All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation, and that the name of The University
 * of Michigan not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission. This software is supplied as is without expressed or
 * implied warranties of any kind.
 *
 *	Research Systems Unix Group
 *	The University of Michigan
 *	c/o Mike Clark
 *	535 W. William Street
 *	Ann Arbor, Michigan
 *	+1-313-763-0525
 *	netatalk@itd.umich.edu
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/protosw.h>
#include <sys/domain.h>
#include <sys/types.h>
#include <sys/socket.h>

#if defined( __FreeBSD__ )
#include <sys/kernel.h>
#include <net/if.h>
#include <net/radix.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/route.h>
#endif

#include "at.h"
#include "ddp.h"
#include "at_var.h"
#include "ddp_var.h"
#include <netatalk/at_extern.h>


#ifdef ultrix
extern int		ddp_ifoutput();
extern int		ddp_ifinput();
extern int		ddp_ifioctl();
#endif ultrix

struct protosw		atalksw[] = {
    {
	/* Identifiers */
	SOCK_DGRAM,	&atalkdomain,	ATPROTO_DDP,	PR_ATOMIC|PR_ADDR,
	/*
	 * protocol-protocol interface.
	 * fields are pr_input, pr_output, pr_ctlinput, and pr_ctloutput.
	 * pr_input can be called from the udp protocol stack for iptalk
	 * packets bound for a local socket.
	 * pr_output can be used by higher level appletalk protocols, should
	 * they be included in the kernel.
	 */
	0,		ddp_output,	0,		0,
	/* socket-protocol interface. */
	ddp_usrreq,
	/* utility routines. */
	ddp_init,	0,		0,		0,
#ifdef ultrix
	/* interface hooks */
	ddp_ifoutput,	ddp_ifinput,	ddp_ifioctl,	0,
#endif ultrix
    },
};

#if defined( __FreeBSD__ ) && defined ( NETATALKDEBUG )
extern int at_inithead();
#endif

struct domain		atalkdomain = {
    AF_APPLETALK,	"appletalk",	0,	0,	0,
    atalksw, &atalksw[sizeof(atalksw)/sizeof(atalksw[0])]
#if defined( __FreeBSD__ )
#ifdef NETATALKDEBUG
	, 0, at_inithead, 32, sizeof(struct sockaddr_at)
#else
	, 0, rn_inithead, 32, sizeof(struct sockaddr_at)
#endif
#endif
};

#if defined( __FreeBSD__ )
DOMAIN_SET(atalk);
#endif
