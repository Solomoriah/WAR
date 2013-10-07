/*
    Solomoriah's WAR!

    warmap.c -- main procedure file for war map print

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

FILE *mfile = NULL;

void do_overlay();


int main(int argc, char *argv[])
{
    int rc, i, j, n, u;
    FILE *fp;
    char filename[64], inbuf[128], *p;
    int n_cities[MAXNATIONS], n_armies[MAXNATIONS], n_heros[MAXNATIONS];
    struct stat st_buf;


    /* set directory */

    if(argc > 1)
        if(chdir(argv[1]) == -1) {
            fprintf(stderr, "Error Setting Directory %s\n", argv[1]);
            exit(1);
        }
    
    /* load map file */

    rc = loadmap();

    if(rc != 0) {
        fprintf(stderr, "Error Loading Map (%s)\n", errors[rc]);
        exit(1);
    }

    /* load game save */

    rc = loadsave();

    if(rc != 0) {
        fprintf(stderr, "Error Loading Game Save (%s)\n", errors[rc]);
        exit(1);
    }

    /* execute master file */

    fp = fopen(MASTERFL, "r");

    if(fp != NULL) {
        for(i = 0; fgets(inbuf, 128, fp) != NULL; i++) {
            rc = execpriv(inbuf);
            if(rc != 0) {
                fprintf(stderr,
                    "Master Cmd Failed, Line %d, Code %d\n", i+1, rc);
                exit(2);
            }
        }

        fclose(fp);
    }

    do_overlay();

    /* print the map */

    fputs("   ", stdout);

    for(i = 0; i < map_width; i++)
        if(i % 10 == 0)
            putchar('0' + (i / 10) % 10);
        else
            putchar(' ');

    fputs("\n   ", stdout);

    for(i = 0; i < map_width; i++)
        putchar('0' + i % 10);

    putchar('\n');

    for(i = 0; i < map_height; i++) {

        if(i % 10 == 0)
            putchar('0' + (i / 10) % 10);
        else
            putchar(' ');

        putchar('0' + i % 10);
        putchar(' ');

        for(j = 0; j < map_width; j++)
            putchar(mapspot(i, j));

        putchar('\n');
    }

    /* analyze nations */

    for(i = 0; i < MAXNATIONS; i++) {
        n_cities[i] = 0;
        n_armies[i] = 0;
        n_heros[i] = 0;
    }

    for(i = 0; i < citycnt; i++)
        n_cities[cities[i].nation]++;

    for(i = 0; i < armycnt; i++)
        if(armies[i].hero)
            n_heros[armies[i].nation]++;
        else
            n_armies[armies[i].nation]++;

    /* print the nation list */

    puts("\nNations:\n");
    fputs("No.  Nation          Ruler           Mark  ", stdout);
    fputs("Cities  Heros   Armies\n", stdout);
    fputs("===  ==============  ==============  ====  ", stdout);
    fputs("======  ======  ======\n", stdout);

    for(i = 1; i < nationcnt; i++) {
        printf("%3d  %-14.14s  %-14.14s  %c     ",
            i, nationcity(i), nations[i].name,
            marks[0][nations[i].mark]);
        printf("  %3d     %3d     %3d\n",
            n_cities[i], n_heros[i], n_armies[i]);
    }

    /* print the city list */

    puts("\nCities:\n");

    for(i = 0; i < citycnt; i++) {
        j = cities[i].nation;
        printf("%3d  %-14.14s  %-14.14s  R%02d C%02d\n",
            i, cities[i].name,
            j != 0
                ? cities[nations[j].city].name
                : "",
            cities[i].r, cities[i].c);
    }

    exit(0);
}


int mapspot(int r, int c)
{
    if(mapovl[r][c] != ' ')
        return mapovl[r][c];

    return (map[r][c] == '~' ? ' ' : map[r][c]);
}


void do_overlay()
{
    int i, j;

    for(i = 0; i < map_height; i++)
        for(j = 0; j < map_height; j++)
            mapovl[i][j] = ' ';

    /* find the player's armies */

    for(i = 0; i < armycnt; i++) {
        if(mapovl[armies[i].r][armies[i].c] == ' '
        || mapovl[armies[i].r][armies[i].c] ==
           marks[1][nations[armies[i].nation].mark])
            mapovl[armies[i].r][armies[i].c] =
                marks[1][nations[armies[i].nation].mark];
        else
            mapovl[armies[i].r][armies[i].c] = '!';
    }

    for(i = 0; i < citycnt; i++) {
        if(mapovl[cities[i].r][cities[i].c] == ' '
        || strchr(marks[1], mapovl[cities[i].r][cities[i].c]) != NULL)
            mapovl[cities[i].r][cities[i].c] =
                marks[0][nations[cities[i].nation].mark];
        else
            mapovl[cities[i].r][cities[i].c] = '!';
    }
}


void nation_news(char *name, int city)
{
    ;
}


/* end of file. */
