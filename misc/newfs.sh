#!/bin/sh

#
# Copyright (c) 2008-2011 Peter Holm <pho@FreeBSD.org>
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

. ../default.cfg

mount | grep "$mntpoint" | grep md${mdstart}$part > /dev/null && umount $mntpoint
mdconfig -l | grep md$mdstart > /dev/null &&  mdconfig -d -u $mdstart

mdconfig -a -t swap -s 1g -u $mdstart
bsdlabel -w md$mdstart auto

for opt in -O1 -O2 -U -j; do
	blocksize=4096
	while [ $blocksize -le 65536 ]; do
		for i in 8 4 2 1; do
			fragsize=$((blocksize / i))
			echo "`date '+%T'` newfs $opt -b $blocksize -f $fragsize md${mdstart}${part}"
			newfs $opt -b $blocksize -f $fragsize  md${mdstart}$part > /dev/null
			mount /dev/md${mdstart}$part $mntpoint
			export RUNDIR=$mntpoint/stressX
			export runRUNTIME=4m
			(cd ..; ./run.sh disk.cfg > /dev/null 2>&1) 
			while mount | grep "$mntpoint" | grep -q md${mdstart}$part; do
				umount $mntpoint > /dev/null 2>&1 || sleep 1
			done
		done
		blocksize=$((blocksize * 2))
	done
done
mdconfig -d -u $mdstart
