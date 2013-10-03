/*
    Solomoriah's WAR!

    dispsupp.c -- support functions for display handlers

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
#include <string.h>
#include <ctype.h>

#include "war.h"
#include "warext.h"
#include "genio.h"
#include "function.h"


int fixrow(r)
int r;
{
    r += map_height;
    r %= map_height;

    return r;
}


int fixcol(c)
int c;
{
    c += map_width;
    c %= map_width;

    return c;
}


saystat(msg)
char *msg;
{
    gmove(20, 1);
    gputs(msg);
    gclrline();
}


clearstat(mode)
int mode;
{
    if(mode == -1) {
        gmove(20, 1); gclrline();
        gmove(21, 1); gclrline();
        gmove(22, 1); gclrline();
        gmove(23, 1); gclrline();
    } else {
        gmove(20+mode, 1); gclrline();
    }
}


/*
    groupcmp() -- if r1:c1 > r2:c2, returns 1
                if r1:c1 < r2:c2, returns -1
                otherwise returns 0
*/

int groupcmp(r1, c1, r2, c2)
int r1, c1, r2, c2;
{
    /* quadrant check */

    if(r1 / 16 > r2 / 16)
        return 1;

    if(r1 / 16 < r2 / 16)
        return -1;

    if(c1 / 16 > c2 / 16)
        return 1;

    if(c1 / 16 < c2 / 16)
        return -1;

    /* exact check */

    if(r1 > r2)
        return 1;

    if(r1 < r2)
        return -1;

    if(c1 > c2)
        return 1;

    if(c1 < c2)
        return -1;

    return 0;
}


int nextarmy(int ntn, int *rp, int *cp)
{
    int i, t_r, t_c;

    t_r = -1;
    t_c = -1;

    for(i = 0; i < armycnt; i++)
        if(armies[i].nation == ntn && armies[i].move_left > 0)
            if(groupcmp(armies[i].r, armies[i].c, *rp, *cp) > 0
            && (t_r == -1
            || groupcmp(armies[i].r, armies[i].c, t_r, t_c) < 0))
                t_r = armies[i].r, t_c = armies[i].c;

    if(t_r != -1) {
        *rp = t_r;
        *cp = t_c;
        return 1;
    } else
        return 0;
}


int nextgroup(int ntn, int *rp, int *cp)
{
    int i, t_r, t_c;

    t_r = -1;
    t_c = -1;

    for(i = 0; i < citycnt; i++)
        if(cities[i].nation == ntn)
            if(groupcmp(cities[i].r, cities[i].c, *rp, *cp) > 0
            && (t_r == -1
            || groupcmp(cities[i].r, cities[i].c, t_r, t_c) < 0))
                t_r = cities[i].r, t_c = cities[i].c;

    for(i = 0; i < armycnt; i++)
        if(armies[i].nation == ntn)
            if(groupcmp(armies[i].r, armies[i].c, *rp, *cp) > 0
            && (t_r == -1
            || groupcmp(armies[i].r, armies[i].c, t_r, t_c) < 0))
                t_r = armies[i].r, t_c = armies[i].c;

    if(t_r != -1) {
        *rp = t_r;
        *cp = t_c;
        return 1;
    } else
        return 0;
}


int prevarmy(int ntn, int *rp, int *cp)
{
    int i, t_r, t_c;

    t_r = -1;
    t_c = -1;

    for(i = 0; i < armycnt; i++)
        if(armies[i].nation == ntn && armies[i].move_left > 0)
            if(groupcmp(armies[i].r, armies[i].c, *rp, *cp) < 0
            && (t_r == -1
            || groupcmp(armies[i].r, armies[i].c, t_r, t_c) > 0))
                t_r = armies[i].r, t_c = armies[i].c;

    if(t_r != -1) {
        *rp = t_r;
        *cp = t_c;
        return 1;
    } else
        return 0;
}


int prevgroup(ntn, rp, cp)
int ntn, *rp, *cp;
{
    int i, t_r, t_c;

    t_r = -1;
    t_c = -1;

    for(i = 0; i < citycnt; i++)
        if(cities[i].nation == ntn)
            if(groupcmp(cities[i].r, cities[i].c, *rp, *cp) < 0
            && (t_r == -1
            || groupcmp(cities[i].r, cities[i].c, t_r, t_c) > 0))
                t_r = cities[i].r, t_c = cities[i].c;

    for(i = 0; i < armycnt; i++)
        if(armies[i].nation == ntn)
            if(groupcmp(armies[i].r, armies[i].c, *rp, *cp) < 0
            && (t_r == -1
            || groupcmp(armies[i].r, armies[i].c, t_r, t_c) > 0))
                t_r = armies[i].r, t_c = armies[i].c;

    if(t_r != -1) {
        *rp = t_r;
        *cp = t_c;
        return 1;
    } else
        return 0;
}


int movecost(int a, int r, int c)
{
    int t, tbl, mv, city, cost, a2;
    char *p;

    city = city_at(r, c);

    if(city == -1) {
        /* not a city */

        /* search for a TRANS_ALL unit there */
        if(!(armies[a].r == r && armies[a].c == c)
        && armies[a].special_mv != TRANS_ALL)
            for(a2 = 0; a2 < armycnt; a2++)
                if(a != a2
                && armies[a2].r == r
                && armies[a2].c == c
                && armies[a2].nation == armies[a].nation
                && armies[a2].special_mv == TRANS_ALL)
                    return 1;

        /* else do this: */
        for(t = 0; terrain[t]; t++)
            if(terrain[t] == map[r][c])
                break;
        if(terrain[t] != '\0') {
            tbl = armies[a].move_tbl;
            mv = move_table[tbl].cost[t];
            
            if(mv == 0
                /* otherwise can't move, and */
            && map[r][c] != '~' 
                /* target space is not water, and */
            && map[armies[a].r][armies[a].c] == '~'
                /* current space is water, and */
            && (cost = move_table[tbl].cost[0]) > 0)
                /* this unit moves on water, then */
                mv = cost * 2; /* beaching is allowed */
        } else
            switch(map[r][c]) {
            case '+' : /* road */
                mv = 1;
                break;
            default : /* should not happen */
                mv = 3;
                break;
            }

    } else {

        /* city */

        if(armies[a].nation == cities[city].nation)
            mv = 1;
        else
            mv = 2;
    }

    return mv;
}


status()
{
    int ch, pos, i, j, hc, ac, cc;

    /* set up screen */

    gerase(0);

    gmove(1, 20);
    gputs("Nation Status Display");

    gmove(3, 1);    gputs("Nation:");
    gmove(4, 1);    gputs("Ruler:");
    gmove(5, 1);    gputs("Mark:");
    gmove(7, 1);    gputs("Cities:");
    gmove(8, 1);    gputs("Heros:");
    gmove(9, 1);    gputs("Armies:");

    gmove(11, 1);   gputs("(n)ext page  (p)revious page  (SPACE) to exit");

    /* display loop */

    pos = 0;

    do {
        /* show the nations. */

        for(i = 3; i <= 10; i++) {
            gmove(i, 16);
            gclrline();
        }

        for(i = 0; i < 4; i++)
            if(pos + 1 + i < nationcnt) {
                gmove(3, i * 16 + 16);
                gputs(nationcity(pos+1+i));

                gmove(4, i * 16 + 16);
                gputs(nations[pos+1+i].name);

                gmove(5, i * 16 + 19);
                gputch(marks[0][nations[pos+1+i].mark]);

                cc = 0;

                for(j = 0; j < citycnt; j++)
                    if(cities[j].nation == pos + 1 + i)
                        cc++;

                hc = 0;
                ac = 0;

                for(j = 0; j < armycnt; j++)
                    if(armies[j].nation == pos + 1 + i)
                        if(armies[j].hero > 0)
                            hc++;
                        else
                            ac++;

                gmove(7, i * 16 + 16);
                gprintf("%5d", cc);

                gmove(8, i * 16 + 16);
                gprintf("%5d", hc);

                gmove(9, i * 16 + 16);
                gprintf("%5d", ac);
            }

        /* handle input. */

        gmove(11, 70);
        gputs("< >\b\b");

        switch(ch = ggetch()) {

        case ' ' :
        case 'q' :
            ch = '\033';
            break;

        case 'n' :
            if(pos + 5 < nationcnt)
                pos += 4;
            break;

        case 'p' :
            if(pos - 3 > 0)
                pos -= 4;
            break;
        }

    } while(ch != '\033');

    /* clean up */

    mainscreen();
}


help()
{
    static mode = 0;

    gmove(20, 1);
    gprintf("Commands, Page %d  (Press ? for More Help)", mode + 1);
    gclrline();

    switch(mode) {

    case 0 :
        gmove(21, 1);
        gputs("  (n)ext group  (p)revious group  (N)last (P)first");
        gclrline();

        gmove(22, 1);
        gputs("  (s)tatus display  (i)nformation mode");
        gputs("  (a)rmy production");
        gclrline();

        gmove(23, 1);
        gputs("  (R)ead news  read (M)ail  (S)end mail  (q)uit");
        gclrline();

        break;

    case 1 :
        gmove(21, 1);
        gputs("  (j)pointer down  (k)pointer up  (SPACE)mark  (*)mark all");
        gclrline();

        gmove(22, 1);
        gputs("  (m)ove marked armies  (r)ename hero  (q)uit");
        gclrline();

        gmove(23, 1);
        gclrline();

        break;
    }

    mode++;
    mode %= 2;
}


int getstring(char *s)
{
    int i, ch;

    i = 0;
    s[0] = '\0';

    while((ch = ggetch()) != '\r' && ch != '\n' && ch != '\033') {

        if(isgraph(ch) && ch != ':' && i < 14) {
            s[i++] = ch;
            s[i] = '\0';
            gputch(ch);
        } else if(isspace(ch) && i < 14 && i > 0) {
            s[i++] = ' ';
            s[i] = '\0';
            gputch(' ');
        } else if((ch == '\b' || ch == GKEY_LEFT) && i > 0) {
            s[--i] = '\0';
            gputch('\b');
            gputch(' ');
            gputch('\b');
        }
    }
}


int getpass(char *s)
{
    int i, ch;

    i = 0;
    s[0] = '\0';

    while((ch = ggetch()) != '\r' && ch != '\n' && ch != '\033') {

        if(isgraph(ch) && ch != ':' && i < 14) {
            s[i++] = ch;
            s[i] = '\0';
            gputch('*');
        } else if(isspace(ch) && i < 14 && i > 0) {
            s[i++] = ' ';
            s[i] = '\0';
            gputch('*');
        } else if((ch == '\b' || ch == GKEY_LEFT) && i > 0) {
            s[--i] = '\0';
            gputch('\b');
            gputch(' ');
            gputch('\b');
        }
    }
}


show_info(r, c)
int r, c;
{
    char buff[80], buf2[64];
    int i, ch, city, ntn, ac, hc;

    buff[0] = '\0';
    city = city_at(r, c);

    if(city != -1) {
        sprintf(buff, "City:  %s (%s)", cities[city].name,
            nationcity(cities[city].nation));
    } else {
        ch = map[r][c];

        for(i = 0; terrain[i]; i++)
            if(terrain[i] == ch)
                break;

        if(terrain[i])
            strcpy(buff, terr_names[i]);
        else if(ch == ' ')
            strcpy(buff, "Ocean");
    }

    ac = 0;
    hc = 0;

    for(i = 0; i < armycnt; i++)
        if(armies[i].r == r && armies[i].c == c)
            if(armies[i].hero > 0)
                hc++;
            else
                ac++;

    sprintf(buf2, "  %d Armies, %d Heros", ac, hc);
    strcat(buff, buf2);

    saystat(buff);
}


/* end of file. */
