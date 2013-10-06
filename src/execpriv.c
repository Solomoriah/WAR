/*
    Solomoriah's WAR!

    execpriv.c -- execute command files in priveledged mode

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

#include "war.h"
#include "warext.h"
#include "function.h"
#include "execsupp.h"


#pragma argsused
static int update(int argc, char *argv[])
{
    int i, k, n, t, cnt, xch, max;

    /* pack army list */

    do {
        xch = 0;

        for(k = 0; k < armycnt; k++)
            if(armies[k].nation == -1)
                break;

        if(k < armycnt) {
            for(i = armycnt - 1; i >= k; i--)
                if(armies[i].nation != -1) 
                    break;
                else
                    armycnt--;

            if(i > k) {
                strcpy(armies[k].name, armies[i].name);
                armies[k].nation = armies[i].nation;
                armies[k].r = armies[i].r;
                armies[k].c = armies[i].c;
                armies[k].combat = armies[i].combat;
                armies[k].hero = armies[i].hero;
                armies[k].move_rate = armies[i].move_rate;
                armies[k].move_tbl = armies[i].move_tbl;
                armies[k].special_mv = armies[i].special_mv;
                armies[k].eparm1 = armies[i].eparm1;

                armycnt--;

                xch = 1;
            }
        }
    } while(xch);

    /* create new armies */

    for(i = 0; i < citycnt; i++) {

        cnt = 0;

        for(k = 0; k < armycnt; k++)
            if(armies[k].nation == cities[i].nation
            && armies[k].r == cities[i].r
            && armies[k].c == cities[i].c)
                cnt++;

        max = cities[i].defense * 2;

        if(max > 10)
            max = 10;

        if(cities[i].nation != 0) {
            max = 12;
            k = cities[i].nation;
            if(nations[k].idle_turns >= TIME_TO_DESERT)
                max = 0;
        }

        if(cnt < max) {

            n = cities[i].prod_type;

            if(n >= 0 && n < 4) {
                cities[i].turns_left--;
                if(cities[i].turns_left < 1) {
                    if(armycnt < MAXARMIES - 1) {
                        t = cities[i].types[n];

                        sprintf(armies[armycnt].name,
                            ".%03d/%03d/%03d", i, t,
                            ++cities[i].instance[n]);

                        armies[armycnt].nation = cities[i].nation;
                        armies[armycnt].r = cities[i].r;
                        armies[armycnt].c = cities[i].c;

                        armies[armycnt].combat = ttypes[t].combat;
                        armies[armycnt].hero = 0;

                        armies[armycnt].move_rate = ttypes[t].move_rate;
                        armies[armycnt].move_tbl = ttypes[t].move_tbl;
                        armies[armycnt].special_mv =
                                            ttypes[t].special_mv;
                        armies[armycnt].eparm1 = 0;

                        armycnt++;

                        cities[i].turns_left = cities[i].times[n];

                    } else
                        cities[i].turns_left = 0;
                }
            }
        }
    }

    /* update all armies' movement. */

    for(i = 0; i < armycnt; i++)
        armies[i].move_left = armies[i].move_rate;

    gen++;

    return 0;
}


/*
    new-nation 'ruler' uid city mark
*/
#pragma argsused
static int newnation(int argc, char *argv[])
{
    int i, r, c;

    /* if in war update, add news entry */

    nation_news(argv[1], atoi(argv[3]));

    /* create nation entry */

    strcpy(nations[nationcnt].name, argv[1]);
    nations[nationcnt].uid = atoi(argv[2]);
    nations[nationcnt].city = atoi(argv[3]);
    nations[nationcnt].mark = atoi(argv[4]);

    /* transfer armies */

    r = cities[nations[nationcnt].city].r;
    c = cities[nations[nationcnt].city].c;

    for(i = 0; i < armycnt; i++)
        if(armies[i].r == r
        && armies[i].c == c
        && armies[i].nation == 0)
            armies[i].nation = nationcnt;

    /* clean up */

    nationcnt++;

    return 0;
}


/*
    control-city nation city max-time
*/
#pragma argsused
static int contcity(int argc, char *argv[])
{
    int c;

    cities[c = atoi(argv[2])].nation = atoi(argv[1]);

    if(argv[3] != NULL && atoi(argv[3]) > 0)
        cities[c].turns_left = cities[c].times[cities[c].prod_type] + 1;

    return 0;
}


/*
    make-army 'name' nation row column
            combat hero move_rate move_tbl special_mv eparm1
*/
#pragma argsused
static int makearmy(int argc, char *argv[])
{
    /* no error, just don't do it. */

    if(armycnt >= MAXARMIES)
        return 0;

    strncpy(armies[armycnt].name, argv[1], ARMYNAMLEN);
    armies[armycnt].name[ARMYNAMLEN] = '\0';

    armies[armycnt].nation = atoi(argv[2]);
    armies[armycnt].r = atoi(argv[3]);
    armies[armycnt].c = atoi(argv[4]);
    armies[armycnt].combat = atoi(argv[5]);
    armies[armycnt].hero = atoi(argv[6]);
    armies[armycnt].move_rate = atoi(argv[7]);
    armies[armycnt].move_left = atoi(argv[7]);
    armies[armycnt].move_tbl = atoi(argv[8]);
    armies[armycnt].special_mv = atoi(argv[9]);
    armies[armycnt].eparm1 = atoi(argv[10]);

    armycnt++;

    return 0;
}


/*
    kill-army army              (kill one)
        or
    kill-army -1 nation         (kill all)
        or
    kill-army -2 army nation    (army deserts to nation)
*/
#pragma argsused
static int killarmy(int argc, char *argv[])
{
    int a, i, n;

    a = atoi(argv[1]);

    if(a == -2) {
        i = atoi(argv[2]);
        n = atoi(argv[3]);
        if(n > 0) {
            armies[i].nation = n;
            if(armies[i].hero > 1)
                armies[i].hero--;
            return 0;
        }
        a = i;
    }

    if(a == -1) {
        n = atoi(argv[2]);
        if(n < 1 || n > nationcnt)
            return 0;
        for(i = 0; i < armycnt; i++)
            if(armies[i].nation == n) {
                armies[i].nation = -1;
                armies[i].r = -1;
                armies[i].c = -1;
            }
        return 0;
    }

    armies[a].nation = -1;
    armies[a].r = -1;
    armies[a].c = -1;

    return 0;
}


/*
    move-army army cost row column
*/
#pragma argsused
static int movearmy(int argc, char *argv[])
{
    int a, b;

    a = atoi(argv[1]);

    if(armies[a].nation == -1)
        return 0;

    b = armies[a].move_left;
    b -= atoi(argv[2]);
    if(b < 0)
        b = 0;
    armies[a].move_left = b;
    armies[a].r = atoi(argv[3]);
    armies[a].c = atoi(argv[4]);

    return 0;
}


/*
    name-army army 'name'
*/
#pragma argsused
static int namearmy(int argc, char *argv[])
{
    int a;

    a = atoi(argv[1]);

    if(armies[a].nation == -1)
        return 0;

    strncpy(armies[a].name, argv[2], ARMYNAMLEN);
    armies[a].name[ARMYNAMLEN] = '\0';

    return 0;
}


/*
    change-army army combat hero
*/
#pragma argsused
static int changearmy(int argc, char *argv[])
{
    int a, c, h;

    a = atoi(argv[1]);
    c = atoi(argv[2]);
    h = atoi(argv[3]);

    if(armies[a].nation == -1)
        return 0;

    if(c >= 0) armies[a].combat = c;
    if(h >= 0) armies[a].hero = h;

    return 0;
}


/*
    set-eparm army eparm
*/
#pragma argsused
static int seteparm(int argc, char *argv[])
{
    int a;

    a = atoi(argv[1]);

    if(armies[a].nation == -1)
        return 0;

    armies[a].eparm1 = atoi(argv[2]);

    return 0;
}


/*
    set-produce city type
*/
#pragma argsused
static int setprod(int argc, char *argv[])
{
    int c;

    c = atoi(argv[1]);

    cities[c].prod_type = atoi(argv[2]);
    cities[c].turns_left = cities[c].times[cities[c].prod_type];

    return 0;
}


static struct parse_tbl table[] = {

    "global-update",  update,
    "new-nation",     newnation,
    "control-city",   contcity,

    "kill-army",      killarmy,
    "move-army",      movearmy,
    "make-army",      makearmy,
    "name-army",      namearmy,
    "change-army",    changearmy,
    "set-eparm",      seteparm,

    "set-produce",    setprod,

    NULL,             NULL
};


int execpriv(char *line)
{
    char **args;
    int n;

    args = parseline(line);

    if(args[0] == NULL)
        return 5;

    for(n = 0; table[n].command != NULL; n++)
        if(strcmp(table[n].command, args[0]) == 0)
            break;

    if(table[n].command == NULL)
        return 6;

    return table[n].func(countargs(args), args);
}


/* end of file. */
