/* $FreeBSD$ */
/*	$NetBSD: cd9660.c,v 1.5 1997/06/26 19:11:33 drochner Exp $	*/

/*
 * Copyright (C) 1996 Wolfgang Solfrank.
 * Copyright (C) 1996 TooLs GmbH.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by TooLs GmbH.
 * 4. The name of TooLs GmbH may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY TOOLS GMBH ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TOOLS GMBH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Stand-alone ISO9660 file reading package.
 *
 * Note: This doesn't support Rock Ridge extensions, extended attributes,
 * blocksizes other than 2048 bytes, multi-extent files, etc.
 */
#include <sys/param.h>
#include <string.h>
#include <sys/dirent.h>
#include <isofs/cd9660/iso.h>

#include "stand.h"

static int	cd9660_open(const char *path, struct open_file *f);
static int	cd9660_close(struct open_file *f);
static int	cd9660_read(struct open_file *f, void *buf, size_t size, size_t *resid);
static int	cd9660_write(struct open_file *f, void *buf, size_t size, size_t *resid);
static off_t	cd9660_seek(struct open_file *f, off_t offset, int where);
static int	cd9660_stat(struct open_file *f, struct stat *sb);

struct fs_ops cd9660_fsops = {
	"cd9660", cd9660_open, cd9660_close, cd9660_read, cd9660_write, cd9660_seek, cd9660_stat
};

struct file {
	int isdir;			/* nonzero if file is directory */
	off_t off;			/* Current offset within file */
	daddr_t bno;			/* Starting block number  */
	off_t size;			/* Size of file */
};

struct ptable_ent {
	char namlen	[ISODCL( 1, 1)];	/* 711 */
	char extlen	[ISODCL( 2, 2)];	/* 711 */
	char block	[ISODCL( 3, 6)];	/* 732 */
	char parent	[ISODCL( 7, 8)];	/* 722 */
	char name	[1];
};
#define	PTFIXSZ		8
#define	PTSIZE(pp)	roundup(PTFIXSZ + isonum_711((pp)->namlen), 2)

#define	cdb2devb(bno)	((bno) * ISO_DEFAULT_BLOCK_SIZE / DEV_BSIZE)

/* XXX these should be in the system headers */
static __inline int
isonum_722(p)
	u_char *p;
{
	return (*p << 8)|p[1];
}

static __inline int
isonum_732(p)
	u_char *p;
{
	return (*p << 24)|(p[1] << 16)|(p[2] << 8)|p[3];
}



static int
dirmatch(path, dp)
	const char *path;
	struct iso_directory_record *dp;
{
	char *cp;
	int i;

	cp = dp->name;
	for (i = isonum_711(dp->name_len); --i >= 0; path++, cp++) {
		if (!*path || *path == '/')
			break;
		if (toupper(*path) == *cp)
			continue;
		return 0;
	}
	if (*path && *path != '/')
		return 0;
	/*
	 * Allow stripping of trailing dots and the version number.
	 * Note that this will find the first instead of the last version
	 * of a file.
	 */
	if (i >= 0 && (*cp == ';' || *cp == '.')) {
		/* This is to prevent matching of numeric extensions */
		if (*cp == '.' && cp[1] != ';')
			return 0;
		while (--i >= 0)
			if (*++cp != ';' && (*cp < '0' || *cp > '9'))
				return 0;
	}
	return 1;
}

static int
cd9660_open(path, f)
	const char *path;
	struct open_file *f;
{
	struct file *fp = 0;
	void *buf;
	struct iso_primary_descriptor *vd;
	size_t buf_size, read, dsize, off;
	daddr_t bno, boff;
	struct iso_directory_record rec;
	struct iso_directory_record *dp = 0;
	int rc;
	
	/* First find the volume descriptor */
	buf = malloc(buf_size = ISO_DEFAULT_BLOCK_SIZE);
	vd = buf;
	for (bno = 16;; bno++) {
		twiddle();
		rc = f->f_dev->dv_strategy(f->f_devdata, F_READ, cdb2devb(bno),
					   ISO_DEFAULT_BLOCK_SIZE, buf, &read);
		if (rc)
			goto out;
		if (read != ISO_DEFAULT_BLOCK_SIZE) {
			rc = EIO;
			goto out;
		}
		rc = EINVAL;
		if (bcmp(vd->id, ISO_STANDARD_ID, sizeof vd->id) != 0)
			goto out;
		if (isonum_711(vd->type) == ISO_VD_END)
			goto out;
		if (isonum_711(vd->type) == ISO_VD_PRIMARY)
			break;
	}
	if (isonum_723(vd->logical_block_size) != ISO_DEFAULT_BLOCK_SIZE)
		goto out;
	
	rec = *(struct iso_directory_record *) vd->root_directory_record;
	if (*path == '/') path++; /* eat leading '/' */

	while (*path) {
		bno = isonum_733(rec.extent) + isonum_711(rec.ext_attr_length);
		dsize = isonum_733(rec.size);
		off = 0;
		boff = 0;

		while (off < dsize) {
			if ((off % ISO_DEFAULT_BLOCK_SIZE) == 0) {
				twiddle();
				rc = f->f_dev->dv_strategy
					(f->f_devdata, F_READ,
					 cdb2devb(bno + boff),
					 ISO_DEFAULT_BLOCK_SIZE,
					 buf, &read);
				if (rc)
					goto out;
				if (read != ISO_DEFAULT_BLOCK_SIZE) {
					rc = EIO;
					goto out;
				}
				boff++;
				dp = (struct iso_directory_record *) buf;
			}
			if (isonum_711(dp->length) == 0) {
			    /* skip to next block, if any */
			    off = boff * ISO_DEFAULT_BLOCK_SIZE;
			    continue;
			}

			if (dirmatch(path, dp))
				break;

			dp = (struct iso_directory_record *)
				((char *) dp + isonum_711(dp->length));
			off += isonum_711(dp->length);
		}
		if (off == dsize) {
			rc = ENOENT;
			goto out;
		}

		rec = *dp;
		while (*path && *path != '/') /* look for next component */
			path++;
		if (*path) path++; /* skip '/' */
	}
	
	/* allocate file system specific data structure */
	fp = malloc(sizeof(struct file));
	bzero(fp, sizeof(struct file));
	f->f_fsdata = (void *)fp;

	fp->isdir = (isonum_711(rec.flags) & 2) != 0;
	fp->off = 0;
	fp->bno = isonum_733(rec.extent) + isonum_711(rec.ext_attr_length);
	fp->size = isonum_733(rec.size);
	free(buf);
	
	return 0;
	
out:
	if (fp)
		free(fp);
	free(buf);
	
	return rc;
}

static int
cd9660_close(f)
	struct open_file *f;
{
	struct file *fp = (struct file *)f->f_fsdata;
	
	f->f_fsdata = 0;
	free(fp);
	
	return 0;
}

static int
cd9660_readfile(f, start, size, resid)
	struct open_file *f;
	void *start;
	size_t size;
	size_t *resid;
{
	struct file *fp = (struct file *)f->f_fsdata;
	int rc = 0;
	daddr_t bno;
	char buf[ISO_DEFAULT_BLOCK_SIZE];
	char *dp;
	size_t read, off;
	
	while (size) {
		if (fp->off < 0 || fp->off >= fp->size)
			break;
		bno = fp->off / ISO_DEFAULT_BLOCK_SIZE + fp->bno;
		if (fp->off & (ISO_DEFAULT_BLOCK_SIZE - 1)
		    || size < ISO_DEFAULT_BLOCK_SIZE)
			dp = buf;
		else
			dp = start;
		twiddle();	
		rc = f->f_dev->dv_strategy(f->f_devdata, F_READ, cdb2devb(bno),
					   ISO_DEFAULT_BLOCK_SIZE, dp, &read);
		if (rc)
			return rc;
		if (read != ISO_DEFAULT_BLOCK_SIZE)
			return EIO;
		if (dp == buf) {
			off = fp->off & (ISO_DEFAULT_BLOCK_SIZE - 1);
			if (read > off + size)
				read = off + size;
			read -= off;
			bcopy(buf + off, start, read);
			start += read;
			fp->off += read;
			size -= read;
		} else {
			start += ISO_DEFAULT_BLOCK_SIZE;
			fp->off += ISO_DEFAULT_BLOCK_SIZE;
			size -= ISO_DEFAULT_BLOCK_SIZE;
		}
	}
	if (resid)
		*resid = size;
	return rc;
}

static int
cd9660_readdir(f, start, size, resid)
	struct open_file *f;
	void *start;
	size_t size;
	size_t *resid;
{
	struct file *fp = (struct file *)f->f_fsdata;
	int rc = 0;
	daddr_t bno, boff;
	char buf[ISO_DEFAULT_BLOCK_SIZE];
	struct dirent *dp;
	struct dirent *lastdp;
	struct iso_directory_record *ep = 0;
	size_t read, off, reclen, namelen;
	
	if (fp->off < 0 || fp->off >= fp->size)
		return 0;
	boff = fp->off / ISO_DEFAULT_BLOCK_SIZE;
	bno = fp->bno;
	off = fp->off;

	if (off % ISO_DEFAULT_BLOCK_SIZE) {
		twiddle();	
		rc = f->f_dev->dv_strategy(f->f_devdata, F_READ, cdb2devb(bno),
					   ISO_DEFAULT_BLOCK_SIZE, buf, &read);
		if (rc)
			return rc;
		if (read != ISO_DEFAULT_BLOCK_SIZE)
			return EIO;
		boff++;
		ep = (struct iso_directory_record *)
			(buf + (off % ISO_DEFAULT_BLOCK_SIZE));
	}

	lastdp = dp = (struct dirent *) start;
	dp->d_fileno = 0;
	dp->d_type = DT_UNKNOWN;
	dp->d_namlen = 0;
	while (size && off < fp->size) {
		if ((off % ISO_DEFAULT_BLOCK_SIZE) == 0) {
			twiddle();
			rc = f->f_dev->dv_strategy
				(f->f_devdata, F_READ,
				 cdb2devb(bno + boff),
				 ISO_DEFAULT_BLOCK_SIZE,
				 buf, &read);
			if (rc)
				break;
			if (read != ISO_DEFAULT_BLOCK_SIZE) {
				rc = EIO;
				break;
			}
			boff++;
			ep = (struct iso_directory_record *) buf;
		}

		if (isonum_711(ep->length) == 0) {
			/* skip to next block, if any */
			off = boff * ISO_DEFAULT_BLOCK_SIZE;
			continue;
		}

		namelen = isonum_711(ep->name_len);
		if (namelen == 1 && ep->name[0] == 1)
			namelen = 2;
		reclen = sizeof(struct dirent) - (MAXNAMLEN+1) + namelen + 1;
		reclen = (reclen + 3) & ~3;
		if (reclen > size)
			break;

		dp->d_fileno = isonum_733(ep->extent);
		dp->d_reclen = reclen;
		if (isonum_711(ep->flags) & 2)
			dp->d_type = DT_DIR;
		else
			dp->d_type = DT_REG;
		dp->d_namlen = namelen;
		if (isonum_711(ep->name_len) == 1 && ep->name[0] == 0)
			strcpy(dp->d_name, ".");
		else if (isonum_711(ep->name_len) == 1 && ep->name[0] == 1)
			strcpy(dp->d_name, "..");
		else
			bcopy(ep->name, dp->d_name, dp->d_namlen);
		dp->d_name[dp->d_namlen] = 0;

		lastdp = dp;
		dp = (struct dirent *) ((char *) dp + dp->d_reclen);
		size -= reclen;
		ep = (struct iso_directory_record *)
		    ((char *) ep + isonum_711(ep->length));
		off += isonum_711(ep->length);
	}

	fp->off = off;
	lastdp->d_reclen += size;
	if (resid)
		*resid = 0;
	return rc;
}

static int
cd9660_read(f, start, size, resid)
	struct open_file *f;
	void *start;
	size_t size;
	size_t *resid;
{
	struct file *fp = (struct file *)f->f_fsdata;

	if (fp->isdir)
		return cd9660_readdir(f, start, size, resid);
	else
		return cd9660_readfile(f, start, size, resid);
}

static int
cd9660_write(f, start, size, resid)
	struct open_file *f;
	void *start;
	size_t size;
	size_t *resid;
{
	return EROFS;
}

static off_t
cd9660_seek(f, offset, where)
	struct open_file *f;
	off_t offset;
	int where;
{
	struct file *fp = (struct file *)f->f_fsdata;
	
	switch (where) {
	case SEEK_SET:
		fp->off = offset;
		break;
	case SEEK_CUR:
		fp->off += offset;
		break;
	case SEEK_END:
		fp->off = fp->size - offset;
		break;
	default:
		return -1;
	}
	return fp->off;
}

static int
cd9660_stat(f, sb)
	struct open_file *f;
	struct stat *sb;
{
	struct file *fp = (struct file *)f->f_fsdata;
	
	/* only important stuff */
	sb->st_mode = S_IRUSR | S_IRGRP | S_IROTH;
	if (fp->isdir)
		sb->st_mode |= S_IFDIR;
	else
		sb->st_mode |= S_IFREG;
	sb->st_uid = sb->st_gid = 0;
	sb->st_size = fp->size;
	return 0;
}
