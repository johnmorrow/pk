# $FreeBSD$

PORTNAME=		pk
PORTVERSION=	1.0.2
CATEGORIES=		textproc
MASTER_SITES=	https://github.com/johnmorrow/pk/releases/download/v1.0.2/pk-1.0.2.tar.gz

MAINTAINER=		jmorrow@amoeba.org
COMMENT=		A field extractor command line utility

MAN1=			pk.1
MANCOMPRESSED=	no

LICENSE=		AL2

GNU_CONFIGURE=	yes
LIB_DEPENDS=	argp:${PORTSDIR}/devel/argp-standalone \
				execinfo:${PORTSDIR}/devel/libexecinfo
PLIST_FILES=	bin/pk
MANDIR=			${PREFIX}/man
CFLAGS+=		-I${PREFIX}/include -D__BSD_VISIBLE
LDFLAGS+=		-L${PREFIX}/lib

.include <bsd.port.mk>
