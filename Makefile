# $FreeBSD$

.include <bsd.own.mk>

PROG=	hastd
SRCS=	activemap.c
SRCS+=	control.c crc32.c
SRCS+=	ebuf.c event.c
SRCS+=	hast_checksum.c hast_compression.c hast_proto.c hastd.c hooks.c
SRCS+=	lzf.c
SRCS+=	metadata.c
SRCS+=	nv.c
SRCS+=	secondary.c
SRCS+=	parse.y pjdlog.c primary.c
SRCS+=	proto.c proto_common.c proto_socketpair.c proto_tcp.c proto_uds.c
SRCS+=	rangelock.c
SRCS+=	subr.c
SRCS+=	token.l
SRCS+=	y.tab.h
MAN=	hastd.8 hast.conf.5

NO_WFORMAT=
CFLAGS+=-DPROTO_TCP_DEFAULT_PORT=8457
CFLAGS+=-I${.CURDIR}
CFLAGS+=-DINET
.if ${MK_INET6_SUPPORT} != "no"
CFLAGS+=-DINET6
.endif
# This is needed to have WARNS > 1.
CFLAGS+=-DYY_NO_UNPUT
CFLAGS+=-DYY_NO_INPUT

DPADD=	${LIBGEOM} ${LIBBSDXML} ${LIBSBUF} ${LIBL} ${LIBPTHREAD} ${LIBUTIL}
LDADD=	-lgeom -lbsdxml -lsbuf -ll -lpthread -lutil
.if ${MK_OPENSSL} != "no"
DPADD+=	${LIBCRYPTO}
LDADD+=	-lcrypto
CFLAGS+=-DHAVE_CRYPTO
.endif

YFLAGS+=-v

CLEANFILES=y.tab.c y.tab.h y.output

.include <bsd.prog.mk>
