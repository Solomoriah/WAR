/*
	Solomoriah's WAR!

	savegame.c -- savegame procedure

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
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "war.h"
#include "warext.h"


savegame(FILE *fp)
{
	int i, j;
	char *p;

	/* write header line */

	for(p = world; *p; p++)
		if(*p == ' ')
			*p = '_';

	fprintf(fp, "G %d %d %d %d %d g%d %s\n",
		nationcnt, citycnt, ttypecnt, armycnt, mvtcnt, gen, world);

	/* save nations table */

	putc('\n', fp);

	for(i = 0; i < nationcnt; i++)
		fprintf(fp, "%s: U%d C%d M%d I%d\n",
				nations[i].name, nations[i].uid,
				nations[i].city, nations[i].mark,
				nations[i].idle_turns);

	/* save cities table */

	for(i = 0; i < citycnt; i++) {

		putc('\n', fp);

		/* city id line */

		fprintf(fp, "%s: C%d N%d @%d:%d P%d:%d D%d T%d\n",
			cities[i].name,
			cities[i].cluster,
			cities[i].nation,
			cities[i].r, cities[i].c,
			cities[i].prod_type, cities[i].turns_left,
			cities[i].defense, cities[i].ntypes);

		/* troop types */

		for(j = 0; j < 4; j++) {
			fprintf(fp, "%d %d %d\n",
				cities[i].types[j],
				cities[i].times[j],
				cities[i].instance[j]);
		}
	}

	/* save troop types table */

	putc('\n', fp);

	for(i = 0; i < ttypecnt; i++)
		fprintf(fp, "%s: C%d M%d:%d:%d\n",
			ttypes[i].name,
			ttypes[i].combat,
			ttypes[i].move_rate,
			ttypes[i].move_tbl,
			ttypes[i].special_mv);

	/* save armies table */

	if(armycnt > 0) {

		putc('\n', fp);

		for(i = 0; i < armycnt; i++)
			fprintf(fp, "%s: N%d @%d:%d C%d:%d M%d:%d S%d L%d\n",
				armies[i].name,
				(int)armies[i].nation,
				(int)armies[i].r, (int)armies[i].c,
				(int)armies[i].combat, (int)armies[i].hero,
				(int)armies[i].move_rate, (int)armies[i].move_tbl,
				(int)armies[i].special_mv, (int)armies[i].eparm1);
	}

	/* save move table */

	putc('\n', fp);

	for(i = 0; i < mvtcnt; i++) {

		for(j = 0; j < TERR_TYPES; j++)
			fprintf(fp, "%d ", move_table[i].cost[j]);

		putc('\n', fp);
	}
}


/* end of file. */
