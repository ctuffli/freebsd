#!/bin/sh

#
# Copyright (c) 2008, 2011 Peter Holm <pho@FreeBSD.org>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# $FreeBSD$
#

# Hunt for deadlock that could occur running umount and quota at the same time

[ `id -u ` -ne 0 ] && echo "Must be root!" && exit 1

. ../default.cfg

mounts=15		# Number of parallel scripts
mdstart=$mdstart	# Use md unit numbers from this point
D=$diskimage
export PATH_FSTAB=/tmp/fstab

if [ $# -eq 0 ]; then
	rm -f $PATH_FSTAB
	for i in `jot $mounts`; do
		m=$(( i + mdstart - 1 ))
		[ ! -d ${mntpoint}$m ] && mkdir ${mntpoint}$m
		mount | grep "$mntpoint" | grep -q md$m && umount ${mntpoint}$m
		mdconfig -l | grep -q md$m &&  mdconfig -d -u $m

		dede $D$m 1m 1
		mdconfig -a -t vnode -f $D$m -u $m
		bsdlabel -w md$m auto
		newfs md${m}${part} > /dev/null 2>&1
		echo "/dev/md${m}${part} ${mntpoint}$m ufs rw,userquota 2 2" >> $PATH_FSTAB
		mount ${mntpoint}$m
		edquota -u -f ${mntpoint}$m -e ${mntpoint}$m:100000:110000:15000:16000 root
		umount ${mntpoint}$m
	done
	sync;sync;sync

	# start the parallel tests
	touch /tmp/$0
	for i in `jot $mounts`; do
		m=$(( i + mdstart - 1 ))
		./$0 $m &
		./$0 find $m &
	done

	for i in `jot $mounts`; do
		wait; wait
	done

	for i in `jot $mounts`; do
		m=$(( i + mdstart - 1 ))
		mdconfig -d -u $m
		rm -f $D$m
	done
	rm -f $PATH_FSTAB
else
	if [ $1 = find ]; then
		while [ -r /tmp/$0 ]; do
			(
			quotaon  ${mntpoint}$2
			quotaoff ${mntpoint}$2
			) 2>&1 | egrep -v "No such file or directory"
		done
		echo "Done 1 @ `date '+%T'`"
	else

		# The test: Parallel mount and unmounts
		for i in `jot 1024`; do
			m=$1
			opt=`[ $(( m % 2 )) -eq 0 ] && echo -f`
			mount $opt /dev/md${m}${part} ${mntpoint}$m
			while mount | grep -qw $mntpoint$m; do
				opt=$([ $((`date '+%s'` % 2)) -eq 0 ] && echo "-f")
				umount $opt ${mntpoint}$m > /dev/null 2>&1
			done
		done
		rm -f /tmp/$0
		echo "Done 2 @ `date '+%T'`"
	fi
fi
