/*
	Solomoriah's WAR!

	mailer.c -- mail sender

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
#include "genio.h"
#include "function.h"
#include "reader.h"


#define MAILTTL "Mail Editor:  "
#define MAILKEYS    "(ctrl-j)down  (ctrl-k)up  (ctrl-h)left  (ctrl-l)right"
#define MAILMENU    "(s)end message  (c)ancel message  (r)eturn to message"
#define MAILESC "Press CTRL-A for Menu"


/*
	mailer() allows the user to enter up to a screen full of message,
	then sends it to the specified nations' mailbox.
*/

mailer(from, to)
int from, to;
{
	char message[17][80], heading[80], fname[16];
	int i, j, l, c, ch, save, menu, last;
	FILE *fp, *log;

	l = c = 0;

	for(i = 0; i < 17; i++)
		for(j = 0; j < 80; j++)
			message[i][j] = ' ';

	if(to > 0) {
		sprintf(heading, "From %s of %s to %s of %s  (Turn %d)",
			nationname(from), nationcity(from),
			nationname(to), nationcity(to), turn());
	} else {
		sprintf(heading, "From %s of %s  (Turn %d)",
			nationname(from), nationcity(from), turn());
	}

	/* set up screen */

	gerase(0);

	gmove(19, 1);
	gputs("---------------------------------------");
	gputs("---------------------------------------");

	gmove(0, 1);
	gputs(heading);

	menu = 1;
	save = 0;

	/* edit loop */

	do {
		if(menu) {
			gmove(20, 1);
			gputs(MAILTTL);
			gputs(MAILKEYS);
			gclrline();

			gmove(21, 1);
			gputs(MAILESC);
			gclrline();

			menu = 0;
		}
		gmove(l+2, c+1);
		ch = ggetch();

		switch(ch) {

        case GKEY_HOME :
            c = 0;
            break;

		case '\r' :   /* return */
			c = 0;
			l++;
			break;

		case '\b' :   /* ctrl-h */
		case (char)0xff :   /* del */
		case GKEY_LEFT :
			c--;
			break;

		case '\f' :   /* ctrl-l */
		case GKEY_RIGHT :
			c++;
			break;

		case '\013' : /* ctrl-k */
		case GKEY_UP :
			l--;
			break;

		case '\n' :   /* ctrl-j */
		case GKEY_DOWN :
			l++;
			break;

		case '\033' :
		case '\01' :

            ch = '\033';

			gmove(20, 1);
			gputs(MAILTTL);
			gputs(MAILMENU);
			gclrline();

			gmove(21, 1);
			gclrline();

			menu = 1;

			switch(tolower(ggetch())) {

			case 'c' : /* cancel */
			case 'q' :
			case '\033' :
				save = 0;
				break;

			default :
			case 'r' : /* return */
				ch = '\0';
				break;

			case 's' : /* send */
				save = 1;
				break;
			}

			break;

		default :

			if(c > 75)
				break;

			if(isspace(ch))
				gputch(message[l][c++] = ' ');
			else if(isprint(ch))
			    gputch(message[l][c++] = ch);

			break;
		}

		if(c < 0)
			c = 0;

		if(c > 75)
			c = 75;

		if(l < 0)
			l = 16;

		if(l > 16)
			l = 0;

	} while(ch != '\033');

	/* clean up screen */

	gerase(0);
	mainscreen();

	/* save? */

	if(!save)
		return;

	if(to > 0)
		sprintf(fname, MAILFL, to);
	else
		strcpy(fname, NEWSFL);

	fp = fopen(fname, "a");

	if(fp == NULL) {
		saystat("Mail Could Not Be Sent...  Cancelled.");
		return;
	}

	log = fopen(MAILLOG, "a");

	fputs(HEADERMARK, fp);
	fputs(heading, fp);
	fputs("\n\n", fp);

	if(log != NULL) {
		fputs(HEADERMARK, log);
		fputs(heading, log);
		fputs("\n\n", log);
	}

	for(i = 0; i < 17; i++) {
		for(j = 78; j >= 0 && message[i][j] == ' '; j--);
		message[i][j+1] = '\0';
	}

	last = 16;

	for(i = 16; i >= 0; i--)
		if(message[i][0] == '\0')
			last = i;
		else
			break;

	for(i = 0; i <= last; i++) {
		for(j = 0; message[i][j]; j++) {
			putc(message[i][j], fp);
			if(log != NULL)
				putc(message[i][j], log);
		}

		putc('\n', fp);

		if(log != NULL)
			putc('\n', log);
	}

	fclose(fp);
	fclose(log);

	saystat("Mail Sent.");
}


/* end of file. */
