/*
	Solomoriah's WAR!

	cursdrv.c -- curses driver for genio

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


#include <curses.h>
#include "genio.h"


int curs_hilite(int mode)
{
    if(mode)
        standout();
    else
        standend();
}


int curs_initscr()
{
    initscr();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr,FALSE);
    keypad(stdscr,TRUE);
}


int curs_endwin()
{
    endwin();
}


int curs_move(int r, int c)
{
    move(r, c);
}


int curs_erase(int mode)
{
    if(mode)
        clear();
    else
        erase();
}


int curs_clrtoeol()
{
    clrtoeol();
}


int curs_addch(int ch)
{
    addch(ch);
}


int curs_refresh()
{
    refresh();
}


int curs_getch()
{
    int rc;

    rc = getch();

    switch(rc) {
    case KEY_HOME:
        rc = GKEY_HOME;
        break;
    case KEY_END:
        rc = GKEY_END;
        break;
    case KEY_LEFT:
        rc = GKEY_LEFT;
        break;
    case KEY_RIGHT:
        rc = GKEY_RIGHT;
        break;
    case KEY_UP:
        rc = GKEY_UP;
        break;
    case KEY_DOWN:
        rc = GKEY_DOWN;
        break;
    case KEY_NPAGE:
        rc = GKEY_PAGEDOWN;
        break;
    case KEY_PPAGE:
        rc = GKEY_PAGEUP;
        break;
    case KEY_BACKSPACE:
    case KEY_DC:
        rc = '\b';
        break;
    }

    return rc;
}


/* end of file. */
