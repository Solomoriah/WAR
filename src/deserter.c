/*
	Solomoriah's WAR!

	deserter.c -- desertion rules

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

char *armyname();


/*
	deserter() causes a randomly-selected non-hero army to desert the 
	given nation.
*/

deserter(n)
int n;
{
	int narmies, i, die;
	char buff[128];

	narmies = 0;

	for(i = 0; i < armycnt; i++)
		if(armies[i].nation == n
		&& armies[i].hero < 1)
			narmies++;

	if(narmies < 1)
		return;

	die = roll(narmies);

	for(i = 0; i < armycnt; i++)
		if(armies[i].nation == n
		&& armies[i].hero < 1)
			if(die == 0)
				break;
			else
				die--;


	/* found one, kill it. */

	mail_line(" \b", n);
	sprintf(buff, "%s deserts %s of %s.",
		armyname(i), nations[n].name,
		nationcity(n));
	message_out(buff, n, 0, 1);

	sprintf(buff, " (Turn %d)", gen);
	message_out(buff, n, 0, 0);

	message_out("\n\n", n, 0, 1);

	/* so kill it. */

	sprintf(buff, "kill-army %d\n", i);
	fputs(buff, mfile);
	execpriv(buff);
}


/* end of file. */
