/*-
 * Copyright (c) 1996-1997 John D. Polstra.
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

#ifndef _MACHINE_ELF_H_
#define	_MACHINE_ELF_H_ 1

/*
 * ELF definitions for the i386 architecture.
 */

#include <sys/elf32.h>	/* Definitions common to all 32 bit architectures. */

#define	__ELF_WORD_SIZE	32	/* Used by <sys/elf_generic.h> */
#include <sys/elf_generic.h>

#define	ELF_ARCH	EM_386

#define	ELF_MACHINE_OK(x) ((x) == EM_386 || (x) == EM_486)

/*
 * Auxiliary vector entries for passing information to the interpreter.
 *
 * The i386 supplement to the SVR4 ABI specification names this "auxv_t",
 * but POSIX lays claim to all symbols ending with "_t".
 */

typedef struct {	/* Auxiliary vector entry on initial stack */
	int	a_type;			/* Entry type. */
	union {
		long	a_val;		/* Integer value. */
		void	*a_ptr;		/* Address. */
		void	(*a_fcn)(void);	/* Function pointer (not used). */
	} a_un;
} Elf32_Auxinfo;

__ElfType(Auxinfo);

/*
 * Types of dynamic symbol hash table bucket and chain elements.
 *
 * This is inconsistent among 64 bit architectures, so a machine dependent
 * typedef is required.
 */

typedef Elf32_Off Elf32_Hashelt;

__ElfType(Hashelt);

/* Values for a_type. */
#define	AT_NULL		0	/* Terminates the vector. */
#define	AT_IGNORE	1	/* Ignored entry. */
#define	AT_EXECFD	2	/* File descriptor of program to load. */
#define	AT_PHDR		3	/* Program header of program already loaded. */
#define	AT_PHENT	4	/* Size of each program header entry. */
#define	AT_PHNUM	5	/* Number of program header entries. */
#define	AT_PAGESZ	6	/* Page size in bytes. */
#define	AT_BASE		7	/* Interpreter's base address. */
#define	AT_FLAGS	8	/* Flags (unused for i386). */
#define	AT_ENTRY	9	/* Where interpreter should transfer control. */

/*
 * The following non-standard values are used for passing information
 * from John Polstra's testbed program to the dynamic linker.  These
 * are expected to go away soon.
 *
 * Unfortunately, these overlap the Linux non-standard values, so they
 * must not be used in the same context.
 */
#define	AT_BRK		10	/* Starting point for sbrk and brk. */
#define	AT_DEBUG	11	/* Debugging level. */

/*
 * The following non-standard values are used in Linux ELF binaries.
 */
#define	AT_NOTELF	10	/* Program is not ELF ?? */
#define	AT_UID		11	/* Real uid. */
#define	AT_EUID		12	/* Effective uid. */
#define	AT_GID		13	/* Real gid. */
#define	AT_EGID		14	/* Effective gid. */

#define	AT_COUNT	15	/* Count of defined aux entry types. */

/*
 * Relocation types.
 */

#define	R_386_NONE	0	/* No relocation. */
#define	R_386_32	1	/* Add symbol value. */
#define	R_386_PC32	2	/* Add PC-relative symbol value. */
#define	R_386_GOT32	3	/* Add PC-relative GOT offset. */
#define	R_386_PLT32	4	/* Add PC-relative PLT offset. */
#define	R_386_COPY	5	/* Copy data from shared object. */
#define	R_386_GLOB_DAT	6	/* Set GOT entry to data address. */
#define	R_386_JMP_SLOT	7	/* Set GOT entry to code address. */
#define	R_386_RELATIVE	8	/* Add load address of shared object. */
#define	R_386_GOTOFF	9	/* Add GOT-relative symbol address. */
#define	R_386_GOTPC	10	/* Add PC-relative GOT table address. */

#define	R_386_COUNT	11	/* Count of defined relocation types. */

/* Define "machine" characteristics */
#define	ELF_TARG_CLASS	ELFCLASS32
#define	ELF_TARG_DATA	ELFDATA2LSB
#define	ELF_TARG_MACH	EM_386
#define	ELF_TARG_VER	1

#ifdef _KERNEL

/*
 * On the i386 we load the dynamic linker where a userland call
 * to mmap(0, ...) would put it.  The rationale behind this
 * calculation is that it leaves room for the heap to grow to
 * its maximum allowed size.
 */
#define ELF_RTLD_ADDR(vmspace) \
    (round_page((vm_offset_t)(vmspace)->vm_daddr + MAXDSIZ))

#endif /* _KERNEL */
#endif /* !_MACHINE_ELF_H_ */
