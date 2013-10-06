/*
	Solomoriah's WAR!

	wartool.c -- main procedure file for wartool editor

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
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "war.h"
#include "warext.h"
#include "display.h"

extern char *errors[];

extern int map_width, map_height;

/* following line only present to allow display.c to link. */
FILE *pfile = NULL;

extern int avcnt, avpnt;
extern struct aview armyview[12];

extern char *marks[];


main(argc, argv)
int argc;
char **argv;
{
	int rc, i, uid, n, c, ch, t, mtbl, line, offset;
	FILE *fp;
	char filename[64], inbuf[128], name[NATIONNMLEN+1], *p, *u;
	struct stat st_buf;


	/* set directory */

	if(argc > 1)
		if(chdir(argv[1]) == -1) {
			gmove(20,18);
			gprintf("Error Setting Directory %s\n", argv[1]);
			ggetch();
			gend();
			exit(1);
		}
	

	/* set up display */

	ginit();

	gmove(5, 10);
	gputs("WARTOOL 1.0 Game Editor for Solomoriah's WAR!");
	gmove(6, 10);
	gputs("Copyright 1994, J. Christopher Gonnerman");
	gmove(7, 10);
	gputs("All Rights Reserved.");

	
	/* load map file */

	rc = loadmap();

	if(rc != 0) {
		gmove(20,10);
		gprintf("Error Loading Map (%s)\n", errors[rc]);
		ggetch();
		gend();
		exit(1);
	}

	/* load game save */

	rc = loadsave();

	if(rc != 0) {
		gmove(20,10);
		gprintf("Error Loading Game Save (%s)\n", errors[rc]);
		ggetch();
		gend();
		exit(1);
	}

	/* execute master file */

	gmove(20,10);
	gputs("Reading Master Commands...  ");
	grefresh();

	fp = fopen(MASTERFL, "r");

	if(fp != NULL) {
		for(i = 0; fgets(inbuf, 128, fp) != NULL; i++) {
			rc = execpriv(inbuf);
			if(rc == 0) {
				if((i+1) % 10 == 0) {
					gmove(20,48);
					gprintf("%3d lines", i+1);
					grefresh();
				}
			} else {
				gmove(20,10);
				gprintf("Master Cmd Failed, Line %d, Code %d  ", i+1, rc);
				(void)ggetch();
				gend();
				exit(2);
			}
		}

		fclose(fp);
	}

	/* main loop */

	gmove(20,10);
	gputs("Press Any Key to Begin...  ");
	gclrline();

	ggetch();

	mainscreen();

	toolmain();

	unlink("game.bak");
	if(rename("game.sav", "game.bak") == 0)
		fp = fopen("game.sav", "w");
	else
		fp = fopen("game.sv!", "w");

	savegame(fp);

	fclose(fp);

	/* clean up */

	endwin();

	exit(0);
}


/*
	nation_news is a stub here because the front-end war program
	does not write the news.
*/

nation_news(name, city)
char *name;
int city;
{
	;
}


toolmain()
{
	int r, c, ch, cmove, i, j;
	char buf[80];

	r = 0;
	c = 0;

	setfocus(-1, r, c);
	
	cmove = -1;

	while(ch != 'q' && ch != 'Q') {

		showmap(r, c, 1);
		showcity(r, c);
		showfocus(r, c, 1);

		ch = ggetch();

		showfocus(r, c, 0);

		switch(ch) {

		case '7' :
			r--;
			c--;
			setfocus(-1, r, c);
			break;

		case '8' :
			r--;
			setfocus(-1, r, c);
			break;

		case '9' :
			r--;
			c++;
			setfocus(-1, r, c);
			break;

		case '6' :
			c++;
			setfocus(-1, r, c);
			break;

		case '3' :
			r++;
			c++;
			setfocus(-1, r, c);
			break;

		case '2' :
			r++;
			setfocus(-1, r, c);
			break;

		case '1' :
			r++;
			c--;
			setfocus(-1, r, c);
			break;

		case '4' :
			c--;
			setfocus(-1, r, c);
			break;

		case 'j' :
		case 'd' :
			if(avcnt > 0) {
				avpnt++;
			 avpnt += avcnt;
				avpnt %= avcnt;
			} else
				saystat("No Armies!");
			break;

		case 'k' :
		case 'u' :
			if(avcnt > 0) {
				avpnt--;
				avpnt += avcnt;
				avpnt %= avcnt;
			} else
				saystat("No Armies!");
			break;

		case '\f' :
			clearok(stdscr,TRUE);
			break;

		case '*' : /* mark all */
			for(i = 0; i < avcnt; i++)
				armyview[i].mark = 1;
			break;

		case ' ' : /* mark/unmark current */
			if(avcnt > 0)
				armyview[avpnt].mark =
					armyview[avpnt].mark ? 0 : 1;
			break;

		case '/' : /* unmark all */
			for(i = 0; i < avcnt; i++)
				armyview[i].mark = 0;
			break;

		case 'm' : /* move army group */
			move_mode(-1, &r, &c);
			setfocus(-1, r, c);
			break;

		case 'C' : /* clear all */
			saystat("Clear All Turns?  (Y/N)  ");
			if(tolower(ggetch()) == 'y')
				clearall();    
			clearstat(-1);
			setfocus(-1, r, c);
			break;

		case 'M' : /* move city */
			if(cmove >= 0) {
				cmove = -1;
				clearstat(-1);
				break;
			}
			/* find the city */
			for(i = 0; i < citycnt; i++)
				if(cities[i].r == r && cities[i].c == c)
					cmove = i;
			if(cmove < 0)
				saystat("No City Here.");
			else {
				sprintf(buf, "Moving %s", cities[cmove].name);
				saystat(buf);
			}
			break;

		case '\033' :
			cmove = -1;
			clearstat(-1);
			setfocus(-1, r, c);
			break;
		
		case 'q' : /* quit */
			saystat("Quit?  (Y/N)  ");
			if(tolower(ggetch()) != 'y')
				ch = '\0';                                    
			clearstat(-1);
			break;
		}

		r = (r + map_height) % map_height;
		c = (c + map_width) % map_width;

		if(cmove >= 0) {
			cities[cmove].r = r;
			cities[cmove].c = c;
			setfocus(-1, r, c);
		}
	}
}


clearall()
{
	int i, j;

	/* reset all counters */

	gen = 0;
	armycnt = 0;
	nationcnt = 1;

	for(i = 0; i < citycnt; i++) {
		cities[i].nation = 0;
		cities[i].prod_type = 0;
		cities[i].turns_left = 1;
		for(j = 0; j < TYPECNT; j++)
			cities[i].instance[j] = 0;
	}
}


mailer()
{
	;
}


reader()
{
	;
}


/* end of file. */
