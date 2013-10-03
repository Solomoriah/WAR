/*
	Solomoriah's WAR!

	warupd.c -- main procedure file for war update

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
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#include "war.h"
#include "warext.h"
#include "function.h"

extern char *errors[];

int nlist[MAXNATIONS], nlcnt = 0;

FILE *mfile = NULL, *news = NULL;


main(argc, argv)
int argc;
char **argv;
{
	int rc, i, j, k, n, u;
	FILE *fp;
	char filename[64], inbuf[128], *p;
	struct stat st_buf;


	fprintf(stdout,
        "\nWARUPD Version %d.%d  \"Code by Solomoriah\"\n", 
        major_ver, minor_ver);
	fputs("Copyright 1993, 1994, 2001, Chris Gonnerman\n", stdout);
	fputs("All Rights Reserved.\n\n", stdout);

	
	/* set directory */

	if(argc > 1)
		if(chdir(argv[1]) == -1) {
			fprintf(stderr, "Error Setting Directory %s\n", argv[1]);
			exit(1);
		}
	
	/* randomize */

	srand48((long)time(NULL));

	/* load map file */

	rc = loadmap();

	if(rc != 0) {
		printf("Error Loading Map (%s)\n", errors[rc]);
		exit(1);
	}

	/* load game save */

	rc = loadsave();

	if(rc != 0) {
		printf("Error Loading Game Save (%s)\n", errors[rc]);
		exit(1);
	}

	if(world[0] != '\0')
		printf("World:  %s\n", world);

	/* open news output */

	news = fopen(NEWSFL, "a");

	if(news == NULL) {
		printf("Error Writing News File <%s>\n", NEWSFL);
		exit(10);
	}

	printf("Turn Update %d\n\n", gen);

	if(world[0] != '\0') {
		fputs(" \b", news);
		fputs(world, news);
	} else
		fputs(" \bSolomoriah's WAR!", news);

	fprintf(news, " News Report     Turn %d\n\n", gen);

	/* execute master file */

	puts("Reading Master Commands...");

	fp = fopen(MASTERFL, "r");

	if(fp != NULL) {
		for(i = 0; fgets(inbuf, 128, fp) != NULL; i++) {
			rc = execpriv(inbuf);
			if(rc != 0) {
				printf("Master Cmd Failed, Line %d, Code %d\n", i+1, rc);
				exit(2);
			}
		}

		fclose(fp);
	}

	/* open master for writing. */

	mfile = fopen(MASTERFL, "a");

	if(mfile == NULL) {
		puts("Can't Append to Master File");
		exit(5);
	}

	/* execute player commands */

	puts("Reading Player Commands...");

	shuffle();

	for(k = 0; k < nlcnt; k++) {

		n = nlist[k];
		u = nations[n].uid;

		sprintf(filename, PLAYERFL, u);

		fp = fopen(filename, "r");

		if(fp != NULL) {

			printf("\n%s of %s moves...\n",
				nations[n].name, nationcity(n));

			nations[n].idle_turns = 0;

			for(i = 0; fgets(inbuf, 128, fp) != NULL; i++) {
				rc = execuser(inbuf);
				if(rc != 0) {
					printf("Player Cmd Failed, Line %d, Code %d  ",
						i+1, rc);
					exit(2);
				}
			}

			fclose(fp);

			unlink(filename);
		} else {
			printf("\n%s of %s is idle.\n",
				nations[n].name, nationcity(n));
			nations[n].idle_turns++;
		}

		combat(n);

		if(nations[n].idle_turns >= TIME_TO_DESERT)
			deserter(n);
		else
			creator(n);
	}

	/* do the update */

	puts("Global Update...\n");

	execpriv("global-update");
	fputs("global-update\n", mfile);

	/* close up */

	fclose(news);
	fclose(mfile);

	mail_line(NULL, -1);

	/* consolidate */

	sprintf(filename, MASTERBAK, gen);

	if(rename(MASTERFL, filename) == -1) {
		puts("Can't Rename Master Cmd File");
		exit(9);
	}

	sprintf(filename, GAMEBAK, gen);

	if(rename(GAMESAVE, filename) == -1) {
		puts("Can't Rename Game Save");
		exit(9);
	}

	fp = fopen(GAMESAVE, "w");

	if(fp == NULL) {
		puts("Can't Create New Save File");
		exit(9);
	}

	/* clean up outdated files. */

	if(gen - 4 > 0) {
		sprintf(filename, MASTERBAK, gen - 4);
		unlink(filename);

		sprintf(filename, GAMEBAK, gen - 4);
		unlink(filename);
	}

	savegame(fp);

	fclose(fp);

	exit(0);
}


shuffle()
{
	int i, n1, n2, tmp;

	nlcnt = 0;

	for(i = 1; i < nationcnt; i++)
		nlist[nlcnt++] = i;

	for(i = 0; i < nlcnt * nlcnt; i++) {
		n1 = roll(nlcnt);
		n2 = roll(nlcnt);
		if(n1 != n2) {
			tmp = nlist[n1];
			nlist[n1] = nlist[n2];
			nlist[n2] = tmp;
		}
	}
}


message_out(text, n1, n2, do_news)
char *text;
int n1, n2, do_news;
{
	if(n1)
		mail_line(text, n1);

	if(n2)
		mail_line(text, n2);

	if(do_news)
		fputs(text, news);

	fputs(text, stdout);
}


mail_line(text, ntn)
char *text;
int ntn;
{
	int i, pos;
	char fname[16];

	static mail_gen = 0;

	static struct {
		int ntn, age;
		FILE *fp;
	} mailfiles[8] = {
		-1, 0, NULL,    -1, 0, NULL,
		-1, 0, NULL,    -1, 0, NULL,
		-1, 0, NULL,    -1, 0, NULL,
		-1, 0, NULL,   -1, 0, NULL,
	};


	if(ntn == -1) { /* close all */
		for(i = 0; i < 8; i++) {
			if(mailfiles[i].fp != NULL)
				fclose(mailfiles[i].fp);

			mailfiles[i].ntn = -1;
			mailfiles[i].age = 0;
			mailfiles[i].fp = NULL;
		}
		return;
	}

	if(ntn == 0 || ntn == 27) /* ignore god and rogue */
		return;

	/* find the nation's mail file if already open... */

	for(i = 0; i < 8; i++)
		if(mailfiles[i].ntn == ntn) {
			mailfiles[i].age = mail_gen++;
			break;
		}

	if(i >= 8) { /* not open. */
		pos = 0;

		for(i = 1; i < 8; i++)
			if(mailfiles[i].age < mailfiles[pos].age)
				pos = i;

		if(mailfiles[pos].fp != NULL)
			fclose(mailfiles[pos].fp);

		mailfiles[pos].ntn = -1;

		sprintf(fname, MAILFL, ntn);
		mailfiles[pos].fp = fopen(fname, "a");

		if(mailfiles[pos].fp == NULL) /* error, can't stop now... */
			return;

		mailfiles[pos].ntn = ntn;
		mailfiles[pos].age = mail_gen++;

	} else
		pos = i;

	fputs(text, mailfiles[pos].fp);
}


nation_news(name, city)
char *name;
int city;
{
	char buff[80];

	sprintf(buff, "%s becomes the ruler of %s!\n\n",
		name, cities[city].name);

	fputs(buff, stdout);
	fputs(buff, news);
}


/* end of file. */
