#	@(#)Makefile	8.1 (Berkeley) 5/31/93
# $FreeBSD: src/games/canfield/canfield/Makefile,v 1.5.2.3 2002/08/07 16:31:41 ru Exp $

PROG=	canfield
MAN=	canfield.6
MLINKS=	canfield.6 cfscores.6
VARGAMES=
GAMESCURSES=

beforeinstall:
.if !exists(${VARDIR}/cfscores)
	${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 664 /dev/null \
	    ${VARDIR}/cfscores
.endif

.include "../../Makefile.inc"
.include <bsd.prog.mk>
