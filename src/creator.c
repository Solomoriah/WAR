/*
	Solomoriah's WAR!

	creator.c -- creation rules

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

#include "war.h"
#include "warext.h"
#include "function.h"


extern FILE *mfile;


/*
	creator() constructs a hero for the given nation, *if* the nation
	needs one and if a roll indicates one is to be created.

	a nation "needs" a hero when it has more cities than heros.

	a roll is then made using (number of cities) - (number of heros)
	as a percent chance EXCEPT if the nation has NO heros; then the
    odds are 10 times normal.  Maximum odds are 90% in either case.
*/

creator(n)
int n;
{
	int nheros, ncities, odds, counter, i, c, r, t, mtbl;
	char buff[128];

	ncities = 0;

	for(i = 0; i < citycnt; i++)
		if(cities[i].nation == n)
			ncities++;

	nheros = 0;

	for(i = 0; i < armycnt; i++)
		if(armies[i].nation == n
		&& armies[i].hero > 0)
			nheros++;

	odds = ncities - nheros;

	if(nheros == 0)
		odds *= 10;

    if(odds > 90)
        odds = 90;

	if(nheros >= ncities
	|| roll(100) >= odds)
		return;

	r = roll(ncities);

	for(i = 0; i < citycnt; i++)
		if(cities[i].nation == n)
			if(r-- < 1)
				break;

	c = i;

	/* too many armies there? */

	counter = 0;

	for(i = 0; i < armycnt; i++)
		if(armies[i].nation == n
		&& armies[i].r == cities[c].r
		&& armies[i].c == cities[c].c)
			counter++;

	if(counter >= 12)
		return;

	/* it's OK, do it. */

	mail_line(" \b", n);
	sprintf(buff, "A hero arises in %s and joins %s of %s!",
		cities[c].name, nations[n].name, nationcity(n));
	message_out(buff, n, 0, 1);

	sprintf(buff, " (Turn %d)", gen);
	message_out(buff, n, 0, 0);

	message_out("\n\n", n, 0, 1);

		t = cities[c].types[0];
		mtbl = ttypes[t].move_tbl;

	sprintf(buff, "make-army '' %d %d %d %d %d %d %d %d %d\n",
		   n, cities[c].r, cities[c].c,
		   roll(4)+2, roll(2)+1, 8, mtbl, 0, 0);

	fputs(buff, mfile);
	execpriv(buff);
}


/* end of file. */
