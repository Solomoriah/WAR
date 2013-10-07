/*
    Solomoriah's WAR!

    warnews.c -- news reader

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
#include <sys/stat.h>

#include "war.h"
#include "dispsupp.h"


int main(int argc, char *argv[])
{
    struct stat st_buf;
    char *gamedir, *p;

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
    
    ginit();

    reader(NEWSFL, 0);

    gend();

    exit(0);
}


void mainscreen()
{
    ;
}


void saystat(char *s)
{
    gmove(24, 0);
    gputs(s);
}


/* end of file. */
