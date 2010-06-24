/*
 * Copyright (c) 2004-2007 Voltaire Inc.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <stdint.h>
#include <byteswap.h>

#ifdef __cplusplus
#  define BEGIN_C_DECLS extern "C" {
#  define END_C_DECLS   }
#else /* !__cplusplus */
#  define BEGIN_C_DECLS
#  define END_C_DECLS
#endif /* __cplusplus */

BEGIN_C_DECLS

#if __BYTE_ORDER == __LITTLE_ENDIAN
#ifndef ntohll
static inline uint64_t ntohll(uint64_t x) {
	return bswap_64(x);
}
#endif
#ifndef htonll
static inline uint64_t htonll(uint64_t x) {
	return bswap_64(x);
}
#endif
#elif __BYTE_ORDER == __BIG_ENDIAN
#ifndef ntohll
static inline uint64_t ntohll(uint64_t x) {
	return x;
}
#endif
#ifndef htonll
static inline uint64_t htonll(uint64_t x) {
	return x;
}
#endif
#endif	/* __BYTE_ORDER == __BIG_ENDIAN */

/*****************************
 * COMMON MACHINE INDEPENDENT
 */

/* Misc. macros: */
/** align value \a l to \a size (ceil) */
#define ALIGN(l, size) (((l) + ((size) - 1)) / (size) * (size))

/** align value \a l to \a sizeof 32 bit int (ceil) */
#define ALIGN32(l) (ALIGN((l), sizeof(uint32)))

/** printf style debugging MACRO, conmmon header includes name of function */
#define IBWARN(fmt, args...)	ibwarn(__FUNCTION__, fmt, ## args)

/** printf style debugging MACRO, conmmon header includes name of function */
#define LOG(fmt, args...)	logmsg(__FUNCTION__, fmt, ## args)

/** printf style abort MACRO, common header includes name of function */
#define IBPANIC(fmt, args...)	ibpanic(__FUNCTION__, fmt, ## args)

/** abort program if expression \a x is \b false */
#define SANITY(x)		if (common.sanity && !(x))\
					ibpanic(__FUNCTION__,\
					"sanity check <%s> failed: line %d",\
					(x), __LINE__)

/** avoid unused compilation warning  */
#ifndef USED
#define USED(x)	while(0) {void *v = &(x); printf("%p", v);}
#endif

/** define index macro for string array generated by enumstr.awk */
#define ENUM_STR_DEF(enumname, last, val) 	(((unsigned)(val) < last) ? enumname ## _str[val] : "???")
#define ENUM_STR_ARRAY(name)		char * name ## _str[]

#ifdef __GNUC__
#define IBCOMMON_STRICT_FORMAT __attribute__((format(printf, 2, 3)))
#else
#define IBCOMMON_STRICT_FORMAT
#endif

/* util.c: debugging and tracing */
void	ibwarn(const char * const fn, char *msg, ...) IBCOMMON_STRICT_FORMAT;
void	ibpanic(const char * const fn, char *msg, ...) IBCOMMON_STRICT_FORMAT;
void	logmsg(const char *const fn, char *msg, ...) IBCOMMON_STRICT_FORMAT;

void	xdump(FILE *file, char *msg, void *p, int size);

/* sysfs.c: /sys utilities */
int	sys_read_string(char *dir_name, char *file_name, char *str, int max_len);
int	sys_read_guid(char *dir_name, char *file_name, uint64_t *net_guid);
int	sys_read_gid(char *dir_name, char *file_name, uint8_t *gid);
int	sys_read_uint64(char *dir_name, char *file_name, uint64_t *u);
int	sys_read_uint(char *dir_name, char *file_name, unsigned *u);

/* stack.c */
void	stack_dump(void);
void	enable_stack_dump(int loop);

/* time.c */
uint64_t getcurrenttime(void);

/* hash.c */
uint32_t fhash(uint8_t *k, int length, uint32_t initval);

END_C_DECLS

#endif /* __COMMON_H__ */
