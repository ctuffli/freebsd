#!/bin/sh

#
# Copyright (c) 2011 Peter Holm <pho@FreeBSD.org>
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

[ `id -u ` -ne 0 ] && echo "Must be root!" && exit 1

# Run a test on a 128 MB malloc backed MD with UFS SU fs with option trim.
# Verify that deleted records are gone.

. ../default.cfg

mount | grep $mntpoint | grep -q /dev/md && umount -f $mntpoint
mdconfig -l | grep -q md$mdstart &&  mdconfig -d -u $mdstart
mdconfig -a -t malloc -o reserve -s 128m -u $mdstart || exit 1
bsdlabel -w md$mdstart auto
[ $# -eq 0 ] && trim=-t
newfs $trim -U md${mdstart}$part > /dev/null
mount /dev/md${mdstart}$part $mntpoint

echo "This is a Trim (TRIM) test." >> $mntpoint/file
for i in `jot 20`; do
	cat $mntpoint/file $mntpoint/file > $mntpoint/file2
	mv $mntpoint/file2 $mntpoint/file
done
rm $mntpoint/file

while mount | grep $mntpoint | grep -q /dev/md; do
	umount $mntpoint || sleep 1
done

grep -a -qm1 Trim /dev/md${mdstart}$part && echo "Test failed"

fsck -t ufs -y /dev/md${mdstart}$part 2>&1 | tee /tmp/fsck.log | \
     grep -v "IS CLEAN" | egrep -q  -m1 "[A-Z][A-Z]" && \
     cat /tmp/fsck.log
mdconfig -d -u $mdstart
rm -f /tmp/fsck.log
