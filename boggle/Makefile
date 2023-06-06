#	$OpenBSD: Makefile,v 1.4 2003/04/10 22:42:29 millert Exp $

SUBDIR=		boggle

# Only if building the dictionary
.if !defined(NO_SHARE)
SUBDIR+=	mkdict mkindex dictfiles
.endif

beforeinstall:
.if !exists(${BINDIR})
	mkdir -p ${BINDIR}
.endif
.if !exists(${VARDIR})
	mkdir -p ${VARDIR}
.endif
.if !exists(${MANDIR}6)
	mkdir -p ${MANDIR}6
.endif

.include <bsd.subdir.mk>
