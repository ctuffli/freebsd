/*-
 * Copyright (c) 2010 Juli Mallett <jmallett@FreeBSD.org>
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
 * $FreeBSD$
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/systm.h>

#include <sys/bus.h>
#include <sys/endian.h>
#include <sys/interrupt.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <sys/rman.h>

#include <vm/vm.h>
#include <vm/pmap.h>
#include <vm/vm_extern.h>

#include <machine/bus.h>
#include <machine/cpu.h>
#include <machine/pmap.h>

#include <contrib/octeon-sdk/cvmx.h>

#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>

#include <dev/pci/pcib_private.h>

#include <mips/cavium/octopcireg.h>

#include "pcib_if.h"

struct octopci_softc {
	device_t sc_dev;
	unsigned sc_domain;
	unsigned sc_bus;
};

static void	octopci_identify(driver_t *, device_t);
static int	octopci_probe(device_t);
static int	octopci_attach(device_t);
static int	octopci_read_ivar(device_t, device_t, int, uintptr_t *);
static int	octopci_maxslots(device_t);
static uint32_t	octopci_read_config(device_t, u_int, u_int, u_int, u_int, int);
static void	octopci_write_config(device_t, u_int, u_int, u_int, u_int, uint32_t, int);

static uint64_t	octopci_cs_addr(unsigned, unsigned, unsigned, unsigned);

static void
octopci_identify(driver_t *drv, device_t parent)
{
	BUS_ADD_CHILD(parent, 0, "pcib", 0);
}

static int
octopci_probe(device_t dev)
{
	if (device_get_unit(dev) != 0)
		return (ENXIO);
	/* XXX Check sysinfo flag.  */
	device_set_desc(dev, "Cavium Octeon PCI bridge");
	return (0);
}

static int
octopci_attach(device_t dev)
{
	struct octopci_softc *sc;

	sc = device_get_softc(dev);
	sc->sc_dev = dev;
	sc->sc_domain = 0;
	sc->sc_bus = 0;

	device_add_child(dev, "pci", 0);

	return (bus_generic_attach(dev));
}

static int
octopci_read_ivar(device_t dev, device_t child, int which, uintptr_t *result)
{
	struct octopci_softc *sc;
	
	sc = device_get_softc(dev);

	switch (which) {
	case PCIB_IVAR_DOMAIN:
		*result = sc->sc_domain;
		return (0);
	case PCIB_IVAR_BUS:
		*result = sc->sc_bus;
		return (0);
		
	}
	return (ENOENT);
}

static int
octopci_maxslots(device_t dev)
{
	return (PCI_SLOTMAX);
}

static uint32_t
octopci_read_config(device_t dev, u_int bus, u_int slot, u_int func, u_int reg,
    int bytes)
{
	struct octopci_softc *sc;
	uint64_t addr;
	uint32_t data;

	sc = device_get_softc(dev);

	addr = octopci_cs_addr(bus, slot, func, reg);

	switch (bytes) {
	case 4:
		data = le32toh(cvmx_read64_uint32(addr));
		return (data);
	case 2:
		data = le16toh(cvmx_read64_uint16(addr));
		return (data);
	case 1:
		data = cvmx_read64_uint8(addr);
		return (data);
	default:
		return ((uint32_t)-1);
	}
}

static void
octopci_write_config(device_t dev, u_int bus, u_int slot, u_int func, u_int reg,
    uint32_t data, int bytes)
{
	struct octopci_softc *sc;
	uint64_t addr;

	sc = device_get_softc(dev);

	addr = octopci_cs_addr(bus, slot, func, reg);

	switch (bytes) {
	case 4:
		cvmx_write64_uint32(addr, htole32(data));
		return;
	case 2:
		cvmx_write64_uint16(addr, htole16(data));
		return;
	case 1:
		cvmx_write64_uint8(addr, data);
		return;
	default:
		return;
	}
}

static uint64_t
octopci_cs_addr(unsigned bus, unsigned slot, unsigned func, unsigned reg)
{
	octeon_pci_config_space_address_t pci_addr;

	pci_addr.u64 = 0;
	pci_addr.s.upper = 2;
	pci_addr.s.io = 1;
	pci_addr.s.did = 3;
	pci_addr.s.subdid = CVMX_OCT_SUBDID_PCI_CFG;
	pci_addr.s.endian_swap = 1;
	pci_addr.s.bus = bus;
	pci_addr.s.dev = slot;
	pci_addr.s.func = func;
	pci_addr.s.reg = reg;

	return (pci_addr.u64);
}

static device_method_t octopci_methods[] = {
	/* Device interface */
	DEVMETHOD(device_identify,	octopci_identify),
	DEVMETHOD(device_probe,		octopci_probe),
	DEVMETHOD(device_attach,	octopci_attach),

	/* Bus interface */
	DEVMETHOD(bus_read_ivar,	octopci_read_ivar),
	DEVMETHOD(bus_print_child,	bus_generic_print_child),

	/* pcib interface */
	DEVMETHOD(pcib_maxslots,	octopci_maxslots),
	DEVMETHOD(pcib_read_config,	octopci_read_config),
	DEVMETHOD(pcib_write_config,	octopci_write_config),

	{0, 0}
};

static driver_t octopci_driver = {
	"pcib",
	octopci_methods,
	sizeof(struct octopci_softc),
};
static devclass_t octopci_devclass;
DRIVER_MODULE(octopci, ciu, octopci_driver, octopci_devclass, 0, 0);
