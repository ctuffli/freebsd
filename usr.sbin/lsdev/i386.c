#include "lsdev.h"
#include <stdio.h>
#include <string.h>
#include <machine/vmparam.h>
#include <i386/isa/isa_device.h>
#include <i386/eisa/eisaconf.h>
#include <scsi/scsiconf.h>
#include <pci/pcivar.h>
#include <pci/pcireg.h>


static void print_isa(struct devconf *);
static void print_eisa(struct devconf *);
static void print_pci(struct devconf *);
static void print_scsi(struct devconf *);
static void print_disk(struct devconf *);

void
hprint_config(void)
{
	printf("# This listing automatically generated by lsdev(1)\n");
}

void
print_config(struct devconf *dc)
{
	if(vflag)
		printf("%d: ", dc->dc_number);

	switch(dc->dc_devtype) {
	case MDDT_CPU:
		printf("# CPU %s%d", dc->dc_name, dc->dc_unit);
		break;
	case MDDT_BUS:
		printf("controller %s%d", dc->dc_name, dc->dc_unit);
		break;
	case MDDT_ISA:
		if(dc->dc_datalen >= ISA_EXTERNALLEN) {
			print_isa(dc);
		} else {
printit:
			printf("%s%d at %s%d",
			       dc->dc_name, dc->dc_unit, dc->dc_pname,
			       dc->dc_punit);
		}
		break;
	case MDDT_EISA:
		if(dc->dc_datalen >= EISA_EXTERNALLEN) {
			print_eisa(dc);
		} else {
			goto printit;
		}
		break;
	case MDDT_PCI:
		if(dc->dc_datalen >= PCI_EXTERNAL_LEN) {
			print_pci(dc);
		} else {
			goto printit;
		}
		break;
	case MDDT_SCSI:
		if(dc->dc_datalen >= SCSI_EXTERNALLEN) {
			print_scsi(dc);
		} else {
			goto printit;
		}
		break;
	case MDDT_DISK:
		if(dc->dc_datalen >= DISK_EXTERNALLEN) {
			print_disk(dc);
		} else {
			goto printit;
		}
		break;

	default:
		if(dc->dc_devtype >= NDEVTYPES) {
			printf("%s%d (#%d) at %s%d",
			       dc->dc_name, dc->dc_unit, dc->dc_devtype,
			       dc->dc_pname, dc->dc_punit);
		} else {
			printf("%s%d (%s) at %s%d",
			       dc->dc_name, dc->dc_unit,
			       devtypes[dc->dc_devtype], dc->dc_pname,
			       dc->dc_punit);
		}
		break;
	}
	fputc('\n', stdout);
}

static void
print_isa(struct devconf *dc)
{
	struct isa_device *id = (struct isa_device *)dc->dc_data;

	printf("%s%d\tat isa?", dc->dc_name, dc->dc_unit);

	if(dc->dc_md.mddc_imask[0]) {
		printf(" %3.3s", dc->dc_md.mddc_imask);
	}

	if(vflag) {
		printf(" (id %d)", id->id_id);
	}

	if(id->id_iobase) {
		if(id->id_iobase < 0) {
			printf(" port ?");
		} else {
			printf(" port 0x%x", id->id_iobase);
		}
	}

	if(id->id_irq) {
		int bit = ffs(id->id_irq) - 1;

		if(id->id_irq & ~(1 << bit)) {
			printf(" irq ?");
		} else {
			printf(" irq %d", bit);
		}
	}

	if(id->id_drq >= 0) {
		printf(" drq %d", id->id_drq);
	}

	if(id->id_maddr) {
		if((unsigned long)id->id_maddr == ~0UL) {
			printf(" iomem ?");
		} else {
			printf(" iomem 0x%lx",
			       (unsigned long)id->id_maddr & ~KERNBASE);
		}
	}

	if(id->id_msize) {
		if(id->id_msize < 0) {
			printf(" iosiz ?", id->id_msize);
		} else {
			printf(" iosiz %d", id->id_msize);
		}
	}

	if(id->id_flags) {
		printf(" flags 0x%x", id->id_flags);
	}
}

static void
print_eisa(struct devconf *dc)
{
	struct eisa_device *e_dev = (struct eisa_device *)dc->dc_data;
        printf("%s%ld\tat eisa0 slot %d # %#x-%#x",
		dc->dc_name,              
		dc->dc_unit,              
		e_dev->ioconf.slot,
		e_dev->ioconf.iobase,
		e_dev->ioconf.iobase + e_dev->ioconf.iosize - 1);
	if(e_dev->ioconf.irq)
		printf(" irq %d", ffs(e_dev->ioconf.irq) - 1);
}

static void
print_pci(struct devconf *dc)
{
	/*
	**	PCI is capable of true autoconfiguration.
	**	The pci_device structure isn't required at all.
	**
	**	You can retrieve the configuration info
	**	from the configuration space.
	*/

	struct pci_externalize_buffer *pd =
		(struct pci_externalize_buffer *)dc->dc_data;

	u_long	data, pin, line;

	printf("%s%d\tat pci%d:%d",
		dc->dc_name, dc->dc_unit,
		pd->peb_pci_info.pi_bus,
		pd->peb_pci_info.pi_device);

	/*
	**	For now we show only the interrupt configuration,
	**	because this may collide with other parts of the
	**	configuration.
	*/

	data = pd->peb_config[PCI_INTERRUPT_REG / 4];
	pin  = PCI_INTERRUPT_PIN_EXTRACT  (data);
	line = PCI_INTERRUPT_LINE_EXTRACT (data);

	/*
	**	You may of course display all the configuration registers.
	**	Hint: The #defines are in <i386/pci/pcireg.h>
	*/

	if (pin) printf (" # int %c irq %d", pin + ('a'-1), line);
}

static void
print_scsi(struct devconf *dc)
{
	struct scsi_link *sl = (struct scsi_link *)dc->dc_data;

	printf("%s%d at SCSI bus %d:%d:%d",
	       dc->dc_name, dc->dc_unit, sl->scsibus, sl->target,
	       sl->lun);
	if(vflag) {
		if(sl->flags & SDEV_MEDIA_LOADED)
			printf(" (ready)");
		if(sl->flags & SDEV_OPEN)
			printf(" (open)");
		if(sl->flags & SDEV_BOUNCE)
			printf(" (bounce)");
	}
}

static void
print_disk(struct devconf *dc)
{
	int *slavep = (int *)dc->dc_data;

	printf("%s%d at %s%d drive %d",
	       dc->dc_name, dc->dc_unit, dc->dc_pname, dc->dc_punit, *slavep);
}

