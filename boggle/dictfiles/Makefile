.PATH: ${.CURDIR}/../boggle

# XXX better word list selection
.if exists(${.CURDIR}/../../../share/dict/web2)
BOOGLEWORDS?=	${.CURDIR}/../../../share/dict/web2
.else
BOOGLEWORDS?=	/usr/share/dict/words
.endif

FILES=	dictindex dictionary helpfile
FILESDIR=	${SHAREDIR}/games/boggle

CLEANFILES=	dictindex dictionary

dictionary:
	${.OBJDIR}/../mkdict/mkdict < ${BOOGLEWORDS} > ${.TARGET}

dictindex: dictionary
	${.OBJDIR}/../mkindex/mkindex < ${.ALLSRC} > ${.TARGET}

beforeinstall:
.if !exists(${FILESDIR})
	mkdir -p ${FILESDIR}
.endif


.if exists(${.CURDIR}/../../Makefile.inc)
.include "${.CURDIR}/../../Makefile.inc"
.endif

.include <bsd.prog.mk>
