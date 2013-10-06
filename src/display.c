/*
    Solomoriah's WAR!

    display.c -- display handlers

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
#include "warext.h"
#include "genio.h"
#include "function.h"
#include "data.h"
#include "display.h"
#include "dispsupp.h"

extern FILE *pfile;

int maxtrans = 3;
int trans = 0;

int gran = 2;

char *terminate = "\033q ";

struct {
    int r, c;
} groups[MAXARMIES+MAXCITIES];

int ngroups = 0;

struct aview armyview[12];
int avcnt = 0, avpnt = 0;

struct mv_stack movestack[10];
int mvcnt = 0;

struct {
    int nation;
    int armies, heros;
} enemies[2];

char *title[] = {
    "             S O L O M O R I A H ' S",
    "",
    "           #    #    ##    #####    ###",
    "           #    #   #  #   #    #   ###",
    "           #    #  #    #  #    #   ###",
    "           #    #  ######  #####     # ",
    "           # ## #  #    #  #   #       ",
    "           ##  ##  #    #  #    #   ###",
    "            #  #   #    #  #    #   ###",
    NULL
};

char *theeye[] = {
    "          +@@@@@@@'           ",
    "        #@@@@@@@+@@@#         ",
    "       @@@@@@@@@   ,@@@:+,    ",
    "     `@@@,@@@@@;    `@'       ",
    "    @@@@; :@@@+   ;.          ",
    "     @@@@                     ",
    "       ;@@`                   ",
};

char *instance();

char war_title[129] = "";


/* nation id must be located/created at a higher level. */

void mainloop(int ntn)
{
    int ch, r, c, i, n, city, army, force;
    char inbuf[16], buff[128];

    r = -1;
    c = -1;

    army = -1;

    /* find the player's capitol city */

    if(cities[nations[ntn].city].nation == ntn) {
        i = nations[ntn].city;
        r = cities[i].r;
        c = cities[i].c;
       setfocus(ntn, r, c);
    }

    /* find the player's first city */

    city = city_at(r, c);

    if(city != -1 && cities[city].nation != ntn)
        city = -1;

    if(city == -1)
        for(i = 0; i < citycnt; i++)
            if(cities[i].nation == ntn) {
                r = cities[i].r;
                c = cities[i].c;
                setfocus(ntn, r, c);
                break;
            }

    /* find the player's first army */

    if(city == -1)
        for(i = 0; i < armycnt; i++)
            if(armies[i].nation == ntn) {
                r = armies[i].r;
                c = armies[i].c;
                army = i;
             setfocus(ntn, r, c);
                break;
            }

    if(army == -1 && city == -1) {
        saystat("Nation is Destroyed.  Press Any Key:  ");
        ggetch();
        return;
    }

    /* enter the loop */

    saystat("Welcome to Solomoriah's WAR!  Press ? for Help.");

    force = 0;

    for(;;) {

        showmap(r, c, force);
        force = 0;

        showfocus(r, c, 1);
        ch = ggetch();
        showfocus(r, c, 0);

        clearstat(-1);

        switch(ch) {

        case 'a' : /* armies */
            city = city_at(r, c);
            if(city != -1 && cities[city].nation == ntn)
                produce(city);
            else
                saystat("Must View Your Own City First.");
            break;

        case '\016' : /* next army */
            if(!nextarmy(ntn, &r, &c))
                saystat("No Next Army with Movement Found");
            else
                setfocus(ntn, r, c);
            break;

        case 'n' : /* next group */
            if(!nextgroup(ntn, &r, &c)) {
                saystat("No More Groups Remain.");
            } else
                setfocus(ntn, r, c);
            break;

        case 'N' : /* last group */
            while(nextgroup(ntn, &r, &c));
            setfocus(ntn, r, c);
            break;

        case '\020' : /* prev army */
            if(!prevarmy(ntn, &r, &c))
                saystat("No Previous Army with Movement Found");
            else
                setfocus(ntn, r, c);
            break;

        case 'p' : /* previous group */
            if(!prevgroup(ntn, &r, &c)) {
                saystat("No Previous Groups Remain.");
            } else
                setfocus(ntn, r, c);
          break;

        case 'P' : /* first group */
            while(prevgroup(ntn, &r, &c));
            setfocus(ntn, r, c);
            break;

        case 'r' : /* rename hero */
            if(avcnt > 0 && armies[armyview[avpnt].id].name[0] == '\0') {
                saystat("Enter Hero's Name:  ");
                getstring(inbuf);

                sprintf(buff, "name-army %d '%s'\n",
                    armyview[avpnt].id, inbuf);
                fputs(buff, pfile);
                execpriv(buff);

                setfocus(ntn, r, c);
                clearstat(-1);
            } else
                saystat("Can Only Rename Heros.");
            break;

        case 'j' : /* next army */
        case 'd' :
        case GKEY_DOWN:
            if(avcnt > 0) {
                avpnt++;
             avpnt += avcnt;
                avpnt %= avcnt;
            } else
                saystat("No Armies!");
            break;

        case 'k' : /* previous army */
        case 'u' :
        case GKEY_UP:
            if(avcnt > 0) {
                avpnt--;
                avpnt += avcnt;
                avpnt %= avcnt;
            } else
                saystat("No Armies!");
            break;

        case ' ' : /* mark */
            if(avcnt > 0)
                armyview[avpnt].mark =
                    armyview[avpnt].mark ? 0 : 1;
            break;

        case 'I' : /* army information */
            if(avcnt > 0) {
                int id;
                id = armyview[avpnt].id;
                sprintf(buff, "%s: Combat %d / Hero %d %s",
                    armyname(id), armies[id].combat, armies[id].hero,
                    ((armies[id].hero > 0 && armies[id].eparm1) ? "(Loyal)" : ""));
                saystat(buff);
            } else
                saystat("No Armies!");
            break;

        case '*' : /* mark all */
        case 'g' : /* mark all and move */
            for(i = 0; i < avcnt; i++)
                if(armies[armyview[i].id].move_left > 0)
                    armyview[i].mark = 1;
            if(ch == '*')
                break;
            /* fall through */

        case 'm' : /* move */
            move_mode(ntn, &r, &c);
            setfocus(ntn, r, c);
            break;

        case '/' : /* unmark all */
            for(i = 0; i < avcnt; i++)
                armyview[i].mark = 0;
            break;

        case 'i' : /* information */
        case '1' :
        case '2' :
        case '3' :
        case '4' :
        case '5' :
        case '6' :
        case '7' :
        case '8' :
        case '9' :
            info_mode(&r, &c, ntn, ch);
            setfocus(ntn, r, c);
            break;

        case 'R' : /* read... */
            if(gen > 0) {
                reader(NEWSFL, 0);
                force = 1;
            } else
                saystat("No News.");
            break;

        case 'M' : /* mail... */
            sprintf(inbuf, MAILFL, ntn);
            reader(inbuf, 1);
          force = 1;
            break;

        case 'S' : /* send mail */
            saystat("Send Mail -- Enter Ruler's Name, or All to Post News:  ");
            getstring(inbuf);

            n = -1;
            for(i = 0; i < nationcnt; i++)
                if(strcasecmp(nations[i].name, inbuf) == 0)
                    n = i;

            if(n == -1 && strcasecmp("all", inbuf) == 0)
                n = 0;

            if(n == -1) {
                saystat("No Such Ruler.");
                break;
            }

            mailer(ntn, n);

            force = 1;
            clearstat(-1);
            break;

       case 's' : /* status */
            status();
            force = 1;
            break;

        case 'h' : /* help */
        case '?' :
            help();
            break;

        case '\f' :
            grefresh();
            break;

        case 'q' : /* quit */
            saystat("Quit?  (Y/N)  ");
            if(tolower(ggetch()) == 'y')
                return;
            clearstat(-1);
            break;
       }
    }
}


void setfocus(int ntn, int r, int c)
{
    int i, j, e;

    avcnt = 0;

    /* clear the enemies list */

    for(e = 0; e < 2; e++) {
        enemies[e].nation = -1;
        enemies[e].armies = 0;
        enemies[e].heros = 0;
    }

    /* clear the map overlay */

    for(i = 0; i < map_height; i++)
        for(j = 0; j < map_height; j++)
            mapovl[i][j] = ' ';

    /* find the player's armies */

    for(i = 0; i < armycnt; i++) {
        if(r == armies[i].r
        && c == armies[i].c)
            if(armies[i].nation == ntn || ntn == -1) {
                armyview[avcnt].id = i;
                armyview[avcnt++].mark = mark_of(i);
            } else {

                e = 0;

                if(enemies[0].nation != armies[i].nation
                && enemies[0].nation != -1)
                    e = 1;

                enemies[e].nation = armies[i].nation;

                if(armies[i].hero > 0)
                    enemies[e].heros++;
                else
                    enemies[e].armies++;
            }

        if(armies[i].nation >= 0) {
            if(mapovl[armies[i].r][armies[i].c] == ' '
            || mapovl[armies[i].r][armies[i].c] ==
               marks[1][nations[armies[i].nation].mark])
                mapovl[armies[i].r][armies[i].c] =
                    marks[1][nations[armies[i].nation].mark];
            else
                mapovl[armies[i].r][armies[i].c] = '!';
        }
    }

    sortview();

    /* update map overlay with cities */

    for(i = 0; i < citycnt; i++) {
        if(mapovl[cities[i].r][cities[i].c] == ' '
        || strchr(marks[1], mapovl[cities[i].r][cities[i].c]) != NULL)
            mapovl[cities[i].r][cities[i].c] =
                marks[0][nations[cities[i].nation].mark];
        else
            mapovl[cities[i].r][cities[i].c] = '!';
    }
}


int mapspot(int r, int c)
{
    if(mapovl[r][c] != ' ')
        return mapovl[r][c];

    return (map[r][c] == '~' ? ' ' : map[r][c]);
}


void showmap(int r, int c, int force)
{
    int ul_r, ul_c, f_r, f_c;
    int i, j, zr, zc;
    int rem;
    static old_ul_r = -1, old_ul_c = -1;

    showarmies();

    rem = (16 - gran) / 2;
    f_r = r % gran;
    f_c = c % gran;

    ul_r = (((r / gran) * gran - rem) + map_height) % map_height;
    ul_c = (((c / gran) * gran - rem) + map_width) % map_width;

    if(old_ul_r != ul_r || old_ul_c != ul_c || force)
        for(i = 0; i < 16; i++)
            for(j = 0; j < 16; j++) {
                zr = (ul_r+i) % map_height;
                zc = (ul_c+j) % map_width;
                gmapspot(i, j, map[zr][zc], mapovl[zr][zc], 0);
            }


    old_ul_r = ul_r;
    old_ul_c = ul_c;

    if(mapovl[r][c] != ' ')
        showcity(r, c);

    gmove(f_r + 7, f_c * 2 + 15);
}


void showfocus(int r, int c, int mode)
{
    int f_r, f_c, rem;

    rem = (16 - gran) / 2;

    f_r = (r % gran) + rem;
    f_c = (c % gran) + rem;

    gmapspot(f_r, f_c, map[r][c], mapovl[r][c], 1);
}


void showcity(int r, int c)
{
    int i;
    char buff[64];

    buff[0] = '\0';

    gmove(18, 3);

    i = city_at(r, c);

    if(i != -1) {
        sprintf(buff, "City:  %s (%s)",
            cities[i].name, cityowner(i));
    }
    
    gprintf("%-40.40s", buff);
}


void showarmies()
{
    int i, a;
    char buff[64];

    for(i = 0; i < 12; i++) {

        gmove(i + 2, 37);

        if(i < avcnt) {
            a = armyview[i].id;
            gprintf("%s %c ",
                i == avpnt ? "=>" : "  ",
                armyview[i].mark ? '*' : ' ');
            strcpy(buff, armyname(a));

            if(armies[a].name[0] == '\0' && armies[a].hero > 0)
                strcpy(buff, "(Nameless Hero)");
            else if(armies[a].hero > 0)
                strcat(buff, " (Hero)");

            gprintf("%-31.31s %d:%d", buff,
                armies[a].move_rate, armies[a].move_left);
        }

        gclrline();
    }

    gmove(14, 37);
    if(avcnt > 0)
        gprintf("     Total %d Armies", avcnt);
    gclrline();

    for(i = 0; i < 2; i++) {
        gmove(i + 15, 42);

        if(enemies[i].nation != -1)
            gprintf("%-15.15s %3d Armies, %3d Heros",
                i == 1
                    ? "(Other Nations)"
                    : nationcity(enemies[i].nation),
                enemies[i].armies, enemies[i].heros);

        gclrline();
    }
}


void sortview()
{
    int gap, i, j, temp;

    avpnt = 0;

    for(gap = avcnt / 2; gap > 0; gap /= 2)
        for(i = gap; i < avcnt; i++)
            for(j = i - gap; j >= 0 && isgreater(armyview[j+gap].id, armyview[j].id); j -= gap) {
                temp = armyview[j].id;
                armyview[j].id = armyview[j+gap].id;
                armyview[j+gap].id = temp;
                temp = armyview[j].mark;
                armyview[j].mark = armyview[j+gap].mark;
                armyview[j+gap].mark = temp;
            }
}


void analyze_stack(struct mv_stack *ms, int mc)
{
    int i, j, a, b, mmode;

    mmode = 0;

    for(i = 0; i < mc; i++)
        if(armies[ms[i].id].special_mv > mmode)
            mmode = armies[ms[i].id].special_mv;

    switch(mmode) {

    case TRANS_ALL :

        /* locate fastest transporter */
        j = -1;
        for(i = 0; i < mc; i++) {
            a = ms[i].id;
            if(armies[a].special_mv == TRANS_ALL) {
                if(j == -1 || armies[a].move_left > armies[j].move_left)
                    j = a;
                ms[i].dep = a;
            }
        }

        /* set (almost) all */
        for(i = 0; i < mc; i++) {
            a = ms[i].id;
            if(armies[a].special_mv == TRANS_ALL)
                ms[i].dep = a;
            else if(ms[i].dep == -1)
                ms[i].dep = j;
        }

        break;

    case TRANS_SELF :

        /* easy... all move by themselves */
        for(i = 0; i < mc; i++)
            ms[i].dep = ms[i].id;

        break;

    default :
        /* transport hero and/or one army */

        /* set all heros for transportation */
        for(i = 0; i < mc; i++) {
            a = ms[i].id;
            if(armies[a].hero > 0)
                for(j = 0; j < mc; j++) {
                    b = ms[j].id;
                    if(armies[b].special_mv == TRANS_HERO
                    && ms[j].dep == -1) {
                        ms[j].dep = b;
                        ms[i].dep = b;
                        break;
                    }
                }
        }

        /* set remaining for transportation */
        for(i = 0; i < mc; i++) {
            a = ms[i].id;
            if(ms[i].dep == -1
            && armies[a].special_mv == TRANS_SELF) {
                for(j = 0; j < mc; j++) {
                    b = ms[j].id;
                    if(armies[b].special_mv == TRANS_ONE
                    && ms[j].dep == -1) {
                        ms[j].dep = b;
                        ms[i].dep = b;
                        break;
                    }
                }
            }
        }

        /* leftovers transport self. */
        for(i = 0; i < mc; i++)
            if(ms[i].dep == -1)
                ms[i].dep = ms[i].id;

        break;
    }
}


void move_mode(int ntn, int *rp, int *cp)
{
    int i, j, mv, ch, city, army, t_r, t_c, a, b, ok, cnt, max, flag;
    int ac, hc, mvc[TRANS_ALL+1], mmode;
    int gap, temp;
    struct mv_stack unmarked[12];
    int uncnt;

    char buff[80];

    if(avcnt < 1) {
        saystat("No Army to Move.");
        return;
    }

    mvcnt = 0;
    flag = 0;

    for(i = 0; i < avcnt; i++) {
        if(armyview[i].mark)
            flag = 1;
        if(armyview[i].mark
        && (armies[armyview[i].id].move_left > 0 || ntn == -1)) {
            if(mvcnt >= 10 && ntn >= 0) {
                saystat("Too Many Armies for Group.");
                return;
            }
            movestack[mvcnt].moved = 0;
            movestack[mvcnt].dep = -1;
            movestack[mvcnt++].id = armyview[i].id;
        }
    }

    if(mvcnt < 1 && !flag
    && (armies[armyview[avpnt].id].move_left > 0 || ntn == -1)) {
        movestack[mvcnt].moved = 0;
        movestack[mvcnt].dep = -1;
        movestack[mvcnt++].id = armyview[avpnt].id;
        armyview[avpnt].mark = 1;
    }

    if(mvcnt < 1 && ntn > -1) {
        saystat("Armies Have No Movement Left.");
        return;
    }

    /* 
        prevent stranding:  
            analyze the unmarked armies
    
        create a "movestack" of the unmarked.
        if they can move, stranding can't occur.
    */

    if(ntn > -1) {

        uncnt = 0;

        for(i = 0; i < avcnt; i++) {
            if(!armyview[i].mark) {
                unmarked[uncnt].moved = 0;
                unmarked[uncnt].dep = -1;
                unmarked[uncnt++].id = armyview[i].id;
            }
        }

        analyze_stack(unmarked, uncnt);

        for(i = 0; i < uncnt; i++) {
            a = unmarked[i].id;
            if(unmarked[i].dep == a
            && armies[a].special_mv != TRANS_ALL
            && movecost(a, armies[a].r, armies[a].c) == 0) {
                saystat("Armies Would Be Stranded...  Movement Cancelled.");
                return;
            }
        }
    }

    /* analyze move stack */

    if(ntn > -1)
        analyze_stack(movestack, mvcnt);

    /* perform movement */

    clearstat(-1);

    gmove(21, 1);
    gprintf("Move %s", mvcnt > 1 ? "Armies" : "Army");

    gmove(22, 20);
    gputs("4   6  or  h   l");

    gmove(23, 20);
    gputs("1 2 3      n j m");

    gmove(21, 20);
    gputs("7 8 9      y k u    SPACE to Stop.  ");

    setfocus(ntn, *rp, *cp);
    showmap(*rp, *cp, 0);
    showfocus(*rp, *cp, 1);

    while(mvcnt > 0 && (ch = ggetch()) != ' ' 
    && strchr(terminate, ch) == NULL) {

        showfocus(*rp, *cp, 0);
        clearstat(0);

        t_r = *rp;
        t_c = *cp;

        switch(ch) { /* directions */

        case '7' :
        case 'y' :
            t_r--;
            t_c--;
          break;

        case '8' :
        case 'k' :
            t_r--;
            break;

        case '9' :
        case 'u' :
            t_r--;
            t_c++;
            break;

        case '4' :
        case 'h' :
            t_c--;
            break;

        case '6' :
        case 'l' :
            t_c++;
            break;

        case '1' :
        case 'n' :
            t_r++;
            t_c--;
          break;

        case '2' :
        case 'j' :
            t_r++;
            break;

        case '3' :
        case 'm' :
            t_r++;
            t_c++;
            break;

        case '\f' :
            grefresh();
            break;
        }

        t_r = fixrow(t_r);
        t_c = fixcol(t_c);

        if(t_r != *rp || t_c != *cp) {
            /* actual move code... */

            ok = 1;

            /* verify that all units can make the move. */

            if(ntn > -1) {

                for(i = 0; i < mvcnt; i++) {

                    a = movestack[i].id;

                    if(movestack[i].dep == a) {

                        mv = movecost(a, t_r, t_c);

                        if(mv > armies[a].move_left
                        || mv == 0) {
                            ok = 0;
                            sprintf(buff,
                                "%s Can't Move There.",
                                armyname(a));
                            saystat(buff);
                            break;
                        }
                    }
                }
            }

            /* prevent overstacking. */

            if(ok && ntn > -1) {
                cnt = 0;

                for(i = 0; i < armycnt; i++)
                if(armies[i].nation == ntn
                    && armies[i].r == t_r
                    && armies[i].c == t_c)
                        cnt++;

                city = city_at(t_r, t_c);

                max = 10;

                if(city != -1
                && cities[city].nation == ntn)
                    max = 12;

                if(mvcnt + cnt > max) {
                    ok = 0;
                    saystat("Too Many Armies There.");
                }
            }

            /* can't leave combat. */

            if(ok && ntn > -1)
                for(i = 0; i < armycnt; i++)
                    if(armies[i].nation != ntn
                    && armies[i].r == *rp
                    && armies[i].c == *cp) {
                        ok = 0;
                    saystat("Can't Leave Combat.");
                        break;
                    }

            /* do it! */

            if(ok) {
                for(i = 0; i < mvcnt; i++) {
                    a = movestack[i].id;

                    if(ntn > -1) {
                        if(movestack[i].dep == a)
                            mv = movecost(a, t_r, t_c);
                        else
                            mv = armies[a].move_left ? 1 : 0;
                    } else
                        mv = 0;

                    movestack[i].moved += mv;
                    armies[a].move_left -= mv;
                    armies[a].r = t_r;
                    armies[a].c = t_c;
                }
                *rp = t_r;
                *cp = t_c;
            }

            /* redo screen. */

            setfocus(ntn, *rp, *cp);
            showmap(*rp, *cp, ok);
        }

        showfocus(*rp, *cp, 1);
    }

    if(ntn > -1)
        for(i = 0; i < mvcnt; i++)
            if(movestack[i].moved > 0 || ntn == -1)
                fprintf(pfile, "move-army %d %d %d %d %d\n",
                    movestack[i].id, movestack[i].moved,
                    *rp, *cp, movestack[i].dep);

    clearstat(-1);

    mvcnt = 0;
}


int mark_of(int a)
{
    int i;

    if(mvcnt < 1)
        return 0;

    for(i = 0; i < mvcnt; i++)
        if(movestack[i].id == a)
            return 1;

    return 0;
}


void mainscreen()
{
    int i;

    gerase(0);

    gmove(0, 1);    gputch('+');
    gmove(17, 1);   gputch('+');
    gmove(0, 34);   gputch('-'); gputch('+');
    gmove(17, 34);  gputch('-'); gputch('+');

    for(i = 0; i < 16; i++) {
        gmove(i+1, 1);
        gputch('|');
        gmove(i+1, 34);
        gputch(' ');
        gputch('|');
        gmove(0, i*2+2);
        gputs("--");
        gmove(17, i*2+2);
        gputs("--");
    }

    gmove(19,0);
    gputs("----------------------------------------");
    gputs("----------------------------------------");

    gmove(0, 40);
    ghilite(1);
    gprintf("   %-20s    Turn %d   ", world, gen);
    ghilite(0);
}


void titlescreen()
{
    int i;

    gerase(0);

    for(i = 0; title[i] != NULL; i++) {
        gmove(i+2,10);
        gputs(title[i]);
    }

    gmove(i+3,10);
    gprintf("WAR Version %d.%d               \"Code by Solomoriah\"",
        major_ver, minor_ver);

    gmove(i+4,10);
    gputs("Copyright 1993, 1994, 2001, Chris Gonnerman");

    gmove(i+5,10);
    gputs("All Rights Reserved.");
}


void produce(int city)
{
    int i, t, ch, ofs;
    char buff[80], okstring[6];

    gmove(21, 1);  gclrline();
    gmove(22, 1);  gclrline();

    memset(okstring, 0, 5);
    ofs = 0;

    for(i = 0; i < cities[city].ntypes; i++) {
        if(cities[city].types[i] != -1) {
            gmove((i % 2) + 21, (i / 2) * 30 + 1);
            gprintf("%d) %-14.14s (%d)", i + 1,
                ttypes[cities[city].types[i]].name,
                cities[city].times[i]);
            okstring[ofs++] = '1' + i;
        }
    }

    gmove(23, 1);
    t = cities[city].types[cities[city].prod_type];
    gprintf("Current:  %s (%d turns left)    ESC to Cancel",
        ttypes[t].name,
        cities[city].turns_left);
    gclrline();

    sprintf(buff, "Set Army Production for %s:  ", cities[city].name);
    saystat(buff);

    if(strchr(okstring, ch = ggetch()) != NULL) {
        gputch(ch);
        sprintf(buff, "set-produce %d %d\n", city, ch - '1');
        fputs(buff, pfile);
        execpriv(buff);
    }

    clearstat(-1);
}


void info_mode(int *rp, int *cp, int n, int ch)
{
    int done, r, c, ul_r, ul_c, f_r, f_c, t_r, t_c;
    int city, army, i, flag;
    int rem;

    rem = (16 - gran) / 2;

    showfocus(*rp, *cp, 1);

    clearstat(-1);

    gmove(22, 20);
    gputs("4   6  or  h   l");

    gmove(23, 20);
    gputs("1 2 3      n j m");

    gmove(21, 1);
    gputs("Info Mode:         7 8 9      y k u    ESC to Stop.  ");

    r = *rp;
    c = *cp;

    ul_r = (((r / gran) * gran - rem) + map_height) % map_height;
    ul_c = (((c / gran) * gran - rem) + map_width) % map_width;

    f_r = (r % gran) + rem;
    f_c = (c % gran) + rem;

    done = 0;

    do {
        flag = 1;

        switch(ch) {

        case '7' :
        case 'y' :
            f_r--;
            f_c--;
            break;

        case '8' :
        case 'k' :
            f_r--;
            break;

        case '9' :
        case 'u' :
            f_r--;
            f_c++;
            break;

        case '6' :
        case 'l' :
            f_c++;
            break;

        case '3' :
        case 'm' :
            f_r++;
            f_c++;
            break;

        case '2' :
        case 'j' :
            f_r++;
            break;

        case '1' :
        case 'n' :
            f_r++;
            f_c--;
            break;

        case '4' :
        case 'h' :
            f_c--;
            break;

        case '\f' :
            grefresh();
            break;

        default :
            done = 1;
        }

        if(f_r < 0)  f_r = 0;
        if(f_c < 0)  f_c = 0;

        if(f_r > 15) f_r = 15;
        if(f_c > 15) f_c = 15;

        t_r = (ul_r + f_r + map_height) % map_height;
        t_c = (ul_c + f_c + map_width) % map_width;

        city = my_city_at(t_r, t_c, n);
        army = my_army_at(t_r, t_c, n);

        if(city >= 0 || army >= 0) {
            showfocus(*rp, *cp, 0);
            r = *rp = t_r;
            c = *cp = t_c;
            setfocus(n, *rp, *cp);
            showmap(*rp, *cp, 0);
            showfocus(*rp, *cp, 1);
            ul_r = (((r / gran) * gran - rem) + map_height)
                % map_height;
            ul_c = (((c / gran) * gran - rem) + map_width)
                % map_width;
            f_r = (r % gran) + rem;
            f_c = (c % gran) + rem;
            done = 1;
        }

        if(flag)
            show_info((ul_r + f_r + map_height) % map_height,
                    (ul_c + f_c + map_width) % map_width);

        gmove(f_r + 1, f_c * 2 + 3);

        if(!done)
            ch = ggetch();

    } while(!done);

    clearstat(-1);

    showfocus(*rp, *cp, 0);
}


/* end of file. */
