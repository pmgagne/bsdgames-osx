#	@(#)Makefile	8.1 (Berkeley) 5/31/93
# $FreeBSD: src/games/hack/Makefile,v 1.20.2.4 2002/08/07 16:31:41 ru Exp $

.PATH: ${.CURDIR}/..

PROG=	hack
SRCS=	alloc.c hack.Decl.c hack.apply.c hack.bones.c hack.c hack.cmd.c \
	hack.do.c hack.do_name.c hack.do_wear.c hack.dog.c hack.eat.c \
	hack.end.c hack.engrave.c hack.fight.c hack.invent.c hack.ioctl.c \
	hack.lev.c hack.main.c hack.makemon.c hack.mhitu.c hack.mklev.c \
	hack.mkmaze.c hack.mkobj.c hack.mkshop.c hack.mon.c hack.monst.c \
	hack.o_init.c hack.objnam.c hack.options.c hack.pager.c hack.potion.c \
	hack.pri.c hack.read.c hack.rip.c hack.rumors.c hack.save.c \
	hack.search.c hack.shk.c hack.shknam.c hack.steal.c hack.termcap.c \
	hack.timeout.c hack.topl.c hack.track.c hack.trap.c hack.tty.c \
	hack.u_init.c hack.unix.c hack.vault.c hack.version.c hack.wield.c \
	hack.wizard.c hack.worm.c hack.worn.c hack.zap.c rnd.c \
	hack.onames.h
MAN=	hack.6
VARGAMES=
GAMESCURSES=

NO_STRICT_ALIASING=
CFLAGS+= 	-I${.CURDIR}/.. -I.

FILES=		rumors help hh data
FILESMODE_rumors=	440
FILESGRP=	${BINGRP}
#FILESDIR=	/var/games/hackdir
CLEANFILES=	hack.onames.h

hack.onames.h: def.objects.h makedefs.c
	${.OBJDIR}/../makedefs/makedefs ${.CURDIR}/../def.objects.h > hack.onames.h

beforeinstall:
	${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 660 /dev/null \
	    ${VARDIR}/hackdir/perm
.if !exists(${VARDIR}/hackdir/record)
	${INSTALL} -o ${BINOWN} -g ${BINGRP} -m 660 /dev/null \
	    ${VARDIR}/hackdir/record
.endif

#	rm -f ${VARDIR}/hackdir/bones* \
#	    ${VARDIR}/hackdir/save/*

.include <bsd.prog.mk>
