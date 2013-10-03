/*
	Solomoriah's WAR!

	ansidrv.c -- ansi driver for genio

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
#include <termio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "genio.h"

#define ATTR_NORMAL     0
#define ATTR_REVERSE    7

struct cell {
	char ch, attr;
};

static struct cell ascreen[24][80];
static struct cell vscreen[24][80];

static int cur_x, cur_y;
static unsigned char cur_attr;

static ansi_goto(int x, int y);
static ansi_attr(int attr);

static struct termio term, save;

struct node {
    char ch;
    int result;
    struct node *match, *nomatch;
};

static struct node *keys = NULL;

static struct {
    char *match;
    int result;
} keymap[] = {
    { "[A",  GKEY_UP },
    { "[B",  GKEY_DOWN },
    { "[C",  GKEY_RIGHT },
    { "[D",  GKEY_LEFT },
    { "OA",  GKEY_UP },
    { "OB",  GKEY_DOWN },
    { "OC",  GKEY_RIGHT },
    { "OD",  GKEY_LEFT },
    { "[3~", GKEY_PAGEUP },
    { "[6~", GKEY_PAGEDOWN },
    { "[2~", GKEY_HOME },
    { "[5~", GKEY_END },
    { NULL, 0 }
};


struct node *make_node()
{
    struct node *n;

    n = malloc(sizeof(struct node));
    memset(n, 0, sizeof(struct node));

    return n;
}


addkey(char *match, int result)
{
	int i, len;
	struct node *k, *pk;

    len = strlen(match);

	k = pk = keys;

	for(i = 0; i < len; i++, pk = k, k = k->match) {

		if(k == NULL) {
			k = make_node();
			k->ch = match[i];

			if(keys == NULL)
				keys = k;
			else
				pk->match = k;
		} else {
			for(; k->ch != match[i] && k->nomatch != NULL; k = k->nomatch);

			if(k->ch != match[i]) {
				k->nomatch = make_node();
				k = k->nomatch;
				k->ch = match[i];
			}
		}
	}

	pk->result = result;
}


int ansi_initscr()
{
    int i;

    if(keys == NULL)
        for(i = 0; keymap[i].match != NULL; i++)
            addkey(keymap[i].match, keymap[i].result);

	cur_attr = ATTR_NORMAL;
	setbuf(stdin, NULL);
	ansi_erase();

	ioctl(0, TCGETA, &save);

	term = save;

	term.c_cc[VERASE] = '\377';
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 6;
	term.c_iflag = term.c_iflag & (~ICRNL);
	term.c_lflag = 0;

    return 0;
}


int ansi_endwin()
{
	ansi_refresh();
	ansi_goto(0, 23);

    return 0;
}


int ansi_move(r, c)
int r, c;
{
	cur_y = r;
	cur_x = c;

    return 0;
}


int ansi_erase()
{
	ansi_attr(cur_attr);

	fputs("\033[H\033[2J", stdout);
	fflush(stdout);

	for(cur_x = 0; cur_x < 80; cur_x++)
		for(cur_y = 0; cur_y < 24; cur_y++) {
			ascreen[cur_y][cur_x].ch = ' ';
			ascreen[cur_y][cur_x].attr = cur_attr;
			vscreen[cur_y][cur_x].ch = ' ';
			vscreen[cur_y][cur_x].attr = cur_attr;
		}

	cur_x = 0;
	cur_y = 0;

    return 0;
}


int ansi_clrtoeol()
{
	int cx;

	for(cx = cur_x; cx < 80; cx++) {
		vscreen[cur_y][cx].ch = ' ';
		vscreen[cur_y][cx].attr = cur_attr;
	}

    return 0;
}


int ansi_putch(int c)
{
	unsigned char cc;

	cc = c;

	switch(cc) {
		case '\n' :
			cur_x = 0;
			if(++cur_y >= 24)
				cur_y = 23;
			break;
		case '\r' :
			cur_x = 0;
			break;
		case '\b' :
			cur_x--;
            if(cur_x < 0)
                cur_x = 0;
			break;
		default :
			if(cc >= ' ') {
				vscreen[cur_y][cur_x].ch = cc;
				vscreen[cur_y][cur_x].attr = cur_attr;
				if(++cur_x >= 80) {
					cur_x = 0;
					if(++cur_y >= 24)
						cur_y = 23;
				}
			}
			break;
	}

    return 0;
}


int ansi_hilite(int mode)
{
	cur_attr = (mode == 0 ? ATTR_NORMAL : ATTR_REVERSE);

    return mode;
}


static ansi_goto(int x, int y)
{
	printf("\033[%d;%dH", y+1, x+1);
}


static ansi_attr(int attr)
{
	printf("\033[%dm", attr);
}


int ansi_refresh()
{
	int rx, ry, ra, px, py;

	ra = vscreen[0][0].attr;
	ansi_attr(ra);
	ansi_goto(0, 0);
	px = py = 0;

	for(ry = 0; ry < 24; ry++)
		for(rx = 0; rx < 80; rx++)
			if(vscreen[ry][rx].ch != ascreen[ry][rx].ch
			|| vscreen[ry][rx].attr != ascreen[ry][rx].attr) {
				/* if needed, move cursor */
				if(px != rx || py != ry)
					ansi_goto(rx, ry);
				px = rx;
				py = ry;
				/* correct attribute */
				if(ra != vscreen[ry][rx].attr) {
					ra = vscreen[ry][rx].attr;
					ansi_attr(ra);
				}
				/* rewrite */
				putchar(vscreen[ry][rx].ch);
				if(++px >= 80) {
					px = 0;
					py++;
				}
				ascreen[ry][rx].ch = vscreen[ry][rx].ch;
				ascreen[ry][rx].attr = vscreen[ry][rx].attr;
			}

	ansi_goto(cur_x, cur_y);

	fflush(stdout);

    return 0;
}


static int isinput()
{
    fd_set in_set;
    struct timeval timeout;
    int rc;

    FD_ZERO(&in_set);
    FD_SET(0, &in_set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 200;

    return select(1, &in_set, NULL, NULL, &timeout);
}


static int get_key()
{
	struct node *k;
	int ch;

	k = keys;

	while(1) {
		ch = getchar();

		if(ch < 0)
			return ch;

		for(; k != NULL && k->ch != ch; k = k->nomatch);

		if(k == NULL)
			return ch;

		if(k->match == NULL)
			return k->result;
		else
			k = k->match;
	}
}



int ansi_getch()
{
    int ch;
    struct node *k;

    if(isinput() < 1)
        ansi_refresh();

    /* set terminal in raw mode */

	ioctl(0, TCSETA, &term);

    /* get the character */

    ch = getchar();

    if(ch == '\033')
        if(isinput() > 0)
            ch = get_key();

    /* reset terminal */

	ioctl(0, TCSETA, &save);

    /* return character */

    return ch;
}


/* end of file. */
