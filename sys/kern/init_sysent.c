/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * $FreeBSD$
 * created from FreeBSD: src/sys/kern/syscalls.master,v 1.68 1999/12/19 05:54:46 rwatson Exp 
 */

#include "opt_compat.h"

#include <sys/param.h>
#include <sys/sysent.h>
#include <sys/sysproto.h>

#ifdef COMPAT_43
#define compat(n, name) n, (sy_call_t *)__CONCAT(o,name)
#else
#define compat(n, name) 0, (sy_call_t *)nosys
#endif

/* The casts are bogus but will do for now. */
struct sysent sysent[] = {
	{ 0, (sy_call_t *)nosys },			/* 0 = syscall */
	{ 1, (sy_call_t *)exit },			/* 1 = exit */
	{ 0, (sy_call_t *)fork },			/* 2 = fork */
	{ 3, (sy_call_t *)read },			/* 3 = read */
	{ 3, (sy_call_t *)write },			/* 4 = write */
	{ 3, (sy_call_t *)open },			/* 5 = open */
	{ 1, (sy_call_t *)close },			/* 6 = close */
	{ 4, (sy_call_t *)wait4 },			/* 7 = wait4 */
	{ compat(2,creat) },		/* 8 = old creat */
	{ 2, (sy_call_t *)link },			/* 9 = link */
	{ 1, (sy_call_t *)unlink },			/* 10 = unlink */
	{ 0, (sy_call_t *)nosys },			/* 11 = obsolete execv */
	{ 1, (sy_call_t *)chdir },			/* 12 = chdir */
	{ 1, (sy_call_t *)fchdir },			/* 13 = fchdir */
	{ 3, (sy_call_t *)mknod },			/* 14 = mknod */
	{ 2, (sy_call_t *)chmod },			/* 15 = chmod */
	{ 3, (sy_call_t *)chown },			/* 16 = chown */
	{ 1, (sy_call_t *)obreak },			/* 17 = break */
	{ 3, (sy_call_t *)getfsstat },			/* 18 = getfsstat */
	{ compat(3,lseek) },		/* 19 = old lseek */
	{ 0, (sy_call_t *)getpid },			/* 20 = getpid */
	{ 4, (sy_call_t *)mount },			/* 21 = mount */
	{ 2, (sy_call_t *)unmount },			/* 22 = unmount */
	{ 1, (sy_call_t *)setuid },			/* 23 = setuid */
	{ 0, (sy_call_t *)getuid },			/* 24 = getuid */
	{ 0, (sy_call_t *)geteuid },			/* 25 = geteuid */
	{ 4, (sy_call_t *)ptrace },			/* 26 = ptrace */
	{ 3, (sy_call_t *)recvmsg },			/* 27 = recvmsg */
	{ 3, (sy_call_t *)sendmsg },			/* 28 = sendmsg */
	{ 6, (sy_call_t *)recvfrom },			/* 29 = recvfrom */
	{ 3, (sy_call_t *)accept },			/* 30 = accept */
	{ 3, (sy_call_t *)getpeername },		/* 31 = getpeername */
	{ 3, (sy_call_t *)getsockname },		/* 32 = getsockname */
	{ 2, (sy_call_t *)access },			/* 33 = access */
	{ 2, (sy_call_t *)chflags },			/* 34 = chflags */
	{ 2, (sy_call_t *)fchflags },			/* 35 = fchflags */
	{ 0, (sy_call_t *)sync },			/* 36 = sync */
	{ 2, (sy_call_t *)kill },			/* 37 = kill */
	{ compat(2,stat) },		/* 38 = old stat */
	{ 0, (sy_call_t *)getppid },			/* 39 = getppid */
	{ compat(2,lstat) },		/* 40 = old lstat */
	{ 1, (sy_call_t *)dup },			/* 41 = dup */
	{ 0, (sy_call_t *)pipe },			/* 42 = pipe */
	{ 0, (sy_call_t *)getegid },			/* 43 = getegid */
	{ 4, (sy_call_t *)profil },			/* 44 = profil */
	{ 4, (sy_call_t *)ktrace },			/* 45 = ktrace */
	{ compat(3,sigaction) },		/* 46 = old sigaction */
	{ 0, (sy_call_t *)getgid },			/* 47 = getgid */
	{ compat(2,sigprocmask) },		/* 48 = old sigprocmask */
	{ 2, (sy_call_t *)getlogin },			/* 49 = getlogin */
	{ 1, (sy_call_t *)setlogin },			/* 50 = setlogin */
	{ 1, (sy_call_t *)acct },			/* 51 = acct */
	{ compat(0,sigpending) },		/* 52 = old sigpending */
	{ 2, (sy_call_t *)sigaltstack },		/* 53 = sigaltstack */
	{ 3, (sy_call_t *)ioctl },			/* 54 = ioctl */
	{ 1, (sy_call_t *)reboot },			/* 55 = reboot */
	{ 1, (sy_call_t *)revoke },			/* 56 = revoke */
	{ 2, (sy_call_t *)symlink },			/* 57 = symlink */
	{ 3, (sy_call_t *)readlink },			/* 58 = readlink */
	{ 3, (sy_call_t *)execve },			/* 59 = execve */
	{ 1, (sy_call_t *)umask },			/* 60 = umask */
	{ 1, (sy_call_t *)chroot },			/* 61 = chroot */
	{ compat(2,fstat) },		/* 62 = old fstat */
	{ compat(4,getkerninfo) },		/* 63 = old getkerninfo */
	{ compat(0,getpagesize) },		/* 64 = old getpagesize */
	{ 3, (sy_call_t *)msync },			/* 65 = msync */
	{ 0, (sy_call_t *)vfork },			/* 66 = vfork */
	{ 0, (sy_call_t *)nosys },			/* 67 = obsolete vread */
	{ 0, (sy_call_t *)nosys },			/* 68 = obsolete vwrite */
	{ 1, (sy_call_t *)sbrk },			/* 69 = sbrk */
	{ 1, (sy_call_t *)sstk },			/* 70 = sstk */
	{ compat(6,mmap) },		/* 71 = old mmap */
	{ 1, (sy_call_t *)ovadvise },			/* 72 = vadvise */
	{ 2, (sy_call_t *)munmap },			/* 73 = munmap */
	{ 3, (sy_call_t *)mprotect },			/* 74 = mprotect */
	{ 3, (sy_call_t *)madvise },			/* 75 = madvise */
	{ 0, (sy_call_t *)nosys },			/* 76 = obsolete vhangup */
	{ 0, (sy_call_t *)nosys },			/* 77 = obsolete vlimit */
	{ 3, (sy_call_t *)mincore },			/* 78 = mincore */
	{ 2, (sy_call_t *)getgroups },			/* 79 = getgroups */
	{ 2, (sy_call_t *)setgroups },			/* 80 = setgroups */
	{ 0, (sy_call_t *)getpgrp },			/* 81 = getpgrp */
	{ 2, (sy_call_t *)setpgid },			/* 82 = setpgid */
	{ 3, (sy_call_t *)setitimer },			/* 83 = setitimer */
	{ compat(0,wait) },		/* 84 = old wait */
	{ 1, (sy_call_t *)swapon },			/* 85 = swapon */
	{ 2, (sy_call_t *)getitimer },			/* 86 = getitimer */
	{ compat(2,gethostname) },		/* 87 = old gethostname */
	{ compat(2,sethostname) },		/* 88 = old sethostname */
	{ 0, (sy_call_t *)getdtablesize },		/* 89 = getdtablesize */
	{ 2, (sy_call_t *)dup2 },			/* 90 = dup2 */
	{ 0, (sy_call_t *)nosys },			/* 91 = getdopt */
	{ 3, (sy_call_t *)fcntl },			/* 92 = fcntl */
	{ 5, (sy_call_t *)select },			/* 93 = select */
	{ 0, (sy_call_t *)nosys },			/* 94 = setdopt */
	{ 1, (sy_call_t *)fsync },			/* 95 = fsync */
	{ 3, (sy_call_t *)setpriority },		/* 96 = setpriority */
	{ 3, (sy_call_t *)socket },			/* 97 = socket */
	{ 3, (sy_call_t *)connect },			/* 98 = connect */
	{ compat(3,accept) },		/* 99 = old accept */
	{ 2, (sy_call_t *)getpriority },		/* 100 = getpriority */
	{ compat(4,send) },		/* 101 = old send */
	{ compat(4,recv) },		/* 102 = old recv */
	{ compat(1,sigreturn) },		/* 103 = old sigreturn */
	{ 3, (sy_call_t *)bind },			/* 104 = bind */
	{ 5, (sy_call_t *)setsockopt },			/* 105 = setsockopt */
	{ 2, (sy_call_t *)listen },			/* 106 = listen */
	{ 0, (sy_call_t *)nosys },			/* 107 = obsolete vtimes */
	{ compat(3,sigvec) },		/* 108 = old sigvec */
	{ compat(1,sigblock) },		/* 109 = old sigblock */
	{ compat(1,sigsetmask) },		/* 110 = old sigsetmask */
	{ compat(1,sigsuspend) },		/* 111 = old sigsuspend */
	{ compat(2,sigstack) },		/* 112 = old sigstack */
	{ compat(3,recvmsg) },		/* 113 = old recvmsg */
	{ compat(3,sendmsg) },		/* 114 = old sendmsg */
	{ 0, (sy_call_t *)nosys },			/* 115 = obsolete vtrace */
	{ 2, (sy_call_t *)gettimeofday },		/* 116 = gettimeofday */
	{ 2, (sy_call_t *)getrusage },			/* 117 = getrusage */
	{ 5, (sy_call_t *)getsockopt },			/* 118 = getsockopt */
	{ 0, (sy_call_t *)nosys },			/* 119 = resuba */
	{ 3, (sy_call_t *)readv },			/* 120 = readv */
	{ 3, (sy_call_t *)writev },			/* 121 = writev */
	{ 2, (sy_call_t *)settimeofday },		/* 122 = settimeofday */
	{ 3, (sy_call_t *)fchown },			/* 123 = fchown */
	{ 2, (sy_call_t *)fchmod },			/* 124 = fchmod */
	{ compat(6,recvfrom) },		/* 125 = old recvfrom */
	{ 2, (sy_call_t *)setreuid },			/* 126 = setreuid */
	{ 2, (sy_call_t *)setregid },			/* 127 = setregid */
	{ 2, (sy_call_t *)rename },			/* 128 = rename */
	{ compat(2,truncate) },		/* 129 = old truncate */
	{ compat(2,ftruncate) },		/* 130 = old ftruncate */
	{ 2, (sy_call_t *)flock },			/* 131 = flock */
	{ 2, (sy_call_t *)mkfifo },			/* 132 = mkfifo */
	{ 6, (sy_call_t *)sendto },			/* 133 = sendto */
	{ 2, (sy_call_t *)shutdown },			/* 134 = shutdown */
	{ 4, (sy_call_t *)socketpair },			/* 135 = socketpair */
	{ 2, (sy_call_t *)mkdir },			/* 136 = mkdir */
	{ 1, (sy_call_t *)rmdir },			/* 137 = rmdir */
	{ 2, (sy_call_t *)utimes },			/* 138 = utimes */
	{ 0, (sy_call_t *)nosys },			/* 139 = obsolete 4.2 sigreturn */
	{ 2, (sy_call_t *)adjtime },			/* 140 = adjtime */
	{ compat(3,getpeername) },		/* 141 = old getpeername */
	{ compat(0,gethostid) },		/* 142 = old gethostid */
	{ compat(1,sethostid) },		/* 143 = old sethostid */
	{ compat(2,getrlimit) },		/* 144 = old getrlimit */
	{ compat(2,setrlimit) },		/* 145 = old setrlimit */
	{ compat(2,killpg) },		/* 146 = old killpg */
	{ 0, (sy_call_t *)setsid },			/* 147 = setsid */
	{ 4, (sy_call_t *)quotactl },			/* 148 = quotactl */
	{ compat(0,quota) },		/* 149 = old quota */
	{ compat(3,getsockname) },		/* 150 = old getsockname */
	{ 0, (sy_call_t *)nosys },			/* 151 = sem_lock */
	{ 0, (sy_call_t *)nosys },			/* 152 = sem_wakeup */
	{ 0, (sy_call_t *)nosys },			/* 153 = asyncdaemon */
	{ 0, (sy_call_t *)nosys },			/* 154 = nosys */
	{ 2, (sy_call_t *)nosys },			/* 155 = nfssvc */
	{ compat(4,getdirentries) },		/* 156 = old getdirentries */
	{ 2, (sy_call_t *)statfs },			/* 157 = statfs */
	{ 2, (sy_call_t *)fstatfs },			/* 158 = fstatfs */
	{ 0, (sy_call_t *)nosys },			/* 159 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 160 = nosys */
	{ 2, (sy_call_t *)nosys },			/* 161 = getfh */
	{ 2, (sy_call_t *)getdomainname },		/* 162 = getdomainname */
	{ 2, (sy_call_t *)setdomainname },		/* 163 = setdomainname */
	{ 1, (sy_call_t *)uname },			/* 164 = uname */
	{ 2, (sy_call_t *)sysarch },			/* 165 = sysarch */
	{ 3, (sy_call_t *)rtprio },			/* 166 = rtprio */
	{ 0, (sy_call_t *)nosys },			/* 167 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 168 = nosys */
	{ 5, (sy_call_t *)semsys },			/* 169 = semsys */
	{ 6, (sy_call_t *)msgsys },			/* 170 = msgsys */
	{ 4, (sy_call_t *)shmsys },			/* 171 = shmsys */
	{ 0, (sy_call_t *)nosys },			/* 172 = nosys */
	{ 6, (sy_call_t *)pread },			/* 173 = pread */
	{ 6, (sy_call_t *)pwrite },			/* 174 = pwrite */
	{ 0, (sy_call_t *)nosys },			/* 175 = nosys */
	{ 1, (sy_call_t *)ntp_adjtime },		/* 176 = ntp_adjtime */
	{ 0, (sy_call_t *)nosys },			/* 177 = sfork */
	{ 0, (sy_call_t *)nosys },			/* 178 = getdescriptor */
	{ 0, (sy_call_t *)nosys },			/* 179 = setdescriptor */
	{ 0, (sy_call_t *)nosys },			/* 180 = nosys */
	{ 1, (sy_call_t *)setgid },			/* 181 = setgid */
	{ 1, (sy_call_t *)setegid },			/* 182 = setegid */
	{ 1, (sy_call_t *)seteuid },			/* 183 = seteuid */
	{ 0, (sy_call_t *)nosys },			/* 184 = lfs_bmapv */
	{ 0, (sy_call_t *)nosys },			/* 185 = lfs_markv */
	{ 0, (sy_call_t *)nosys },			/* 186 = lfs_segclean */
	{ 0, (sy_call_t *)nosys },			/* 187 = lfs_segwait */
	{ 2, (sy_call_t *)stat },			/* 188 = stat */
	{ 2, (sy_call_t *)fstat },			/* 189 = fstat */
	{ 2, (sy_call_t *)lstat },			/* 190 = lstat */
	{ 2, (sy_call_t *)pathconf },			/* 191 = pathconf */
	{ 2, (sy_call_t *)fpathconf },			/* 192 = fpathconf */
	{ 0, (sy_call_t *)nosys },			/* 193 = nosys */
	{ 2, (sy_call_t *)getrlimit },			/* 194 = getrlimit */
	{ 2, (sy_call_t *)setrlimit },			/* 195 = setrlimit */
	{ 4, (sy_call_t *)getdirentries },		/* 196 = getdirentries */
	{ 8, (sy_call_t *)mmap },			/* 197 = mmap */
	{ 0, (sy_call_t *)nosys },			/* 198 = __syscall */
	{ 5, (sy_call_t *)lseek },			/* 199 = lseek */
	{ 4, (sy_call_t *)truncate },			/* 200 = truncate */
	{ 4, (sy_call_t *)ftruncate },			/* 201 = ftruncate */
	{ 6, (sy_call_t *)__sysctl },			/* 202 = __sysctl */
	{ 2, (sy_call_t *)mlock },			/* 203 = mlock */
	{ 2, (sy_call_t *)munlock },			/* 204 = munlock */
	{ 1, (sy_call_t *)undelete },			/* 205 = undelete */
	{ 2, (sy_call_t *)futimes },			/* 206 = futimes */
	{ 1, (sy_call_t *)getpgid },			/* 207 = getpgid */
	{ 0, (sy_call_t *)nosys },			/* 208 = newreboot */
	{ 3, (sy_call_t *)poll },			/* 209 = poll */
	{ 0, (sy_call_t *)lkmnosys },			/* 210 = lkmnosys */
	{ 0, (sy_call_t *)lkmnosys },			/* 211 = lkmnosys */
	{ 0, (sy_call_t *)lkmnosys },			/* 212 = lkmnosys */
	{ 0, (sy_call_t *)lkmnosys },			/* 213 = lkmnosys */
	{ 0, (sy_call_t *)lkmnosys },			/* 214 = lkmnosys */
	{ 0, (sy_call_t *)lkmnosys },			/* 215 = lkmnosys */
	{ 0, (sy_call_t *)lkmnosys },			/* 216 = lkmnosys */
	{ 0, (sy_call_t *)lkmnosys },			/* 217 = lkmnosys */
	{ 0, (sy_call_t *)lkmnosys },			/* 218 = lkmnosys */
	{ 0, (sy_call_t *)lkmnosys },			/* 219 = lkmnosys */
	{ 4, (sy_call_t *)__semctl },			/* 220 = __semctl */
	{ 3, (sy_call_t *)semget },			/* 221 = semget */
	{ 3, (sy_call_t *)semop },			/* 222 = semop */
	{ 1, (sy_call_t *)semconfig },			/* 223 = semconfig */
	{ 3, (sy_call_t *)msgctl },			/* 224 = msgctl */
	{ 2, (sy_call_t *)msgget },			/* 225 = msgget */
	{ 4, (sy_call_t *)msgsnd },			/* 226 = msgsnd */
	{ 5, (sy_call_t *)msgrcv },			/* 227 = msgrcv */
	{ 3, (sy_call_t *)shmat },			/* 228 = shmat */
	{ 3, (sy_call_t *)shmctl },			/* 229 = shmctl */
	{ 1, (sy_call_t *)shmdt },			/* 230 = shmdt */
	{ 3, (sy_call_t *)shmget },			/* 231 = shmget */
	{ 2, (sy_call_t *)clock_gettime },		/* 232 = clock_gettime */
	{ 2, (sy_call_t *)clock_settime },		/* 233 = clock_settime */
	{ 2, (sy_call_t *)clock_getres },		/* 234 = clock_getres */
	{ 0, (sy_call_t *)nosys },			/* 235 = timer_create */
	{ 0, (sy_call_t *)nosys },			/* 236 = timer_delete */
	{ 0, (sy_call_t *)nosys },			/* 237 = timer_settime */
	{ 0, (sy_call_t *)nosys },			/* 238 = timer_gettime */
	{ 0, (sy_call_t *)nosys },			/* 239 = timer_getoverrun */
	{ 2, (sy_call_t *)nanosleep },			/* 240 = nanosleep */
	{ 0, (sy_call_t *)nosys },			/* 241 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 242 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 243 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 244 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 245 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 246 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 247 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 248 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 249 = nosys */
	{ 3, (sy_call_t *)minherit },			/* 250 = minherit */
	{ 1, (sy_call_t *)rfork },			/* 251 = rfork */
	{ 3, (sy_call_t *)openbsd_poll },		/* 252 = openbsd_poll */
	{ 0, (sy_call_t *)issetugid },			/* 253 = issetugid */
	{ 3, (sy_call_t *)lchown },			/* 254 = lchown */
	{ 0, (sy_call_t *)nosys },			/* 255 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 256 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 257 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 258 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 259 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 260 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 261 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 262 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 263 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 264 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 265 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 266 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 267 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 268 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 269 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 270 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 271 = nosys */
	{ 3, (sy_call_t *)getdents },			/* 272 = getdents */
	{ 0, (sy_call_t *)nosys },			/* 273 = nosys */
	{ 2, (sy_call_t *)lchmod },			/* 274 = lchmod */
	{ 3, (sy_call_t *)lchown },			/* 275 = netbsd_lchown */
	{ 2, (sy_call_t *)lutimes },			/* 276 = lutimes */
	{ 3, (sy_call_t *)msync },			/* 277 = netbsd_msync */
	{ 2, (sy_call_t *)nstat },			/* 278 = nstat */
	{ 2, (sy_call_t *)nfstat },			/* 279 = nfstat */
	{ 2, (sy_call_t *)nlstat },			/* 280 = nlstat */
	{ 0, (sy_call_t *)nosys },			/* 281 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 282 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 283 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 284 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 285 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 286 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 287 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 288 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 289 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 290 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 291 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 292 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 293 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 294 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 295 = nosys */
	{ 0, (sy_call_t *)nosys },			/* 296 = nosys */
	{ 2, (sy_call_t *)fhstatfs },			/* 297 = fhstatfs */
	{ 2, (sy_call_t *)fhopen },			/* 298 = fhopen */
	{ 2, (sy_call_t *)fhstat },			/* 299 = fhstat */
	{ 1, (sy_call_t *)modnext },			/* 300 = modnext */
	{ 2, (sy_call_t *)modstat },			/* 301 = modstat */
	{ 1, (sy_call_t *)modfnext },			/* 302 = modfnext */
	{ 1, (sy_call_t *)modfind },			/* 303 = modfind */
	{ 1, (sy_call_t *)kldload },			/* 304 = kldload */
	{ 1, (sy_call_t *)kldunload },			/* 305 = kldunload */
	{ 1, (sy_call_t *)kldfind },			/* 306 = kldfind */
	{ 1, (sy_call_t *)kldnext },			/* 307 = kldnext */
	{ 2, (sy_call_t *)kldstat },			/* 308 = kldstat */
	{ 1, (sy_call_t *)kldfirstmod },		/* 309 = kldfirstmod */
	{ 1, (sy_call_t *)getsid },			/* 310 = getsid */
	{ 0, (sy_call_t *)nosys },			/* 311 = setresuid */
	{ 0, (sy_call_t *)nosys },			/* 312 = setresgid */
	{ 0, (sy_call_t *)nosys },			/* 313 = obsolete signanosleep */
	{ 1, (sy_call_t *)aio_return },			/* 314 = aio_return */
	{ 3, (sy_call_t *)aio_suspend },		/* 315 = aio_suspend */
	{ 2, (sy_call_t *)aio_cancel },			/* 316 = aio_cancel */
	{ 1, (sy_call_t *)aio_error },			/* 317 = aio_error */
	{ 1, (sy_call_t *)aio_read },			/* 318 = aio_read */
	{ 1, (sy_call_t *)aio_write },			/* 319 = aio_write */
	{ 4, (sy_call_t *)lio_listio },			/* 320 = lio_listio */
	{ 0, (sy_call_t *)yield },			/* 321 = yield */
	{ 1, (sy_call_t *)thr_sleep },			/* 322 = thr_sleep */
	{ 1, (sy_call_t *)thr_wakeup },			/* 323 = thr_wakeup */
	{ 1, (sy_call_t *)mlockall },			/* 324 = mlockall */
	{ 0, (sy_call_t *)munlockall },			/* 325 = munlockall */
	{ 2, (sy_call_t *)__getcwd },			/* 326 = __getcwd */
	{ 2, (sy_call_t *)sched_setparam },		/* 327 = sched_setparam */
	{ 2, (sy_call_t *)sched_getparam },		/* 328 = sched_getparam */
	{ 3, (sy_call_t *)sched_setscheduler },		/* 329 = sched_setscheduler */
	{ 1, (sy_call_t *)sched_getscheduler },		/* 330 = sched_getscheduler */
	{ 0, (sy_call_t *)sched_yield },		/* 331 = sched_yield */
	{ 1, (sy_call_t *)sched_get_priority_max },		/* 332 = sched_get_priority_max */
	{ 1, (sy_call_t *)sched_get_priority_min },		/* 333 = sched_get_priority_min */
	{ 2, (sy_call_t *)sched_rr_get_interval },		/* 334 = sched_rr_get_interval */
	{ 2, (sy_call_t *)utrace },			/* 335 = utrace */
	{ 8, (sy_call_t *)sendfile },			/* 336 = sendfile */
	{ 3, (sy_call_t *)kldsym },			/* 337 = kldsym */
	{ 1, (sy_call_t *)jail },			/* 338 = jail */
	{ 0, (sy_call_t *)nosys },			/* 339 = pioctl */
	{ 3, (sy_call_t *)sigprocmask },		/* 340 = sigprocmask */
	{ 1, (sy_call_t *)sigsuspend },			/* 341 = sigsuspend */
	{ 3, (sy_call_t *)sigaction },			/* 342 = sigaction */
	{ 1, (sy_call_t *)sigpending },			/* 343 = sigpending */
	{ 1, (sy_call_t *)sigreturn },			/* 344 = sigreturn */
	{ 0, (sy_call_t *)nosys },			/* 345 = sigtimedwait */
	{ 0, (sy_call_t *)nosys },			/* 346 = sigwaitinfo */
	{ 3, (sy_call_t *)acl_syscall_get_file },		/* 347 = acl_syscall_get_file */
	{ 3, (sy_call_t *)acl_syscall_set_file },		/* 348 = acl_syscall_set_file */
	{ 3, (sy_call_t *)acl_syscall_get_fd },		/* 349 = acl_syscall_get_fd */
	{ 3, (sy_call_t *)acl_syscall_set_fd },		/* 350 = acl_syscall_set_fd */
	{ 2, (sy_call_t *)acl_syscall_delete_file },		/* 351 = acl_syscall_delete_file */
	{ 2, (sy_call_t *)acl_syscall_delete_fd },		/* 352 = acl_syscall_delete_fd */
	{ 3, (sy_call_t *)acl_syscall_aclcheck_file },		/* 353 = acl_syscall_aclcheck_file */
	{ 3, (sy_call_t *)acl_syscall_aclcheck_fd },		/* 354 = acl_syscall_aclcheck_fd */
	{ 4, (sy_call_t *)extattrctl },			/* 355 = extattrctl */
	{ 4, (sy_call_t *)extattr_set_file },		/* 356 = extattr_set_file */
	{ 4, (sy_call_t *)extattr_get_file },		/* 357 = extattr_get_file */
	{ 2, (sy_call_t *)extattr_delete_file },		/* 358 = extattr_delete_file */
};
