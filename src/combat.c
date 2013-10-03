/*
	Solomoriah's WAR!

	combat.c -- combat rules

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
#include "reader.h"


char *armyname();

extern FILE *mfile, *news;

struct armystack {
	int ctot;
	int hero;
	int count;
	int city;
	int index[12];
};


combat(n)
int n;
{
	int i, j, c;

	for(i = 0; i < armycnt; i++)
		if(armies[i].nation == n)
			for(j = 0; j < armycnt; j++)
				if(armies[j].nation != n
				&& armies[j].r == armies[i].r
				&& armies[j].c == armies[i].c) {

					mail_line(HEADERMARK, armies[i].nation);
					mail_line(HEADERMARK, armies[j].nation);

					battle(i, j);
					putc('\n', stdout);
					putc('\n', news);
					break;
				}

	/* all battles are now over. */
	/* rescan the armies table for armies attacking undefended cities */
	/* must scan separately since armies may have died in battle */

	for(i = 0; i < armycnt; i++)
		if(armies[i].nation == n)
			if((c = city_at(armies[i].r, armies[i].c)) != -1
			&& cities[c].nation != n) {

				mail_line(HEADERMARK, armies[i].nation);
				mail_line(HEADERMARK, cities[c].nation);

				capture(armies[i].nation, c);
				putc('\n', stdout);
				putc('\n', news);
			}
}


/*
	make_stack forms the stack of armies which includes army a.
*/

make_stack(stack, a)
struct armystack *stack;
int a;
{
	int i, n, r, c, h;

	n = armies[a].nation;
	r = armies[a].r;
	c = armies[a].c;

	stack->count = 0;
	stack->ctot = 0;
	stack->hero = -1;
	h = 0;

	for(i = 0; stack->count < 12 && i < armycnt; i++) 
		if(armies[i].nation == n
		&& armies[i].r == r
		&& armies[i].c == c) {
			stack->index[stack->count] = i;
			if(armies[i].hero > h) {
				stack->hero = stack->count;
				h = armies[i].hero;
			}
			stack->ctot += armies[i].combat;
			stack->count++;
		}

	stack->ctot += stack->count * h;

	stack->city = city_at(r, c);

	if(cities[stack->city].nation != n)
		stack->city = -1;

	if(stack->city != -1)
		stack->ctot += stack->count * cities[stack->city].defense;
}


stack_remove(stack, idx)
struct armystack *stack;
int idx;
{
	int i, h;

	if(stack->count < 1)
		return;

	stack->count--;

	if(idx < stack->count)
		stack->index[idx] = stack->index[stack->count];
	
	stack->ctot = 0;
	stack->hero = -1;
	h = 0;

	for(i = 0; i < stack->count; i++) { 
		stack->ctot += armies[stack->index[i]].combat;
		if(armies[stack->index[i]].hero > h) {
			stack->hero = i;
			h = armies[stack->index[i]].hero;
		}
	}
	
	stack->ctot += stack->count * h;

	if(stack->city != -1)
		stack->ctot += stack->count * cities[stack->city].defense;
}


advance(int hero)
{
    int res;
	char buff[128];

    printf("advance %d\n", hero);

    res = roll(9) + 1;

    if(res <= (armies[hero].hero + armies[hero].combat))
        return;

    if(armies[hero].combat < 7 
    && roll(8) >= armies[hero].combat) {
        printf("advance %d combat\n", hero);
		sprintf(buff, "change-army %d %d %d\n",
            hero, armies[hero].combat + 1, armies[hero].hero);
		fputs(buff, mfile);
		execpriv(buff);
        return;
    }

    if(armies[hero].hero < 3
    && roll(4) >= armies[hero].hero) {
        printf("advance %d hero\n", hero);
		sprintf(buff, "change-army %d %d %d\n",
            hero, armies[hero].combat, armies[hero].hero + 1);
		fputs(buff, mfile);
		execpriv(buff);
    }
}


battle(a, b)
{
	struct armystack stacks[2];
	int ntns[2], r, c, city, hi, total, rlose, rwin, l, i, d, dn;
	char *names[2], buff[128];

	ntns[0] = armies[a].nation;
	ntns[1] = armies[b].nation;

	names[0] = nationcity(ntns[0]);
	names[1] = nationcity(ntns[1]);

	r = armies[a].r;
	c = armies[a].c;
	city = city_at(r, c);

	sprintf(buff, "Battle between %s and %s", names[0], names[1]);
	message_out(buff, ntns[0], ntns[1], 1);

	if(city != -1) {
		sprintf(buff, " in %s", cities[city].name);
		message_out(buff, ntns[0], ntns[1], 1);
	}

	sprintf(buff, " (Turn %d)", gen);
	message_out(buff, ntns[0], ntns[1], 0);

	message_out("\n", ntns[0], ntns[1], 1);
	
	make_stack(&(stacks[0]), a);
	make_stack(&(stacks[1]), b);

	sprintf(buff, "(Odds:  %d to %d)\n", stacks[0].ctot, stacks[1].ctot);
	message_out(buff, ntns[0], ntns[1], 0);

	for(i = 0; i < 2; i++) {

		sprintf(buff, "%s has %d armies", names[i], stacks[i].count);
		message_out(buff, ntns[0], ntns[1], 1);

		if((hi = stacks[i].hero) != -1) {
			sprintf(buff, " led by %s\n",
				armies[stacks[i].index[hi]].name[0] == '\0'
					? "a nameless hero"
					: armies[stacks[i].index[hi]].name);
			message_out(buff, ntns[0], ntns[1], 1);
		} else
			message_out("\n", ntns[0], ntns[1], 1);
	}

	while(stacks[0].count > 0 && stacks[1].count > 0) {

		total = stacks[0].ctot + stacks[1].ctot;

		/* roll dem bones */

		if(roll(total) < stacks[0].ctot)
			rlose = 1;
		else
			rlose = 0;

		rwin = (rlose + 1) % 2;

		/* check for hero desertion */

		d = -1;

		if(stacks[rlose].ctot < (stacks[rwin].ctot / 2)
		&& (hi = stacks[rlose].hero) != -1
		&& armies[stacks[rlose].index[hi]].eparm1 != 1) {
			if(roll(10) < 5) {
				dn = -1;
				if(stacks[rwin].count < 12 && roll(10) < 5)
					dn = ntns[rwin];
				d = hi;
			} else {
				sprintf(buff, "set-eparm %d %d\n",
					stacks[d].index[hi], 1);
				fputs(buff, mfile);
				execpriv(buff);
			}
		}

		/* find the victim */

		if(d == -1) {
			l = least(&(stacks[rlose]));

			sprintf(buff, "%s (%s) is destroyed.\n",
				armyname(stacks[rlose].index[l]), names[rlose]);
			message_out(buff, ntns[0], ntns[1], 0);

			if(armies[stacks[rlose].index[l]].hero > 0)
				fputs(buff, news);

			/* kill an army */

			sprintf(buff, "kill-army %d\n", stacks[rlose].index[l]);
			fputs(buff, mfile);
			execpriv(buff);

		} else {
			l = d;

			sprintf(buff, "%s (%s) deserts",
				armyname(stacks[rlose].index[l]), names[rlose]);
			message_out(buff, ntns[0], ntns[1], 1);

			if(dn != -1) {
				sprintf(buff, " and joins %s\n", nationcity(dn));
				message_out(buff, ntns[0], ntns[1], 1);
			} else
				message_out("\n", ntns[0], ntns[1], 1);

			/* hero deserts */

			sprintf(buff, "kill-army -2 %d %d\n",
				stacks[rlose].index[d], dn);
			fputs(buff, mfile);
			execpriv(buff);

			/* rebuild other stack around the deserter */

			if(dn != -1)
			    make_stack(&(stacks[rwin]), d);
		}

		/* remove army from stack */

		stack_remove(&(stacks[rlose]), l);
	}

	if(stacks[0].count < 1)
		rwin = 1;
	else
		rwin = 0;

	sprintf(buff, "%s wins!\n", names[rwin]);
	message_out(buff, ntns[0], ntns[1], 1);

	if(city != -1
	&& cities[city].nation != ntns[rwin])
		capture(ntns[rwin], city);

	if((hi = stacks[rwin].hero) != -1)
        advance(stacks[rwin].index[hi]);
}


capture(n, c)
int n, c;
{
	char buff[128];
	int n2;

	n2 = cities[c].nation;

	sprintf(buff, "%s captures %s", nationcity(n), cities[c].name);
	message_out(buff, n, n2, 1);

	if(cities[c].nation > 0) {
		sprintf(buff, " from %s\n",
			nationcity(n2));
		message_out(buff, n, n2, 1);
	} else
		message_out("\n", n, n2, 1);

	sprintf(buff, "control-city %d %d 1\n", n, c);
	fputs(buff, mfile);
	execpriv(buff);
}


int least(stack)
struct armystack *stack;
{
	int i, n;

	n = 0;

	for(i = 0; i < stack->count; i++)
		if(isgreater(stack->index[n], stack->index[i], GT_COMBAT))
			n = i;

	return n;
}


/* end of file. */
