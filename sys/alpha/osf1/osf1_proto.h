/*
 * System call prototypes.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * $FreeBSD$
 * created from;	FreeBSD: src/sys/alpha/osf1/syscalls.master,v 1.5 2001/09/01 19:36:47 dillon Exp 
 */

#ifndef _OSF1_SYSPROTO_H_
#define	_OSF1_SYSPROTO_H_

#include <sys/signal.h>

#include <sys/acl.h>

struct proc;

struct thread;

#define	PAD_(t)	(sizeof(register_t) <= sizeof(t) ? \
		0 : sizeof(register_t) - sizeof(t))

#if BYTE_ORDER == LITTLE_ENDIAN
#define	PADL_(t)	0
#define	PADR_(t)	PAD_(t)
#else
#define	PADL_(t)	PAD_(t)
#define	PADR_(t)	0
#endif

struct osf1_wait4_args {
	char pid_l_[PADL_(int)]; int pid; char pid_r_[PADR_(int)];
	char status_l_[PADL_(int *)]; int * status; char status_r_[PADR_(int *)];
	char options_l_[PADL_(int)]; int options; char options_r_[PADR_(int)];
	char rusage_l_[PADL_(struct osf1_rusage *)]; struct osf1_rusage * rusage; char rusage_r_[PADR_(struct osf1_rusage *)];
};
struct osf1_mknod_args {
	char path_l_[PADL_(char *)]; char * path; char path_r_[PADR_(char *)];
	char mode_l_[PADL_(int)]; int mode; char mode_r_[PADR_(int)];
	char dev_l_[PADL_(int)]; int dev; char dev_r_[PADR_(int)];
};
struct osf1_getfsstat_args {
	char buf_l_[PADL_(struct osf1_statfs *)]; struct osf1_statfs * buf; char buf_r_[PADR_(struct osf1_statfs *)];
	char bufsize_l_[PADL_(long)]; long bufsize; char bufsize_r_[PADR_(long)];
	char flags_l_[PADL_(int)]; int flags; char flags_r_[PADR_(int)];
};
struct osf1_lseek_args {
	char fd_l_[PADL_(int)]; int fd; char fd_r_[PADR_(int)];
	char offset_l_[PADL_(off_t)]; off_t offset; char offset_r_[PADR_(off_t)];
	char whence_l_[PADL_(int)]; int whence; char whence_r_[PADR_(int)];
};
struct osf1_mount_args {
	char type_l_[PADL_(int)]; int type; char type_r_[PADR_(int)];
	char path_l_[PADL_(char *)]; char * path; char path_r_[PADR_(char *)];
	char flags_l_[PADL_(int)]; int flags; char flags_r_[PADR_(int)];
	char data_l_[PADL_(caddr_t)]; caddr_t data; char data_r_[PADR_(caddr_t)];
};
struct osf1_unmount_args {
	char path_l_[PADL_(char *)]; char * path; char path_r_[PADR_(char *)];
	char flags_l_[PADL_(int)]; int flags; char flags_r_[PADR_(int)];
};
struct osf1_setuid_args {
	char uid_l_[PADL_(uid_t)]; uid_t uid; char uid_r_[PADR_(uid_t)];
};
struct osf1_access_args {
	char path_l_[PADL_(char *)]; char * path; char path_r_[PADR_(char *)];
	char flags_l_[PADL_(int)]; int flags; char flags_r_[PADR_(int)];
};
struct osf1_kill_args {
	char pid_l_[PADL_(int)]; int pid; char pid_r_[PADR_(int)];
	char signum_l_[PADL_(int)]; int signum; char signum_r_[PADR_(int)];
};
struct osf1_set_program_attributes_args {
	char text_start_l_[PADL_(vm_offset_t)]; vm_offset_t text_start; char text_start_r_[PADR_(vm_offset_t)];
	char text_len_l_[PADL_(vm_offset_t)]; vm_offset_t text_len; char text_len_r_[PADR_(vm_offset_t)];
	char bss_start_l_[PADL_(vm_offset_t)]; vm_offset_t bss_start; char bss_start_r_[PADR_(vm_offset_t)];
	char bss_len_l_[PADL_(vm_offset_t)]; vm_offset_t bss_len; char bss_len_r_[PADR_(vm_offset_t)];
};
struct osf1_open_args {
	char path_l_[PADL_(char *)]; char * path; char path_r_[PADR_(char *)];
	char flags_l_[PADL_(int)]; int flags; char flags_r_[PADR_(int)];
	char mode_l_[PADL_(int)]; int mode; char mode_r_[PADR_(int)];
};
struct osf1_sigprocmask_args {
	char how_l_[PADL_(int)]; int how; char how_r_[PADR_(int)];
	char mask_l_[PADL_(osf1_sigset_t)]; osf1_sigset_t mask; char mask_r_[PADR_(osf1_sigset_t)];
};
struct osf1_sigpending_args {
	char p_l_[PADL_(struct proc *)]; struct proc * p; char p_r_[PADR_(struct proc *)];
	char mask_l_[PADL_(osf1_sigset_t *)]; osf1_sigset_t * mask; char mask_r_[PADR_(osf1_sigset_t *)];
};
struct osf1_classcntl_args {
	char opcode_l_[PADL_(int)]; int opcode; char opcode_r_[PADR_(int)];
	char arg1_l_[PADL_(int)]; int arg1; char arg1_r_[PADR_(int)];
	char arg2_l_[PADL_(int)]; int arg2; char arg2_r_[PADR_(int)];
	char arg3_l_[PADL_(int)]; int arg3; char arg3_r_[PADR_(int)];
};
struct osf1_ioctl_args {
	char fd_l_[PADL_(int)]; int fd; char fd_r_[PADR_(int)];
	char com_l_[PADL_(int)]; int com; char com_r_[PADR_(int)];
	char data_l_[PADL_(caddr_t)]; caddr_t data; char data_r_[PADR_(caddr_t)];
};
struct osf1_reboot_args {
	char opt_l_[PADL_(int)]; int opt; char opt_r_[PADR_(int)];
};
struct osf1_execve_args {
	char path_l_[PADL_(char *)]; char * path; char path_r_[PADR_(char *)];
	char argp_l_[PADL_(char **)]; char ** argp; char argp_r_[PADR_(char **)];
	char envp_l_[PADL_(char **)]; char ** envp; char envp_r_[PADR_(char **)];
};
struct osf1_stat_args {
	char path_l_[PADL_(char *)]; char * path; char path_r_[PADR_(char *)];
	char ub_l_[PADL_(struct osf1_stat *)]; struct osf1_stat * ub; char ub_r_[PADR_(struct osf1_stat *)];
};
struct osf1_lstat_args {
	char path_l_[PADL_(char *)]; char * path; char path_r_[PADR_(char *)];
	char ub_l_[PADL_(struct osf1_stat *)]; struct osf1_stat * ub; char ub_r_[PADR_(struct osf1_stat *)];
};
struct osf1_mmap_args {
	char addr_l_[PADL_(caddr_t)]; caddr_t addr; char addr_r_[PADR_(caddr_t)];
	char len_l_[PADL_(size_t)]; size_t len; char len_r_[PADR_(size_t)];
	char prot_l_[PADL_(int)]; int prot; char prot_r_[PADR_(int)];
	char flags_l_[PADL_(int)]; int flags; char flags_r_[PADR_(int)];
	char fd_l_[PADL_(int)]; int fd; char fd_r_[PADR_(int)];
	char pos_l_[PADL_(off_t)]; off_t pos; char pos_r_[PADR_(off_t)];
};
struct osf1_madvise_args {
	register_t dummy;
};
struct osf1_setpgrp_args {
	char pid_l_[PADL_(int)]; int pid; char pid_r_[PADR_(int)];
	char pgid_l_[PADL_(int)]; int pgid; char pgid_r_[PADR_(int)];
};
struct osf1_setitimer_args {
	char which_l_[PADL_(u_int)]; u_int which; char which_r_[PADR_(u_int)];
	char itv_l_[PADL_(struct itimerval *)]; struct itimerval * itv; char itv_r_[PADR_(struct itimerval *)];
	char oitv_l_[PADL_(struct itimerval *)]; struct itimerval * oitv; char oitv_r_[PADR_(struct itimerval *)];
};
struct osf1_table_args {
	char id_l_[PADL_(long)]; long id; char id_r_[PADR_(long)];
	char index_l_[PADL_(long)]; long index; char index_r_[PADR_(long)];
	char addr_l_[PADL_(void *)]; void * addr; char addr_r_[PADR_(void *)];
	char nel_l_[PADL_(long)]; long nel; char nel_r_[PADR_(long)];
	char lel_l_[PADL_(u_long)]; u_long lel; char lel_r_[PADR_(u_long)];
};
struct osf1_getitimer_args {
	char which_l_[PADL_(u_int)]; u_int which; char which_r_[PADR_(u_int)];
	char itv_l_[PADL_(struct itimerval *)]; struct itimerval * itv; char itv_r_[PADR_(struct itimerval *)];
};
struct osf1_fstat_args {
	char fd_l_[PADL_(int)]; int fd; char fd_r_[PADR_(int)];
	char sb_l_[PADL_(void *)]; void * sb; char sb_r_[PADR_(void *)];
};
struct osf1_fcntl_args {
	char fd_l_[PADL_(int)]; int fd; char fd_r_[PADR_(int)];
	char cmd_l_[PADL_(int)]; int cmd; char cmd_r_[PADR_(int)];
	char arg_l_[PADL_(void *)]; void * arg; char arg_r_[PADR_(void *)];
};
struct osf1_select_args {
	char nd_l_[PADL_(u_int)]; u_int nd; char nd_r_[PADR_(u_int)];
	char in_l_[PADL_(fd_set *)]; fd_set * in; char in_r_[PADR_(fd_set *)];
	char ou_l_[PADL_(fd_set *)]; fd_set * ou; char ou_r_[PADR_(fd_set *)];
	char ex_l_[PADL_(fd_set *)]; fd_set * ex; char ex_r_[PADR_(fd_set *)];
	char tv_l_[PADL_(struct timeval *)]; struct timeval * tv; char tv_r_[PADR_(struct timeval *)];
};
struct osf1_socket_args {
	char domain_l_[PADL_(int)]; int domain; char domain_r_[PADR_(int)];
	char type_l_[PADL_(int)]; int type; char type_r_[PADR_(int)];
	char protocol_l_[PADL_(int)]; int protocol; char protocol_r_[PADR_(int)];
};
struct osf1_sigreturn_args {
	char sigcntxp_l_[PADL_(struct osigcontext *)]; struct osigcontext * sigcntxp; char sigcntxp_r_[PADR_(struct osigcontext *)];
};
struct osf1_sigsuspend_args {
	char ss_l_[PADL_(osf1_sigset_t)]; osf1_sigset_t ss; char ss_r_[PADR_(osf1_sigset_t)];
};
struct osf1_osigstack_args {
	char nss_l_[PADL_(struct sigstack *)]; struct sigstack * nss; char nss_r_[PADR_(struct sigstack *)];
	char oss_l_[PADL_(struct sigstack *)]; struct sigstack * oss; char oss_r_[PADR_(struct sigstack *)];
};
struct osf1_gettimeofday_args {
	char tp_l_[PADL_(struct timeval *)]; struct timeval * tp; char tp_r_[PADR_(struct timeval *)];
	char tzp_l_[PADL_(struct timezone *)]; struct timezone * tzp; char tzp_r_[PADR_(struct timezone *)];
};
struct osf1_getrusage_args {
	char who_l_[PADL_(long)]; long who; char who_r_[PADR_(long)];
	char rusage_l_[PADL_(void *)]; void * rusage; char rusage_r_[PADR_(void *)];
};
struct osf1_readv_args {
	char fd_l_[PADL_(int)]; int fd; char fd_r_[PADR_(int)];
	char iovp_l_[PADL_(struct osf1_iovec *)]; struct osf1_iovec * iovp; char iovp_r_[PADR_(struct osf1_iovec *)];
	char iovcnt_l_[PADL_(u_int)]; u_int iovcnt; char iovcnt_r_[PADR_(u_int)];
};
struct osf1_writev_args {
	char fd_l_[PADL_(int)]; int fd; char fd_r_[PADR_(int)];
	char iovp_l_[PADL_(struct osf1_iovec *)]; struct osf1_iovec * iovp; char iovp_r_[PADR_(struct osf1_iovec *)];
	char iovcnt_l_[PADL_(u_int)]; u_int iovcnt; char iovcnt_r_[PADR_(u_int)];
};
struct osf1_truncate_args {
	char path_l_[PADL_(char *)]; char * path; char path_r_[PADR_(char *)];
	char length_l_[PADL_(off_t)]; off_t length; char length_r_[PADR_(off_t)];
};
struct osf1_ftruncate_args {
	char fd_l_[PADL_(int)]; int fd; char fd_r_[PADR_(int)];
	char length_l_[PADL_(off_t)]; off_t length; char length_r_[PADR_(off_t)];
};
struct osf1_setgid_args {
	char gid_l_[PADL_(gid_t)]; gid_t gid; char gid_r_[PADR_(gid_t)];
};
struct osf1_sendto_args {
	char s_l_[PADL_(int)]; int s; char s_r_[PADR_(int)];
	char buf_l_[PADL_(caddr_t)]; caddr_t buf; char buf_r_[PADR_(caddr_t)];
	char len_l_[PADL_(size_t)]; size_t len; char len_r_[PADR_(size_t)];
	char flags_l_[PADL_(int)]; int flags; char flags_r_[PADR_(int)];
	char to_l_[PADL_(struct sockaddr *)]; struct sockaddr * to; char to_r_[PADR_(struct sockaddr *)];
	char tolen_l_[PADL_(int)]; int tolen; char tolen_r_[PADR_(int)];
};
struct osf1_getrlimit_args {
	char which_l_[PADL_(u_int)]; u_int which; char which_r_[PADR_(u_int)];
	char rlp_l_[PADL_(struct rlimit *)]; struct rlimit * rlp; char rlp_r_[PADR_(struct rlimit *)];
};
struct osf1_setrlimit_args {
	char which_l_[PADL_(u_int)]; u_int which; char which_r_[PADR_(u_int)];
	char rlp_l_[PADL_(struct rlimit *)]; struct rlimit * rlp; char rlp_r_[PADR_(struct rlimit *)];
};
struct osf1_sigaction_args {
	char signum_l_[PADL_(long)]; long signum; char signum_r_[PADR_(long)];
	char nsa_l_[PADL_(struct osf1_sigaction *)]; struct osf1_sigaction * nsa; char nsa_r_[PADR_(struct osf1_sigaction *)];
	char osa_l_[PADL_(struct osf1_sigaction *)]; struct osf1_sigaction * osa; char osa_r_[PADR_(struct osf1_sigaction *)];
	char sigtramp_l_[PADL_(void *)]; void * sigtramp; char sigtramp_r_[PADR_(void *)];
};
struct osf1_statfs_args {
	char path_l_[PADL_(char *)]; char * path; char path_r_[PADR_(char *)];
	char buf_l_[PADL_(struct osf1_statfs *)]; struct osf1_statfs * buf; char buf_r_[PADR_(struct osf1_statfs *)];
	char len_l_[PADL_(int)]; int len; char len_r_[PADR_(int)];
};
struct osf1_fstatfs_args {
	char fd_l_[PADL_(int)]; int fd; char fd_r_[PADR_(int)];
	char buf_l_[PADL_(struct osf1_statfs *)]; struct osf1_statfs * buf; char buf_r_[PADR_(struct osf1_statfs *)];
	char len_l_[PADL_(int)]; int len; char len_r_[PADR_(int)];
};
struct osf1_msync_args {
	char addr_l_[PADL_(caddr_t)]; caddr_t addr; char addr_r_[PADR_(caddr_t)];
	char len_l_[PADL_(size_t)]; size_t len; char len_r_[PADR_(size_t)];
	char flags_l_[PADL_(int)]; int flags; char flags_r_[PADR_(int)];
};
struct osf1_signal_args {
	char signum_l_[PADL_(int)]; int signum; char signum_r_[PADR_(int)];
	char handler_l_[PADL_(void *)]; void * handler; char handler_r_[PADR_(void *)];
};
struct osf1_sigaltstack_args {
	char nss_l_[PADL_(struct osf1_sigaltstack *)]; struct osf1_sigaltstack * nss; char nss_r_[PADR_(struct osf1_sigaltstack *)];
	char oss_l_[PADL_(struct osf1_sigaltstack *)]; struct osf1_sigaltstack * oss; char oss_r_[PADR_(struct osf1_sigaltstack *)];
};
struct osf1_sysinfo_args {
	char cmd_l_[PADL_(int)]; int cmd; char cmd_r_[PADR_(int)];
	char buf_l_[PADL_(char *)]; char * buf; char buf_r_[PADR_(char *)];
	char count_l_[PADL_(long)]; long count; char count_r_[PADR_(long)];
};
struct osf1_proplist_syscall_args {
	register_t dummy;
};
struct osf1_ntpadjtime_args {
	char tp_l_[PADL_(void *)]; void * tp; char tp_r_[PADR_(void *)];
};
struct osf1_ntpgettime_args {
	char tp_l_[PADL_(void *)]; void * tp; char tp_r_[PADR_(void *)];
};
struct osf1_pathconf_args {
	char path_l_[PADL_(char *)]; char * path; char path_r_[PADR_(char *)];
	char name_l_[PADL_(int)]; int name; char name_r_[PADR_(int)];
};
struct osf1_fpathconf_args {
	char fd_l_[PADL_(int)]; int fd; char fd_r_[PADR_(int)];
	char name_l_[PADL_(int)]; int name; char name_r_[PADR_(int)];
};
struct osf1_uswitch_args {
	char cmd_l_[PADL_(long)]; long cmd; char cmd_r_[PADR_(long)];
	char mask_l_[PADL_(long)]; long mask; char mask_r_[PADR_(long)];
};
struct osf1_usleep_thread_args {
	char sleep_l_[PADL_(struct timeval *)]; struct timeval * sleep; char sleep_r_[PADR_(struct timeval *)];
	char slept_l_[PADL_(struct timeval *)]; struct timeval * slept; char slept_r_[PADR_(struct timeval *)];
};
struct osf1_getsysinfo_args {
	char op_l_[PADL_(u_long)]; u_long op; char op_r_[PADR_(u_long)];
	char buffer_l_[PADL_(caddr_t)]; caddr_t buffer; char buffer_r_[PADR_(caddr_t)];
	char nbytes_l_[PADL_(u_long)]; u_long nbytes; char nbytes_r_[PADR_(u_long)];
	char arg_l_[PADL_(caddr_t)]; caddr_t arg; char arg_r_[PADR_(caddr_t)];
	char flag_l_[PADL_(u_long)]; u_long flag; char flag_r_[PADR_(u_long)];
};
struct osf1_setsysinfo_args {
	char op_l_[PADL_(u_long)]; u_long op; char op_r_[PADR_(u_long)];
	char buffer_l_[PADL_(caddr_t)]; caddr_t buffer; char buffer_r_[PADR_(caddr_t)];
	char nbytes_l_[PADL_(u_long)]; u_long nbytes; char nbytes_r_[PADR_(u_long)];
	char arg_l_[PADL_(caddr_t)]; caddr_t arg; char arg_r_[PADR_(caddr_t)];
	char flag_l_[PADL_(u_long)]; u_long flag; char flag_r_[PADR_(u_long)];
};
int	osf1_wait4 __P((struct thread *, struct osf1_wait4_args *));
int	osf1_mknod __P((struct thread *, struct osf1_mknod_args *));
int	osf1_getfsstat __P((struct thread *, struct osf1_getfsstat_args *));
int	osf1_lseek __P((struct thread *, struct osf1_lseek_args *));
int	osf1_mount __P((struct thread *, struct osf1_mount_args *));
int	osf1_unmount __P((struct thread *, struct osf1_unmount_args *));
int	osf1_setuid __P((struct thread *, struct osf1_setuid_args *));
int	osf1_access __P((struct thread *, struct osf1_access_args *));
int	osf1_kill __P((struct thread *, struct osf1_kill_args *));
int	osf1_set_program_attributes __P((struct thread *, struct osf1_set_program_attributes_args *));
int	osf1_open __P((struct thread *, struct osf1_open_args *));
int	osf1_sigprocmask __P((struct thread *, struct osf1_sigprocmask_args *));
int	osf1_sigpending __P((struct thread *, struct osf1_sigpending_args *));
int	osf1_classcntl __P((struct thread *, struct osf1_classcntl_args *));
int	osf1_ioctl __P((struct thread *, struct osf1_ioctl_args *));
int	osf1_reboot __P((struct thread *, struct osf1_reboot_args *));
int	osf1_execve __P((struct thread *, struct osf1_execve_args *));
int	osf1_stat __P((struct thread *, struct osf1_stat_args *));
int	osf1_lstat __P((struct thread *, struct osf1_lstat_args *));
int	osf1_mmap __P((struct thread *, struct osf1_mmap_args *));
int	osf1_madvise __P((struct thread *, struct osf1_madvise_args *));
int	osf1_setpgrp __P((struct thread *, struct osf1_setpgrp_args *));
int	osf1_setitimer __P((struct thread *, struct osf1_setitimer_args *));
int	osf1_table __P((struct thread *, struct osf1_table_args *));
int	osf1_getitimer __P((struct thread *, struct osf1_getitimer_args *));
int	osf1_fstat __P((struct thread *, struct osf1_fstat_args *));
int	osf1_fcntl __P((struct thread *, struct osf1_fcntl_args *));
int	osf1_select __P((struct thread *, struct osf1_select_args *));
int	osf1_socket __P((struct thread *, struct osf1_socket_args *));
int	osf1_sigreturn __P((struct thread *, struct osf1_sigreturn_args *));
int	osf1_sigsuspend __P((struct thread *, struct osf1_sigsuspend_args *));
int	osf1_osigstack __P((struct thread *, struct osf1_osigstack_args *));
int	osf1_gettimeofday __P((struct thread *, struct osf1_gettimeofday_args *));
int	osf1_getrusage __P((struct thread *, struct osf1_getrusage_args *));
int	osf1_readv __P((struct thread *, struct osf1_readv_args *));
int	osf1_writev __P((struct thread *, struct osf1_writev_args *));
int	osf1_truncate __P((struct thread *, struct osf1_truncate_args *));
int	osf1_ftruncate __P((struct thread *, struct osf1_ftruncate_args *));
int	osf1_setgid __P((struct thread *, struct osf1_setgid_args *));
int	osf1_sendto __P((struct thread *, struct osf1_sendto_args *));
int	osf1_getrlimit __P((struct thread *, struct osf1_getrlimit_args *));
int	osf1_setrlimit __P((struct thread *, struct osf1_setrlimit_args *));
int	osf1_sigaction __P((struct thread *, struct osf1_sigaction_args *));
int	osf1_statfs __P((struct thread *, struct osf1_statfs_args *));
int	osf1_fstatfs __P((struct thread *, struct osf1_fstatfs_args *));
int	osf1_msync __P((struct thread *, struct osf1_msync_args *));
int	osf1_signal __P((struct thread *, struct osf1_signal_args *));
int	osf1_sigaltstack __P((struct thread *, struct osf1_sigaltstack_args *));
int	osf1_sysinfo __P((struct thread *, struct osf1_sysinfo_args *));
int	osf1_proplist_syscall __P((struct thread *, struct osf1_proplist_syscall_args *));
int	osf1_ntpadjtime __P((struct thread *, struct osf1_ntpadjtime_args *));
int	osf1_ntpgettime __P((struct thread *, struct osf1_ntpgettime_args *));
int	osf1_pathconf __P((struct thread *, struct osf1_pathconf_args *));
int	osf1_fpathconf __P((struct thread *, struct osf1_fpathconf_args *));
int	osf1_uswitch __P((struct thread *, struct osf1_uswitch_args *));
int	osf1_usleep_thread __P((struct thread *, struct osf1_usleep_thread_args *));
int	osf1_getsysinfo __P((struct thread *, struct osf1_getsysinfo_args *));
int	osf1_setsysinfo __P((struct thread *, struct osf1_setsysinfo_args *));

#ifdef COMPAT_43


#endif /* COMPAT_43 */

#undef PAD_
#undef PADL_
#undef PADR_

#endif /* !_OSF1_SYSPROTO_H_ */
