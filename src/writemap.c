/*
    Solomoriah's WAR!

    writemap.c -- write the player's map

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

map_row *pmap = NULL;

extern int gran;

static void markup(int r, int c)
{
    int r_origin, c_origin, i, j, fixed_r, fixed_c;

    r_origin = (r / gran) * gran - 7;
    c_origin = (c / gran) * gran - 7;

    for(i = 0; i < 16; i++)
        for(j = 0; j < 16; j++) {
            fixed_r = fixrow(r_origin+i);
            fixed_c = fixcol(c_origin+j);
            if(pmap[fixed_r][fixed_c] == 0)
                pmap[fixed_r][fixed_c] = 3;
        }
}


int writemap(FILE *fp, int ntn)
{
    int r, c, i, j, m;

	pmap = (map_row *)malloc(sizeof(map_row) * MAXMAPSZ);

    /* clear the player map */

    for(c = 0; c < map_width; c++)
        for(r = 0; r < map_height; r++)
            pmap[r][c] = 0;

    /* find the nation's groups */

    for(i = 0; i < armycnt; i++)
        if(armies[i].nation == ntn) {
            r = armies[i].r;
            c = armies[i].c;
            pmap[r][c] = 1;
        }

    for(i = 0; i < citycnt; i++)
        if(cities[i].nation == ntn) {
            r = cities[i].r;
            c = cities[i].c;
            pmap[r][c] = 1;
        }

    /* mark up the overlay */

    for(c = 0; c < map_width; c++)
        for(r = 0; r < map_height; r++)
            if(pmap[r][c] == 1)
                markup(r, c);

    /* write map */

    fprintf(fp, "%c %d %d\n\n", 
        marks[0][nations[ntn].mark], map_height, map_width);

    for(r = 0; r < map_height; r++) {
        for(c = 0; c < map_width; c++)
            fprintf(fp, "%c%c%c", 
                map[r][c], mapovl[r][c], ('0' + pmap[r][c]));
        fputc('\n', fp);
    }
}


/* end of file. */
