/*
    Solomoriah's WAR!

    data.c -- data manager module

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
#include <string.h>

#include "war.h"
#include "function.h"
#include "data.h"


char *errors[] = {

    "No Error",                     /* WAR_NOERROR */

    "Open Error",                   /* WAR_OPENERROR */
    "Read Error",                   /* WAR_READERROR */
    "Invalid Header",               /* WAR_INVHEADER */
    "Unexpected EOF",               /* WAR_EOFERROR */
    "No Text on Line",              /* WAR_NOTEXTERR */

    "No Such Command",              /* WAR_NOSUCHCMD */
    "Memory Allocation Failed",     /* WAR_MALLOCERR */
    "Invalid Arguments",            /* WAR_INVARGS */
    "Can't Access Directory",       /* WAR_DIRACCERR */
    "Control File Exec Failed",     /* WAR_CFEXECFAIL */

    NULL
};


/* name of the game world */

char world[41];


/* the map */

typedef char map_row[MAXMAPSZ];
map_row *map = NULL;
map_row *mapovl = NULL;
int map_width = 0, map_height = 0;


/* nations table */

struct nation nations[MAXNATIONS+1];
int nationcnt = 0;

char *marks[] = {
    "*ABCDEFGHIJKLMNOPQRSTUVWXYZ?",
    "@abcdefghijklmnopqrstuvwxyz "
};

/* cities table */

struct city cities[MAXCITIES];
int citycnt = 0;

/* troop types table */

struct trooptype ttypes[MAXTTYPES];
int ttypecnt = 0;

/* army table */

struct army *armies;
int armycnt = 0;

/* move table */

struct movetbl move_table[MAXMOVENT];
int mvtcnt = 0;

/* terrain characters */

char *terrain = "~.%#^";        /* standard types only. */

char *terr_names[] = {
    "Water",
    "Plains",
    "Forest",
    "Hills",
    "Mountains",
};


/* generation */

int gen = 0;


int loadmap()
{
    FILE *fp;
    char inbuf[128];
    int rows, cols, i, j;

    /* initialize map arrays */

    map = (map_row *)malloc(sizeof(map_row) * MAXMAPSZ);
    mapovl = (map_row *)malloc(sizeof(map_row) * MAXMAPSZ);

    if(map == NULL || mapovl == NULL)
        return WAR_MALLOCERR;

    fp = fopen(MAPFILE, "r");

    if(fp == NULL)
        return WAR_OPENERROR;

    if(fgets(inbuf, 128, fp) == NULL) {
        fclose(fp);
        return WAR_READERROR;
    }

    if(inbuf[0] != 'M' || inbuf[1] != ' ') {
        fclose(fp);
        return WAR_INVHEADER;
    }

    sscanf(inbuf, "M %d %d", &rows, &cols);

    if(rows < 1 || cols < 1 || rows > MAXMAPSZ || cols > MAXMAPSZ) {
        fclose(fp);
        return WAR_INVHEADER;
    }

    map_width = cols;
    map_height = rows;

    fgets(inbuf, 128, fp);

    for(i = 0; i < rows; i++)
        for(j = 0; j < cols; j++) {
            map[i][j] = '~';
            mapovl[i][j] = ' ';
        }

    for(i = 0; i < rows; i++) {
        inbuf[0] = '\0';

        if(fgets(inbuf, 128, fp) == NULL) {
            fclose(fp);
            return WAR_EOFERROR;
        }

        for(j = 0; j < cols && inbuf[j]; j++)
            map[i][j] = inbuf[j] == ' ' ? '~' : inbuf[j];
    }

    fclose(fp);

    return WAR_NOERROR;
}


int loadsave()
{
    FILE *fp, *ofp;
    char inbuf[128], *p;
    int cnt, ttcnt, ncnt, acnt, mcnt, nt, i, j;
    size_t nbytes;
    int a_nation, a_r, a_c, a_combat, a_hero,
        a_move_rate, a_move_tbl, a_special_mv, a_eparm1;

    /* initialize armies table */

    nbytes = sizeof(struct army);
    armies = (struct army *)malloc(nbytes * MAXARMIES);

    if(armies == NULL)
        return WAR_MALLOCERR;

    /* open the game save */

    fp = fopen(GAMESAVE, "r");

    if(fp == NULL) {
        fp = fopen(GAMEORIG, "r");
        if(fp == NULL)
            return WAR_OPENERROR;
        /* create an empty save file, to make warupd happy. */
        ofp = fopen(GAMESAVE, "w");
        if(ofp == NULL) {
            fclose(fp);
            return WAR_OPENERROR;
        }
        fclose(ofp);
    }

    if(fgets(inbuf, 128, fp) == NULL) {
        fclose(fp);
        return WAR_READERROR;
    }

    if(inbuf[0] != 'G' || inbuf[1] != ' ') {
        fclose(fp);
        return WAR_INVHEADER;
    }

    cnt = ttcnt = ncnt = acnt = mcnt = -1;

    gen = 0;

    world[0] = '\0';

    sscanf(inbuf, "G %d %d %d %d %d g%d %40s",
        &ncnt, &cnt, &ttcnt, &acnt, &mcnt, &gen, world);

    if(cnt < 1 || cnt > MAXCITIES
    || ttcnt < 1 || ttcnt > MAXTTYPES
    || ncnt < 0 || ncnt > MAXNATIONS
    || acnt < 0 || acnt > MAXARMIES) {
        fclose(fp);
        return WAR_INVHEADER;
    }

    for(p = world; *p; p++)
        if(*p == '_')
            *p = ' ';

    /* load nations table */

    if(ncnt == 0) {
        ncnt = 1;
        strcpy(nations[0].name, "God");
        nations[0].uid = 0;
        nations[0].city = 0;
        nations[0].mark = 0;
        nations[0].idle_turns = 0;
    } else {

        fgets(inbuf, 128, fp);

        for(i = 0; i < ncnt; i++) {
            if(fgets(inbuf, 128, fp) == NULL) {
                fclose(fp);
                return WAR_EOFERROR;
            }

            p = strtok(inbuf, ":");
            strncpy(nations[i].name, p, NATIONNMLEN);
            nations[i].name[NATIONNMLEN] = '\0';

            p = strtok(NULL, "\n");
            sscanf(p, " U%d C%d M%d I%d",
                &(nations[i].uid),
                &(nations[i].city),
                &(nations[i].mark),
                &(nations[i].idle_turns));
        }
    }

    /* set up the Rogue nation */

    nations[27].uid = -1;
    nations[27].city = 0;
    nations[27].mark = 27;
    nations[27].idle_turns = 0;
    strcpy(nations[27].name, "Rogue");

    /* load cities table */

    for(i = 0; i < cnt; i++) {

        fgets(inbuf,128,fp);

        /* city id line */

        if(fgets(inbuf,128,fp) == NULL) {
            fclose(fp);
            return WAR_EOFERROR;
        }

        p = strtok(inbuf, ":");

        strcpy(cities[i].name, p);

        p = strtok(NULL, "\n");

        cities[i].r = 0;
        cities[i].c = 0;
        cities[i].prod_type = 0;
        cities[i].turns_left = 0;
        cities[i].defense = 0;
        cities[i].nation = 0;
        cities[i].cluster = -1;
        cities[i].ntypes = -1;

        sscanf(p, " C%d N%d @%d:%d P%d:%d D%d T%d",
            &(cities[i].cluster),
            &(cities[i].nation),
            &(cities[i].r), &(cities[i].c),
            &(cities[i].prod_type), &(cities[i].turns_left),
            &(cities[i].defense), &(cities[i].ntypes));

        /* troop types */

        nt = 0;

        for(j = 0; j < 4; j++) {

            cities[i].types[j] = -1;
            cities[i].times[j] = -1;
            cities[i].instance[j] = -1;

            if(fgets(inbuf,128,fp) == NULL) {
                fclose(fp);
                return WAR_EOFERROR;
            }

            if(inbuf[0] != '\n')
                sscanf(inbuf, "%d %d %d",
                    &(cities[i].types[j]),
                    &(cities[i].times[j]),
                    &(cities[i].instance[j]));

            if(cities[i].types[j] != -1)
                nt = j + 1;
        }

        if(cities[i].ntypes == -1)
            cities[i].ntypes = nt;
    }

    /* load troop types table */

    fgets(inbuf, 128, fp);

    for(i = 0; i < ttcnt; i++) {
        if(fgets(inbuf, 128, fp) == NULL) {
            fclose(fp);
            return WAR_EOFERROR;
        }

        p = strtok(inbuf, ":");

        strcpy(ttypes[i].name, p);

        p = strtok(NULL, "\n");

        ttypes[i].special_mv = TRANS_SELF;

        sscanf(p, " C%d M%d:%d:%d",
            &(ttypes[i].combat),
            &(ttypes[i].move_rate),
            &(ttypes[i].move_tbl),
            &(ttypes[i].special_mv));
    }

    /* load armies table */

    if(acnt > 0) {
        fgets(inbuf, 128, fp);

        for(i = 0; i < acnt; i++) {

            if(fgets(inbuf, 128, fp) == NULL) {
                fclose(fp);
                return WAR_EOFERROR;
            }

            if(inbuf[0] == ':')
                p = "";
            else
                p = strtok(inbuf, ":");
            strncpy(armies[i].name, p, ARMYNAMLEN);
            armies[i].name[ARMYNAMLEN] = '\0';

            a_nation = 0;
            a_r = -1;
            a_c = -1;
            a_combat = 0;
            a_hero = 0;
            a_move_rate = 0;
            a_move_tbl = 0;
            a_special_mv = 0;

            if(inbuf[0] == ':')
                p = inbuf + 1;
            else
                p = strtok(NULL, "\n");
            sscanf(p, " N%d @%d:%d C%d:%d M%d:%d S%d L%d",
                &a_nation, &a_r, &a_c,
                &a_combat, &a_hero,
                &a_move_rate, &a_move_tbl, &a_special_mv,
                &a_eparm1);

            armies[i].nation = a_nation;
            armies[i].r = a_r;
            armies[i].c = a_c;
            armies[i].combat = a_combat;
            armies[i].hero = a_hero;
            armies[i].move_rate = a_move_rate;
            armies[i].move_tbl = a_move_tbl;
            armies[i].special_mv = a_special_mv;
            armies[i].eparm1 = a_eparm1;

            armies[i].move_left = armies[i].move_rate;
        }
    }

    /* load move table */

    fgets(inbuf, 128, fp);

    for(i = 0; i < mcnt; i++) {

        if(fgets(inbuf, 128, fp) == NULL) {
            fclose(fp);
            return WAR_EOFERROR;
        }

        for(j = 0; j < TERR_TYPES; j++)
            move_table[i].cost[j] = 0;  /* cost 0 = can't do it. */

        p = strtok(inbuf, " ");

        for(j = 0; j < TERR_TYPES && p != NULL; j++) {
            move_table[i].cost[j] = atoi(p);
            p = strtok(NULL, " ");
        }
    }

    /* add cities to map overlay */

    for(i = 0; i < citycnt; i++) {
        if(mapovl[cities[i].r][cities[i].c] == ' '
        || strchr(marks[1], mapovl[cities[i].r][cities[i].c]) != NULL)
            mapovl[cities[i].r][cities[i].c] =
                marks[0][nations[cities[i].nation].mark];
        else
            mapovl[cities[i].r][cities[i].c] = '!';
    }

    /* clean up */

    citycnt = cnt;
    ttypecnt = ttcnt;
    nationcnt = ncnt;
    armycnt = acnt;
    mvtcnt = mcnt;

    fclose(fp);

    return WAR_NOERROR;
}


void rmnl(char *s)
{
    for(; *s; s++)
        if(*s == '\n')
            *s = '\0';
}


int roll(int max)
{
    long lrand48();

    return (int)lrand48() % max;
}


int my_army_at(int r, int c, int n)
{
    int i;

    for(i = 0; i < armycnt; i++)
        if(armies[i].nation == n
        && armies[i].r == r
        && armies[i].c == c)
            return i;

    return -1;
}


int my_city_at(int r, int c, int n)
{
    int i;

    for(i = 0; i < citycnt; i++)
        if((n < 0 || cities[i].nation == n)
        && cities[i].r == r
        && cities[i].c == c)
            return i;

    return -1;
}


int city_at(int r, int c)
{
    return my_city_at(r, c, -1);
}


char *instance(int n)
{
    static char buff[10];

    if(n > 10 && n < 20) {
        sprintf(buff, "%dth", n);
        return buff;
    }

    switch(n % 10) {
    case 1 :
        sprintf(buff, "%dst", n);
        break;

    case 2 :
        sprintf(buff, "%dnd", n);
        break;

    case 3 :
        sprintf(buff, "%drd", n);
        break;

    default :
        sprintf(buff, "%dth", n);
        break;
    }

    return buff;
}


char *cityowner(int c)
{
    int n;

    n = cities[c].nation;

    if(n == 0)  return "Neutral";
    if(n == 27) return "Rogue";

    return cities[nations[n].city].name;
}


int turn()
{
    return gen;
}


char *nationname(int n)
{
    return nations[n].name;
}


char *nationcity(int n)
{
    if(n == 0)  return "Militia";
    if(n == 27) return "Rogue";

    return cities[nations[n].city].name;
}


char *armyname(int a)
{
    int c, t, i;
    static char buff[ARMYNAMLEN+CTYNAMLEN+6];
    char *instance();

    if(armies[a].name[0] != '.')
        return armies[a].name;

    c = 0;
    t = 0;
    i = -1;

    sscanf(armies[a].name+1, "%d/%d/%d", &c, &t, &i);

    if(i < 1)
        return "????";

    strcpy(buff, cities[c].name);
    strcat(buff, " ");
    strcat(buff, instance(i));
    strcat(buff, " ");
    strcat(buff, ttypes[t].name);

    return buff;
}


/*
    used for sorting armies
*/
int isgreater(int a1, int a2)
{
    int c1, c2, t1, t2, i1, i2;
    char buf1[ARMYNAMLEN], buf2[ARMYNAMLEN];

    if(armies[a1].hero > armies[a2].hero) return 1;
    if(armies[a1].hero < armies[a2].hero) return 0;

    if(armies[a1].special_mv > armies[a2].special_mv) return 1;
    if(armies[a1].special_mv < armies[a2].special_mv) return 0;

    if(armies[a1].combat > armies[a2].combat) return 1;
    if(armies[a1].combat < armies[a2].combat) return 0;

    if(armies[a1].move_rate > armies[a2].move_rate) return 1;
    if(armies[a1].move_rate < armies[a2].move_rate) return 0;

    if(armies[a1].name[0] != '.' && armies[a2].name[0] == '.')
        return 1;

    if(armies[a1].name[0] == '.' && armies[a2].name[0] != '.')
        return 0;

    if(armies[a1].name[0] != '.' && armies[a2].name[0] != '.')
        if(strcmp(armies[a1].name, armies[a2].name) > 0)
            return 1;
        else
            return 0;

    sscanf(armies[a1].name, ".%d/%d/%d", &c1, &t1, &i1);
    sprintf(buf1, "%03d%03d%03d", c1, t1, 1000 - i1);

    sscanf(armies[a2].name, ".%d/%d/%d", &c2, &t2, &i2);
    sprintf(buf2, "%03d%03d%03d", c2, t2, 1000 - i2);

    if(strcmp(buf1, buf2) > 0)
        return 1;

    return 0;
}


/* end of file. */
