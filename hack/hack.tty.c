/*-
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)hack.tty.c	8.1 (Berkeley) 5/31/93
 * $FreeBSD: src/games/hack/hack.tty.c,v 1.6.2.1 2000/07/20 10:35:07 kris Exp $
 * $DragonFly: src/games/hack/hack.tty.c,v 1.4 2006/08/21 19:45:32 pavalos Exp $
 */

/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* hack.tty.c - version 1.0.3 */
/*
 * With thanks to the people who sent code for SYSV - hpscdi!jon,
 * arnold@ucsf-cgl, wcs@bo95b, cbcephus!pds and others.
 */

#include <termios.h>
#include "hack.h"

typedef unsigned long u_long;

/*
 * Some systems may have getchar() return EOF for various reasons, and
 * we should not quit before seeing at least NR_OF_EOFS consecutive EOFs.
 * FIXME: is this still valid nowadays?
 */
#define	NR_OF_EOFS	20

static char erase_char, kill_char;
static boolean settty_needed = FALSE;
struct termios inittyb, curttyb;

static void setctty(void);

/*
 * Get initial state of terminal, set ospeed (for termcap routines)
 * and switch off tab expansion if necessary.
 * Called by startup() in termcap.c and after returning from ! or ^Z
 */
void
gettty(void)
{
	if (tcgetattr(fileno(stdin), &inittyb) < 0)
		perror("Hack (gettty)");
	curttyb = inittyb;
	erase_char = inittyb.c_cc[VERASE];
	kill_char = inittyb.c_cc[VKILL];
	getioctls();

	/* do not expand tabs - they might be needed inside a cm sequence */
	if (curttyb.c_oflag & OXTABS) {
		curttyb.c_oflag &= ~OXTABS;
		setctty();
	}
	settty_needed = TRUE;
}

/* reset terminal to original state */
void
settty(const char *s)
{
	clear_screen();
	end_screen();
	if (s)
		printf("%s", s);
	fflush(stdout);
	if (tcsetattr(fileno(stdin), TCSANOW, &inittyb) < 0)
		perror("Hack (settty)");
	flags.echo = (inittyb.c_lflag & ECHO) ? ON : OFF;
	flags.cbreak = (inittyb.c_lflag & ICANON) ? OFF : ON;
	setioctls();
}

static void
setctty(void)
{
	if (tcsetattr(fileno(stdin), TCSANOW, &curttyb) < 0)
		perror("Hack (setctty)");
}

void
setftty(void)
{
	u_long ef = 0;		/* desired value of flags & ECHO */
	u_long cf = !(ICANON);	/* desired value of flags & CBREAK */
	int change = 0;

	flags.cbreak = ON;
	flags.echo = OFF;
	/* Should use (ECHO|CRMOD) here instead of ECHO */
	if ((curttyb.c_lflag & ECHO) != ef) {
		curttyb.c_lflag &= ~ECHO;
		change++;
	}
	if ((curttyb.c_lflag & ICANON) != cf) {
		curttyb.c_lflag &= ~ICANON;
		curttyb.c_lflag |= cf;
		/* be satisfied with one character; no timeout */
		curttyb.c_cc[VMIN] = 1;		/* was VEOF */
		curttyb.c_cc[VTIME] = 0;	/* was VEOL */
		change++;
	}
	if (change)
		setctty();
	start_screen();
}

/* fatal error */
/* VARARGS1 */
void
error(const char *s, ...)
{
	va_list ap;

	if (settty_needed)
		settty(NULL);
	va_start(ap, s);
	vprintf(s, ap);
	va_end(ap);
	putchar('\n');
	exit(1);
}

/*
 * Read a line closed with '\n' into the array char bufp[BUFSZ].
 * (The '\n' is not stored. The string is closed with a '\0'.)
 * Reading can be interrupted by an escape ('\033') - now the
 * resulting string is "\033".
 */
void
getlin(char *bufp)
{
	char *obufp = bufp;
	int c;

	flags.toplin = 2;	/* nonempty, no --More-- required */
	for (;;) {
		fflush(stdout);
		if ((c = getchar()) == EOF) {
			*bufp = 0;
			return;
		}
		if (c == '\033') {
			*obufp = c;
			obufp[1] = 0;
			return;
		}
		if (c == erase_char || c == '\b') {
			if (bufp != obufp) {
				bufp--;
				putstr("\b \b");	/* putsym converts \b */
			} else
				bell();
		} else if (c == '\n') {
			*bufp = 0;
			return;
		} else if (' ' <= c && c < '\177') {
			/*
			 * avoid isprint() - some people don't have it ' ' is
			 * not always a printing char
			 */
			*bufp = c;
			bufp[1] = 0;
			putstr(bufp);
			if (bufp - obufp < BUFSZ - 1 && bufp - obufp < COLNO)
				bufp++;
		} else if (c == kill_char || c == '\177') {	/* Robert Viduya */
			/* this test last - @ might be the kill_char */
			while (bufp != obufp) {
				bufp--;
				putstr("\b \b");
			}
		} else
			bell();
	}
}

void
getret(void)
{
	cgetret("");
}

void
cgetret(const char *s)
{
	putsym('\n');
	if (flags.standout)
		standoutbeg();
	putstr("Hit ");
	putstr(flags.cbreak ? "space" : "return");
	putstr(" to continue: ");
	if (flags.standout)
		standoutend();
	xwaitforspace(s);
}

char morc;		/* tell the outside world what char he used */

void
xwaitforspace(const char *s)	/* chars allowed besides space or return */
{
	int c;

	morc = 0;
	while ((c = readchar()) != '\n') {
		if (flags.cbreak) {
			if (c == ' ')
				break;
			if (s && strchr(s, c)) {
				morc = c;
				break;
			}
			bell();
		}
	}
}

char *
parse(void)
{
	static char inputline[COLNO];
	int foo;

	flags.move = 1;
	if (!Invisible)
		curs_on_u();
	else
		home();
	while ((foo = readchar()) >= '0' && foo <= '9')
		multi = 10 * multi + foo - '0';
	if (multi) {
		multi--;
		save_cm = inputline;
	}
	inputline[0] = foo;
	inputline[1] = 0;
	if (foo == 'f' || foo == 'F') {
		inputline[1] = getchar();
#ifdef QUEST
		if (inputline[1] == foo)
			inputline[2] = getchar();
		else
#endif /* QUEST */
			inputline[2] = 0;
	}
	if (foo == 'm' || foo == 'M') {
		inputline[1] = getchar();
		inputline[2] = 0;
	}
	clrlin();
	return (inputline);
}

char
readchar(void)
{
	int sym;

	fflush(stdout);
	if ((sym = getchar()) == EOF)
#ifdef NR_OF_EOFS
	{ /*
	   * Some SYSV systems seem to return EOFs for various reasons
	   * (?like when one hits break or for interrupted systemcalls?),
	   * and we must see several before we quit.
	   */
		int cnt = NR_OF_EOFS;
		while (cnt--) {
			clearerr(stdin); /* omit if clearerr is undefined */
			if ((sym = getchar()) != EOF)
				goto noteof;
		}
		end_of_input();
noteof:;
	}
#else
		end_of_input();
#endif /* NR_OF_EOFS */
	if (flags.toplin == 1)
		flags.toplin = 2;
	return ((char)sym);
}

void
end_of_input(void)
{
	settty("End of input?\n");
	clearlocks();
	exit(0);
}
