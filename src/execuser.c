/*
    Solomoriah's WAR!

    execuser.c -- execute command files in user mode

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


extern FILE *mfile;

static int makearmy(), movearmy(), setprod(), namearmy();


static struct parse_tbl table[] = {

    "move-army",      movearmy,
    "make-army",      makearmy,
    "name-army",      namearmy,
    "set-produce",    setprod,

    NULL,         NULL
};


int execuser(char *line)
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


/*
    make-army 'name' nation row column
            combat hero move_rate move_tbl special_mv
*/

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
    armies[armycnt].eparm1 = 0;

    armycnt++;

    fprintf(mfile, "make-army '%s' %s %s %s %s %s %s %s %s\n",
        argv[1], argv[2], argv[3],
        argv[4], argv[5], argv[6],
        argv[7], argv[8], argv[9]);

    return 0;
}


/*
    move-army army cost row column depends-on
*/

static int movearmy(int argc, char *argv[])
{
    int a, b;

    a = atoi(argv[1]);

    if(armies[a].nation == -1)
        return 0;

    if(argc > 5) {
        b = atoi(argv[5]);

        if(a != b && armies[b].nation == -1)
            return 0;
    }

    b = armies[a].move_left; 
    b -= atoi(argv[2]);
    if(b < 0)
        b = 0;
    armies[a].move_left = b;
    armies[a].r = atoi(argv[3]);
    armies[a].c = atoi(argv[4]);

    if(argc > 5)
        fprintf(mfile, "move-army %s %s %s %s %s\n",
            argv[1], argv[2], argv[3], argv[4], argv[5]);
    else
        fprintf(mfile, "move-army %s %s %s %s\n",
            argv[1], argv[2], argv[3], argv[4]);

    return 0;
}


/*
    name-army army 'name'
*/

static int namearmy(int argc, char *argv[])
{
    int a;

    a = atoi(argv[1]);

    if(armies[a].nation == -1)
        return 0;

    strncpy(armies[a].name, argv[2], ARMYNAMLEN);
    armies[a].name[ARMYNAMLEN] = '\0';

    fprintf(mfile, "name-army %s '%s'\n", argv[1], argv[2]);

    return 0;
}


/*
    set-produce city type
*/

static int setprod(int argc, char *argv[])
{
    int c;

    c = atoi(argv[1]);

    cities[c].prod_type = atoi(argv[2]);
    cities[c].turns_left = cities[c].times[cities[c].prod_type];

    fprintf(mfile, "set-produce %s %s\n", argv[1], argv[2]);

    return 0;
}


/* end of file. */
