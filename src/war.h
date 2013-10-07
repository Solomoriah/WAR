/*
	Solomoriah's WAR!

	war.h -- data structures and functions

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


/* file names: */

#define CONTROLFILE "control.war"
#define MAPFILE     "map"
#define GAMESAVE    "game.sav"
#define GAMEORIG    "game.orig"
#define GAMEBAK     "game.%03d"
#define MASTERFL    "master.cmd"
#define MASTERBAK   "master.%03d"
#define PLAYERFL    "%d.cmd"
#define PLAYERMAP   "%d.map"
#define NEWSFL      "news"
#define MAILFL      "mail.%03d"
#define MAILLOG     "mail.log"
#define TEMPFL      "temp.%03d"

/* constants: */

#define MAXMAPSZ    160
#define MAXCITIES   150
#define MAXTTYPES   50
#define MAXNATIONS  27
#define MAXARMIES   2000
#define MAXMOVENT   16

#define TERR_TYPES  5

#define TIME_TO_DESERT  7

/* string lengths: */

#define CTYNAMLEN   14
#define TYPNAMLEN   14
#define ARMYNAMLEN  14
#define NATIONNMLEN 14

/* maximum troop-types per city: */

#define TYPECNT 4


/* special_mv values: */

#define TRANS_SELF  0
#define TRANS_HERO  1
#define TRANS_ONE   2
#define TRANS_ALL   3

/* isgreater() modes */

#define GT_COMBAT   1
#define GT_DISPLAY  2


struct nation {

	char name[NATIONNMLEN+1];
	int mark, city;

	int idle_turns;

	int uid;
};


struct city {

	char name[CTYNAMLEN+1];

	int r, c;                   /* map position.               */
	int nation;                 /* index of owning nation.     */
	int cluster;                /* cluster-id, used to assign. */

	/* army types */

	int ntypes;                 /* number of types available.  */
	int types[TYPECNT];         /* types that can be produced. */
	int times[TYPECNT];         /* time to produce each type.  */
	int instance[TYPECNT];      /* last instance produced.     */

	int prod_type, turns_left;  /* type of army being produced */
								/* and time left to produce.   */

	int defense;                /* amount to add to defenders. */
};


struct trooptype {
	char name[TYPNAMLEN+1];     /* i.e. Lt. Infantry.          */
	int combat;                 /* combat factor.              */
	int move_rate, move_tbl;    /* movement rate and table.    */
	int special_mv;             /* special movement type.      */
};


struct army {

	char name[ARMYNAMLEN+1];    /* hero name or army code.     */
	short nation;               /* nation number.              */
	short r, c;                 /* map location.               */

	unsigned char combat;       /* combat factor.              */
	unsigned char hero;         /* hero command value.         */
								/* value of 0 is non-hero,     */
								/* else is the value added to  */
								/* commanded troops.           */

	unsigned char eparm1;       /* PC:  loyalty of army        */
								/* 0 = Untested, 1 = Loyal     */
								/* NPC: is it a monster?       */
								/* 0 = Militia, 1 = Rogue      */

	unsigned char move_rate, move_tbl;
								/* movement rate and table.    */
	unsigned char move_left;    /* movement left this turn.    */
	unsigned char special_mv;   /* movement option.            */
};


struct movetbl {
	int cost[TERR_TYPES];
};


struct parse_tbl {
	char *command;
	int (*func)();
};

struct aview {
	int id;
	char mark;
};


/* end of file. */
