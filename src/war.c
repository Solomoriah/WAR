/*
    Solomoriah's WAR!

    war.c -- main procedure file for war user interface

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
#include "genio.h"
#include "function.h"

extern char *errors[];

extern int map_width, map_height;

extern int trans;

FILE *pfile = NULL;



int main(int argc, char *argv[])
{
    int rc, i, uid, n, c, ch, t, trys, mtbl, line, offset, confirmed;
    FILE *fp, *pp;
    char filename[64], inbuf[128], name[NATIONNMLEN+1], *p, *u;
    char user[128], pass[128], cmd[128];
    char *getenv();
    char *gamedir;
    struct stat st_buf;

    /* set directory */

#ifdef WARPATH

    if(chdir(WARPATH) == -1) {
        gmove(20,18);
        gprintf("Error Setting Directory %s\n", WARPATH);
        ggetch();
        gend();
        exit(1);
    }

#endif

    if(argc > 1) {
        gamedir = argv[1];

#ifdef WARPATH

        /* in this code, we must restrict the user from leaving the WARPATH directory */
        /* and actually we'll limit the user to a single subdirectory under it */

        /* ".." in the middle of the gamedir will be effectively blocked below */
        /* (as slashes would be needed to make it useful) */
        if(gamedir[0] == '.' && gamedir[1] == '.' && gamedir[2] == '\0') {
            gmove(20,18);
            gprintf("Invalid Directory Name %s\n", gamedir);
            ggetch();
            gend();
            exit(1);
        }

        for(p = gamedir; *p; p++) {
            if(*p == '/' || *p == '\\' || *p == ':') {
                gmove(20,18);
                gprintf("Invalid Directory Name %s\n", gamedir);
                ggetch();
                gend();
                exit(1);
            }
        }

#endif

        if(chdir(gamedir) == -1) {
            gmove(20,18);
            gprintf("Error Setting Directory %s\n", gamedir);
            ggetch();
            gend();
            exit(1);
        }
    }

    /* set up screen */

    ginit();

    titlescreen();


    /* randomize */

    srand48((long)time(NULL));


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

    gmove(16,10);

    if(world[0] != '\0') {
        gputs("World:  ");
        gputs(world);
        gputs("    ");
    }

    gprintf("Turn:  %d", gen);

    /* get player uid */

    uid = 0;

    u = getenv("WARID");

    if(u == NULL)
        u = getenv("UID");

    if(u != NULL)
        if(strncmp(u, "file ", 5) == 0) {
            /* read UID from file */
            line = 0;
            offset = 0;
            sscanf(u, "file %s %d %d", inbuf, &line, &offset);
            fp = fopen(inbuf, "r");
            for(i = 0; i < 128; i++)
                inbuf[i] = '\0';
            if(fp != NULL) {
                while(line-- && fgets(inbuf, 128, fp) != NULL);
                fclose(fp);
                uid = atoi(inbuf+offset-1);
            }
        } else
            uid = atoi(u);

#ifdef AUTHPGM
    trys = 3;

    while(uid == 0 && trys--) {

        user[0] = '\0';
        pass[0] = '\0';

        gmove(19,10);
        gputs("Username:  ");
        gclrline();
        getstring(user);

        gmove(20,10);
        gputs("Password:  ");
        gclrline();
        getpass(pass);

        gmove(19,10);
        gclrline();
        gmove(20,10);
        gclrline();

        sprintf(cmd, AUTHPGM, user, pass);

        pp = popen(cmd, "r");
        inbuf[0] = '\0';
        inbuf[127] = '\0';
        fgets(inbuf, 127, pp);
        pclose(pp);

        uid = atoi(inbuf);
    }
#endif

    if(uid == 0) {
        gmove(20,10);
        gputs("UID Not Available; Aborting.");
        ggetch();
        gend();
        exit(3);
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

    /* check if nation is entered. */

    n = -1;

    for(i = 0; i < nationcnt; i++)
        if(nations[i].uid == uid) {
            n = i;
            break;
        }

    if(n == -1) { /* nation not entered yet... */

        c = newcity();

        if(c >= 0) {

            fp = fopen(MASTERFL, "a");

            if(fp == NULL) {
                gmove(20,10);
                gputs("Can't Write Master Cmd's  ");
                gclrline();
                ggetch();
                gend();
                exit(6);
            }

            n = nationcnt;
            confirmed = 0;

            while(!confirmed) {
                gmove(18,10);
                gprintf("Your City is %s", cities[c].name);
                gclrline();

                inbuf[0] = '\0';

                while(inbuf[0] == '\0') {
                    gmove(20,10);
                    gputs("(Enter ! to Retry)");
                    gclrline();
                    gmove(19,10);
                    gputs("Enter your name:  ");
                    gclrline();
                    getstring(inbuf);
                }

                if(strcmp(inbuf, "!") == 0) {
                    c = newcity();
                } else
                    confirmed = 1;
            }

            strncpy(name, inbuf, NATIONNMLEN);
            name[NATIONNMLEN] = '\0';

            gmove(21,10);
            gputs("Available:  ");

            strcpy(inbuf, marks[0]);

            for(i = 0; i < nationcnt; i++)
                inbuf[nations[i].mark] = ' ';

            for(i = 0; inbuf[i]; i++)
                if(inbuf[i] != ' ')
                    gputch(inbuf[i]);

            gmove(20,10);
            gputs("Select Nation Mark:  ");
            gclrline();

            while(strchr(inbuf, ch = toupper(ggetch())) == NULL
            || ch == ' ');

            gputch(ch);

            gmove(21,10);
            gclrline();

            for(i = 0; inbuf[i] && inbuf[i] != ch; i++);

            /* create the nation... */

            sprintf(inbuf, "new-nation '%s' %d %d %d\n",
                name, uid, c, i);
            fputs(inbuf, fp);
            execpriv(inbuf);

            /* ... and the hero... */

            t = cities[c].types[0];
            mtbl = ttypes[t].move_tbl;

            sprintf(inbuf, "make-army '%s' %d %d %d %d %d %d %d %d %d\n",
                name, n, cities[c].r, cities[c].c,
                roll(3)+4, roll(2)+1, 8, mtbl, 0, 1);
            fputs(inbuf, fp);
            execpriv(inbuf);

            /* ...and take the city. */

            sprintf(inbuf, "control-city %d %d\n", n, c);
            fputs(inbuf, fp);
            execpriv(inbuf);

            fclose(fp);

        } else {
            gmove(20,10);
            gputs("No Cities Available.  ");
            gclrline();
            ggetch();
            gend();
            exit(5);
        }
    }

    /* execute player commands */

    gmove(20,10);
    gputs("Reading Player Commands...  ");
    gclrline();
    grefresh();

    sprintf(filename, PLAYERFL, uid);

    fp = fopen(filename, "r");

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
                gprintf("Player Cmd Failed, Line %d, Code %d  ", i+1, rc);
                (void)ggetch();
                gend();
                exit(2);
            }
        }

        fclose(fp);
    }

    /* append to player file... */

    pfile = fopen(filename, "a");

    if(pfile == NULL) {
        gmove(20,10);
        gputs("Can't Create or Append Player Cmd's  ");
        gclrline();
        ggetch();
        exit(4);
    }

    setbuf(pfile, NULL);

    /* main loop */

    for(i = 0; i < nationcnt; i++)
        if(nations[i].uid == uid)
            n = i;

    gmove(19,10);
    gprintf("Welcome, %s of %s!", nations[n].name, nationcity(n));

    gclrline();

    gmove(20,10);
    gputs("Press Any Key to Begin...  ");
    gclrline();

    ggetch();

    mainscreen();

    mainloop(n);

    /* clean up */

    fclose(pfile);

    sprintf(filename, PLAYERMAP, uid);
    pfile = fopen(filename, "w");
    writemap(pfile, n);
    fclose(pfile);

    gend();

    exit(0);

}


int newcity()
{
    int i, clusters[MAXCITIES], cc, c, cl;

    for(i = 0; i < MAXCITIES; i++)
        clusters[i] = -1;

    for(i = 0; i < citycnt; i++) {
        if(cities[i].nation > 0)
            clusters[cities[i].cluster % MAXCITIES] = 1;
        else if(clusters[cities[i].cluster % MAXCITIES] == -1)
            clusters[cities[i].cluster % MAXCITIES] = 0;
    }

    cc = 0;

    for(i = 0; i < MAXCITIES; i++)
        if(clusters[i] == 0)
            cc++;

    if(cc == 0) {   /* any city will do... */

        cc = 0;

        for(i = 0; i < citycnt; i++)
            if(cities[i].nation == 0)
                cc++;

        if(cc == 0)
            return -1;

        c = roll(cc);

        for(i = 0; i < citycnt; i++)
            if(cities[i].nation == 0)
                if(c == 0)
                    return i;
                else
                    c--;

        /* should not get here... */

        return -1;

    } else {        /* find an empty cluster... */

        c = roll(cc);

        for(i = 0; i < MAXCITIES; i++)
            if(clusters[i] == 0)
                if(c == 0)
                    break;
                else
                    c--;

        if(i < MAXCITIES) {

            cl = i;

            cc = 0;

            for(i = 0; i < citycnt; i++)
                if(cities[i].cluster == cl)
                    cc++;

            c = roll(cc);

            for(i = 0; i < citycnt; i++)
                if(cities[i].cluster == cl)
                    if(c == 0)
                        return i;
                    else
                        c--;
        }

        /* should not get here. */

        return -1;
    }
}


/*
    nation_news is a stub here because the front-end war program
    does not write the news.
*/

void nation_news(char *name, int city)
{
    ;
}


/* end of file. */
