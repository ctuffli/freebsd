/*-
 * Copyright (c) 1998 Doug Rabson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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
 *
 *	$Id$
 */

#include "vt.h"
#include "bt.h"
#include "adv.h"
#include "aha.h"
#include "wdc.h"
#include "mse.h"
#include "ar.h"
#include "cs.h"
#include "cx.h"
#include "ed.h"
#include "el.h"
#include "ep.h"
#include "ex.h"
#include "fe.h"
#include "ie.h"
#include "le.h"
#include "lnc.h"
#include "rdp.h"
#include "sr.h"
#include "wl.h"
#include "ze.h"
#include "zp.h"
#include "oltr.h"
#include "pas.h"
#include "sb.h"
#include "sbxvi.h"
#include "sbmidi.h"
#include "awe.h"
#include "gus.h"
#include "mss.h"
#include "css.h"
#include "sscape.h"
#include "trix.h"
#include "opl.h"
#include "mpu.h"
#include "uart.h"
#include "pca.h"
#include "mcd.h"
#include "scd.h"
#include "matcd.h"
#include "wt.h"
#include "ctx.h"
#include "spigot.h"
#include "gp.h"
#include "gsc.h"
#include "joy.h"
#include "cy.h"
#include "dgb.h"
#include "dgm.h"
#include "labpc.h"
#include "rc.h"
#include "rp.h"
#include "tw.h"
#include "si.h"
#include "asc.h"
#include "stl.h"
#include "stli.h"
#include "loran.h"
#include "pcf.h"
#include "isic.h"
#include "tina.h"
#include "ppc.h"

struct old_isa_driver {
	int			type;
	struct isa_driver	*driver;
};

extern struct isa_driver  vtdriver;
extern struct isa_driver  btdriver;
extern struct isa_driver advdriver;
extern struct isa_driver ahadriver;
extern struct isa_driver wdcdriver;
extern struct isa_driver msedriver;
extern struct isa_driver  ardriver;
extern struct isa_driver  csdriver;
extern struct isa_driver  cxdriver;
extern struct isa_driver  eddriver;
extern struct isa_driver  eldriver;
extern struct isa_driver  epdriver;
extern struct isa_driver  exdriver;
extern struct isa_driver  fedriver;
extern struct isa_driver  iedriver;
extern struct isa_driver  ledriver;
extern struct isa_driver lncdriver;
extern struct isa_driver rdpdriver;
extern struct isa_driver  srdriver;
extern struct isa_driver  wldriver;
extern struct isa_driver  zedriver;
extern struct isa_driver  zpdriver;
extern struct isa_driver oltrdriver;
extern struct isa_driver pasdriver;
extern struct isa_driver  sbdriver;
extern struct isa_driver sbxvidriver;
extern struct isa_driver sbmididriver;
extern struct isa_driver awedriver;
extern struct isa_driver gusdriver;
extern struct isa_driver mssdriver;
extern struct isa_driver cssdriver;
extern struct isa_driver sscapedriver;
extern struct isa_driver trixdriver;
extern struct isa_driver sscape_mssdriver;
extern struct isa_driver opldriver;
extern struct isa_driver mpudriver;
extern struct isa_driver uartdriver;
extern struct isa_driver pcadriver;
extern struct isa_driver mcddriver;
extern struct isa_driver scddriver;
extern struct isa_driver matcddriver;
extern struct isa_driver  wtdriver;
extern struct isa_driver ctxdriver;
extern struct isa_driver spigotdriver;
extern struct isa_driver  gpdriver;
extern struct isa_driver gscdriver;
extern struct isa_driver joydriver;
extern struct isa_driver  cydriver;
extern struct isa_driver dgbdriver;
extern struct isa_driver dgmdriver;
extern struct isa_driver labpcdriver;
extern struct isa_driver  rcdriver;
extern struct isa_driver  rpdriver;
extern struct isa_driver  twdriver;
extern struct isa_driver  sidriver;
extern struct isa_driver ascdriver;
extern struct isa_driver stldriver;
extern struct isa_driver stlidriver;
extern struct isa_driver lorandriver;
extern struct isa_driver pcfdriver;
extern struct isa_driver isicdriver;
extern struct isa_driver tinadriver;
extern struct isa_driver ppcdriver;


static struct old_isa_driver old_drivers[] = {

/* Sensitive TTY */

/* Sensitive BIO */

/* Sensitive NET */
#if NED > 0
	{ DRIVER_TYPE_NET, &eddriver },
#endif
#if NFE > 0
	{ DRIVER_TYPE_NET, &fedriver },
#endif
#if NRDP > 0
	{ DRIVER_TYPE_NET, &rdpdriver },
#endif

/* Sensitive CAM */

/* TTY */

#if NVT > 0
	{ DRIVER_TYPE_TTY, &vtdriver },
#endif
#if NMSE > 0
	{ DRIVER_TYPE_TTY, &msedriver },
#endif
#if NPCA > 0
	{ DRIVER_TYPE_TTY, &pcadriver },
#endif
#if NGP > 0
	{ DRIVER_TYPE_TTY, &gpdriver },
#endif
#if NGSC > 0
	{ DRIVER_TYPE_TTY, &gscdriver },
#endif
#if NCY > 0
	{ DRIVER_TYPE_TTY, &cydriver },
#endif
#if NDGB > 0
	{ DRIVER_TYPE_TTY, &dgbdriver },
#endif
#if NDGM > 0
	{ DRIVER_TYPE_TTY, &dgmdriver },
#endif
#if NLABPC > 0
	{ DRIVER_TYPE_TTY, &labpcdriver },
#endif
#if NRCD > 0
	{ DRIVER_TYPE_TTY, &rcdriver },
#endif
#if NRP > 0
	{ DRIVER_TYPE_TTY, &rpdriver },
#endif
#if NTW > 0
	{ DRIVER_TYPE_TTY, &twdriver },
#endif
#if NSI > 0
	{ DRIVER_TYPE_TTY, &sidriver },
#endif
#if NASC > 0
	{ DRIVER_TYPE_TTY, &ascdriver },
#endif
#if NSTL > 0
	{ DRIVER_TYPE_TTY, &stldriver },
#endif
#if NSTLI > 0
	{ DRIVER_TYPE_TTY, &stlidriver },
#endif
#if NLORAN > 0
	{ DRIVER_TYPE_TTY, &lorandriver },
#endif
#if NPPC > 0
	{ DRIVER_TYPE_TTY, &ppcdriver },
#endif

/* BIO */

#if NWDC > 0
	{ DRIVER_TYPE_BIO, &wdcdriver },
#endif
#if NMCD > 0
	{ DRIVER_TYPE_BIO, &mcddriver },
#endif
#if NSCD > 0
	{ DRIVER_TYPE_BIO, &scddriver },
#endif
#if NMATCD > 0
	{ DRIVER_TYPE_BIO, &matcddriver },
#endif
#if NWT > 0
	{ DRIVER_TYPE_BIO, &wtdriver },
#endif

/* NET */

#if NIE > 0
	{ DRIVER_TYPE_NET, &iedriver },
#endif
#if NEP > 0
	{ DRIVER_TYPE_NET, &epdriver },
#endif
#if NEX > 0
	{ DRIVER_TYPE_NET, &exdriver },
#endif
#if NLE > 0
	{ DRIVER_TYPE_NET, &ledriver },
#endif
#if NLNC > 0
	{ DRIVER_TYPE_NET, &lncdriver },
#endif
#if NZE > 0
	{ DRIVER_TYPE_NET, &zedriver },
#endif
#if NZP > 0
	{ DRIVER_TYPE_NET, &zpdriver },
#endif
#if NCS > 0
	{ DRIVER_TYPE_NET, &csdriver },
#endif
#if NAR > 0
	{ DRIVER_TYPE_NET, &ardriver },
#endif
#if NCX > 0
	{ DRIVER_TYPE_NET, &cxdriver },
#endif
#if NEL > 0
	{ DRIVER_TYPE_NET, &eldriver },
#endif
#if NSR > 0
	{ DRIVER_TYPE_NET, &srdriver },
#endif
#if NWL > 0
	{ DRIVER_TYPE_NET, &wldriver },
#endif
#if NPCF > 0
	{ DRIVER_TYPE_NET, &pcfdriver },
#endif
#if NISIC > 0
	{ DRIVER_TYPE_NET, &isicdriver },
#endif
#if NTINA > 0
	{ DRIVER_TYPE_NET, &tinadriver },
#endif

/* CAM */

#if NBT > 0
	{ DRIVER_TYPE_CAM, &btdriver },
#endif
#if NADV > 0
	{ DRIVER_TYPE_CAM, &advdriver },
#endif
#if NAHA > 0
	{ DRIVER_TYPE_CAM, &ahadriver },
#endif

/* MISC */

#if NOLTR > 0
	{ DRIVER_TYPE_MISC, &oltrdriver },
#endif
#if NPAS > 0
	{ DRIVER_TYPE_MISC, &pasdriver },
#endif
#if NSB > 0
	{ DRIVER_TYPE_MISC, &sbdriver },
#endif
#if NSBVXI > 0
	{ DRIVER_TYPE_MISC, &sbxvidriver },
#endif
#if NSBMIDI > 0
	{ DRIVER_TYPE_MISC, &sbmididriver },
#endif
#if NAWE > 0
	{ DRIVER_TYPE_MISC, &awedriver },
#endif
#if NGUS > 0
	{ DRIVER_TYPE_MISC, &gusdriver },
#endif
#if NMSS > 0
	{ DRIVER_TYPE_MISC, &mssdriver },
#endif
#if NCSS > 0
	{ DRIVER_TYPE_MISC, &cssdriver },
#endif
#if NSSCAPE > 0
	{ DRIVER_TYPE_MISC, &sscapedriver },
#endif
#if NTRIX > 0
	{ DRIVER_TYPE_MISC, &trixdriver },
#endif
#if NSSCAPE > 0
	{ DRIVER_TYPE_MISC, &sscape_mssdriver },
#endif
#if NOPL > 0
	{ DRIVER_TYPE_MISC, &opldriver },
#endif
#if NMPU > 0
	{ DRIVER_TYPE_MISC, &mpudriver },
#endif
#if NUART > 0
	{ DRIVER_TYPE_MISC, &uartdriver },
#endif
#if NCTX > 0
	{ DRIVER_TYPE_MISC, &ctxdriver },
#endif
#if NSPIGOT > 0
	{ DRIVER_TYPE_MISC, &spigotdriver },
#endif
#if NJOY > 0
	{ DRIVER_TYPE_MISC, &joydriver },
#endif

};

#define old_drivers_count (sizeof(old_drivers) / sizeof(old_drivers[0]))
