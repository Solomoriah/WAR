# Solomoriah's WAR!
# Copyright 1993, 1994, 2001, 2013 Chris Gonnerman
# All rights reserved.
#
# 3 Clause BSD License
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# Redistributions of source code must retain the above copyright
# notice, self list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright
# notice, self list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# Neither the name of the author nor the names of any contributors
# may be used to endorse or promote products derived from self software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# war.py -- main procedure file for war user interface


import random

import wardefs
import data

pfile = None


def newcity():

    freeclusters = []

    for c in data.clusters.keys():
        isfree = 1
        for ct in data.clusters[c]:
            if ct["nation"] > 0:
                isfree = 0
                break
        if isfree:
            freeclusters.append(data.clusters[c])

    if len(freeclusters) == 0:   # any city will do... 

        cc = 0

        for c in data.cities:
            if c["nation"] == 0:
                cc += 1

        if cc == 0:
            return -1

        c = roll(cc)

        for i in range(len(data.cities)):
            if data.cities[i]["nation"] == 0:
                if c == 0:
                    return i
                else:
                    c -= 1

        # should not get here... 

        return -1

    else:        # find an empty cluster... 
        c = random.choice(freeclusters)
        c = random.choice(c)["cityno"]
        return c


# set directory 

if(argc > 1)
    if(chdir(argv[1]) == -1) {
        move(20,18);
        printw("Error Setting Directory %s\n", argv[1]);
        getch();
        endwin();
        exit(1);
    }

# set up screen 

initscr();

cbreak();
noecho();
nonl();
intrflush(stdscr,FALSE);

titlescreen();


# randomize 

srand48((long)time(NULL));


# load map file 

rc = loadmap();

if(rc != 0) {
    move(20,10);
    printw("Error Loading Map (%s)\n", errors[rc]);
    getch();
    endwin();
    exit(1);
}

# load game save 

rc = loadsave();

if(rc != 0) {
    move(20,10);
    printw("Error Loading Game Save (%s)\n", errors[rc]);
    getch();
    endwin();
    exit(1);
}

move(16,10);

if(world[0] != '\0') {
    addstr("World:  ");
    addstr(world);
    addstr("    ");
}

printw("Turn:  %d", gen);

# get player uid 

u = getenv("WARID");

if(u == NULL)
    u = getenv("UID");

if(u != NULL) 
    if(strncmp(u, "file ", 5) == 0) {
        # read UID from file 
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

#ifdef __MSDOS__
if(uid == 0) {
    inbuf[0] = '\0';
    move(20,10);
    addstr("Enter your UID:  ");
    clrtoeol();
    getstring(inbuf);
    uid = atoi(inbuf);
}
#endif

if(uid == 0) {
    move(20,10);
    addstr("UID Not Available; Aborting.");
    getch();
    endwin();
    exit(3);
}

# execute master file 

move(20,10);
addstr("Reading Master Commands...  ");
refresh();

fp = fopen(MASTERFL, "r");

if(fp != NULL) {
    for(i = 0; fgets(inbuf, 128, fp) != NULL; i++) {
        rc = execpriv(inbuf);
        if(rc == 0) {
            if((i+1) % 10 == 0) {
                move(20,48);
                printw("%3d lines", i+1);
                refresh();
            }
        } else {
            move(20,10);
            printw("Master Cmd Failed, Line %d, Code %d  ", i+1, rc);
            (void)getch();
            endwin();
            exit(2);
        }
    }

    fclose(fp);
}

# check if nation is entered. 

n = -1;

for(i = 0; i < nationcnt; i++)
    if(nations[i].uid == uid) {
        n = i;
        break;
    }

if(n == -1) { # nation not entered yet... 

    c = newcity();

    if(c >= 0) {

        fp = fopen(MASTERFL, "a");

        if(fp == NULL) {
            move(20,10);
            addstr("Can't Write Master Cmd's  ");
            clrtoeol();
            getch();
            endwin();
            exit(6);
        }

        n = nationcnt;
        confirmed = 0;

        while(!confirmed) {
            move(18,10);
            printw("Your City is %s", cities[c].name);
            clrtoeol();

            inbuf[0] = '\0';

            while(inbuf[0] == '\0') {
                move(20,10);
                addstr("(Enter ! to Retry)");
                clrtoeol();
                move(19,10);
                addstr("Enter your name:  ");
                clrtoeol();
                getstring(inbuf);
            }

            if(strcmp(inbuf, "!") == 0) {
                c = newcity();
            } else
                confirmed = 1;
        }

        strncpy(name, inbuf, NATIONNMLEN);
        name[NATIONNMLEN] = '\0';

        move(21,10);
        addstr("Available:  ");

        strcpy(inbuf, marks[0]);

        for(i = 0; i < nationcnt; i++)
            inbuf[nations[i].mark] = ' ';

        for(i = 0; inbuf[i]; i++)
            if(inbuf[i] != ' ')
                addch(inbuf[i]);

        move(20,10);
        addstr("Select Nation Mark:  ");
        clrtoeol();

        while(strchr(inbuf, ch = toupper(getch())) == NULL
        || ch == ' ');

        addch(ch);

        move(21,10);
        clrtoeol();

        for(i = 0; inbuf[i] && inbuf[i] != ch; i++);

        # create the nation... 

        sprintf(inbuf, "new-nation '%s' %d %d %d\n",
            name, uid, c, i);
        fputs(inbuf, fp);
        execpriv(inbuf);

        # ... and the hero... 

        t = cities[c].types[0];
        mtbl = ttypes[t].move_tbl;

        sprintf(inbuf, "make-army '%s' %d %d %d %d %d %d %d %d\n",
            name, n, cities[c].r, cities[c].c,
            roll(3)+4, roll(2)+1, 8, mtbl, 0);
        fputs(inbuf, fp);
        execpriv(inbuf);

        # ...and take the city. 

        sprintf(inbuf, "control-city %d %d\n", n, c);
        fputs(inbuf, fp);
        execpriv(inbuf);

        fclose(fp);

    } else {
        move(20,10);
        addstr("No Cities Available.  ");
        clrtoeol();
        getch();
        endwin();
        exit(5);
    }
}

# execute player commands 

move(20,10);
addstr("Reading Player Commands...  ");
clrtoeol();
refresh();

sprintf(filename, PLAYERFL, uid);

fp = fopen(filename, "r");

if(fp != NULL) {
    for(i = 0; fgets(inbuf, 128, fp) != NULL; i++) {
        rc = execpriv(inbuf);
        if(rc == 0) {
            if((i+1) % 10 == 0) {
                move(20,48);
                printw("%3d lines", i+1);
                refresh();
            }
        } else {
            move(20,10);
            printw("Player Cmd Failed, Line %d, Code %d  ", i+1, rc);
            (void)getch();
            endwin();
            exit(2);
        }
    }

    fclose(fp);
}

# append to player file... 

pfile = fopen(filename, "a");

if(pfile == NULL) {
    move(20,10);
    addstr("Can't Create or Append Player Cmd's  ");
    clrtoeol();
    getch();
    exit(4);
}

setbuf(pfile, NULL);

# main loop 

for(i = 0; i < nationcnt; i++)
    if(nations[i].uid == uid)
        n = i;

move(19,10);
printw("Welcome, %s of %s!", nations[n].name, nationcity(n));

clrtoeol();

move(20,10);
addstr("Press Any Key to Begin...  ");
clrtoeol();

getch();

mainscreen();

mainloop(n);

# clean up 

fclose(pfile);

endwin();

sys.exit(0)


# end of file. 
