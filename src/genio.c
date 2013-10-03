/*
	Solomoriah's WAR!

	genio.c -- generic display i/o driver

    Copyright 1993, 1994, 2001, 2013 Chris Gonnerman
    All rights reserved.

    3 Clause BSD License

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    Redistributions of source code must retain the above copyright
    notice, self list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright
    notice, self list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    Neither the name of the author nor the names of any contributors
    may be used to endorse or promote products derived from self software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "genio.h"

#ifdef CURSES_DRIVER
#include "cursdrv.h"
#endif

#ifdef ANSI_DRIVER
#include "ansidrv.h"
#endif

#ifdef WARTERM_DRIVER
#include "wtermdrv.h"
#endif

struct genio_driver drivers[] = {

#ifdef CURSES_DRIVER
/* curses driver */
    {
        "curses", 
        curs_initscr, curs_endwin,   curs_move, 
        curs_erase,   curs_clrtoeol, curs_addch, 
        curs_hilite,  curs_refresh,  curs_getch,
        NULL, NULL
    },
#endif

#ifdef ANSI_DRIVER
/* curses driver */
    {
        "ansi", 
        ansi_initscr, ansi_endwin,   ansi_move, 
        ansi_erase,   ansi_clrtoeol, ansi_putch, 
        ansi_hilite,  ansi_refresh,  ansi_getch,
        NULL, NULL
    },
#endif

#ifdef WARTERM_DRIVER
/* warterm driver */
    {
        "warterm", 
        warterm_initscr, warterm_endwin,   warterm_move, 
        warterm_erase,   warterm_clrtoeol, warterm_putch, 
        warterm_hilite,  warterm_refresh,  warterm_getch,
        warterm_puts,    warterm_mapspot
    },
#endif

/* sentinel */
    {
        NULL,
        NULL, NULL, NULL,
        NULL, NULL, NULL,
        NULL, NULL, NULL,
        NULL, NULL
    }
};


static int driver = 0;
static int initialized = 0;


static int gsetdriver(char *dname)
{
    int i;

    if(initialized)
        return -1;

    for(i = 0; drivers[i].term != NULL; i++)
        if(strcmp(dname, drivers[i].term) == 0)
            return (driver = i);

    return (driver = -1);
}


int ginit()
{
    char *p;

    if(initialized)
        return -1;

    p = getenv("GENIODRV");

    if(p != NULL)
        if(gsetdriver(p) == -1)
            return -1;

    initialized = 1;

	return drivers[driver].init();
}


int gend()
{
    if(!initialized)
        return -1;

    initialized = 0;

	return drivers[driver].end();
}


int gmove(r, c)
int r, c;
{
    if(!initialized)
        return -1;

	return drivers[driver].move(r, c);
}


int gerase(int mode)
{
    if(!initialized)
        return -1;

	drivers[driver].erase(mode);
}


int gclrline()
{
    if(!initialized)
        return -1;

	drivers[driver].clrline();
}


int gputs(s)
char *s;
{
    int cnt;

    if(!initialized)
        return -1;

    if(drivers[driver].puts != NULL)
        return drivers[driver].puts(s);

    cnt = 0;

	while(*s)
		cnt += drivers[driver].addch(*s++);

    return cnt;
}


int gputch(int c)
{
    if(!initialized)
        return -1;

	return drivers[driver].addch(c);
}


int ggetch()
{
    if(!initialized)
        return -1;

	return drivers[driver].getch();
}


int gprintf(char *fmt, ...)
{
    char buffer[256];
    va_list argptr;
    int cnt;

    if(!initialized)
        return -1;

    va_start(argptr, fmt);
    cnt = vsprintf(buffer, fmt, argptr);
    va_end(argptr);

    gputs(buffer);

    return cnt;
}


int ghilite(int mode)
{
    if(!initialized)
        return -1;

	return drivers[driver].hilite(mode);
}


int grefresh()
{
    if(!initialized)
        return -1;

	return drivers[driver].refresh();
}


int gmapspot(int r, int c, char terr, char mark, int focus)
{
    if(!initialized)
        return -1;

    if(drivers[driver].mapspot != NULL)
        return drivers[driver].mapspot(r, c, terr, mark, focus);

    if(mark != ' ')
        terr = mark;

    if(terr == '~')
        terr = ' ';

    gmove(r + 1, c * 2 + 3);
    gputch(terr);

    gmove(r + 1, c * 2 + 3);

    return 0;
}


/* end of file. */
