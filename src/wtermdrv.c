/*
	Solomoriah's WAR!

	wtermdrv.c -- warterm driver for genio

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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "genio.h"


static char *charesc(int ch)
{
    static char buf[128];

    if(ch > 127)
        return "";

    if(ch == '\n')
        return "\02";

    if(ch == '\b' || ch >= ' ') {
        buf[1] = '\0';
        buf[0] = ch;
        return buf;
    } 

    return "";
}


int warterm_erase()
{
    puts("C\01");

    return 0;
}


int warterm_move(int r, int c)
{
    printf("@\01%d\01%d\01\n", r, c);

    return 0;
}


int warterm_initscr()
{
    puts("XYZZY");

    return 0;
}


int warterm_endwin()
{
    warterm_erase();
	warterm_move(0, 29);

    return 0;
}


int warterm_clrtoeol()
{
    puts("E\01");

    return 0;
}


int warterm_putch(int c)
{
    printf("P\01%s\01\n", charesc(c));

    return 0;
}


int warterm_puts(char *s)
{
    char buf[1024];

    strcpy(buf, "P\01");

    for(; *s; s++)
        strcat(buf, charesc(*s));

    strcat(buf, "\01");

    puts(buf);

    return 0;
}


int warterm_hilite(int mode)
{
    printf("H\01%d\01\n", mode);

    return mode;
}


int warterm_refresh()
{
    puts("R\01");

    return 0;
}


int warterm_mapspot(int r, int c, char terr, char mark, int focus)
{
    printf("M\01%d\01%d\01%c\01%c\01%d\01\n", r, c, terr, mark, focus);

    return 0;
}


int warterm_getch()
{
    int rc;

    char inbuf[1024];

    inbuf[1000] = '\0';

    fgets(inbuf, 1000, stdin);

    rc = atoi(inbuf);

    return rc;
}


/* end of file. */
