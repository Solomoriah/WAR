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

# data.py -- data manager module

import os, string
from wardefs import *


###########################################################################
#  Data Model
###########################################################################

world = ""

mapdta = []
mapovl = []
nations = []
cities = []
ttypes = []
armies = []
move_table = []
clusters = {}

marks = [
    "*ABCDEFGHIJKLMNOPQRSTUVWXYZ",
    "@abcdefghijklmnopqrstuvwxyz",
]

special_moves = [
    "self",
    "hero",
    "army",
    "army stack",
]


# terrain characters
#  ~ water
#  . clear
#  % forest
#  # hills
#  ^ mountains

terrain = {
    "~":    "Water",
    ".":    "Plains",
    "%":    "Forest",
    "#":    "Hills",
    "^":    "Mountains",
}

# generation

gen = 0

class error(Exception):
    pass

def loadmap():
    global mapdta, mapovl

    # initialize map arrays

    mapdta = []
    mapovl = []

    fp = open(MAPFILE, "r")

    inbuf = fp.readline()

    if inbuf[0:2] != 'M ':
        fp.close()
        raise error, "invalid map file, bad magic line"

    rows, cols = map(int, inbuf.split()[1:3])

    if rows < 1 or cols < 1:
        fp.close()
        raise error, "invalid map file, rows or cols out of range"

    fp.readline() # skip one

    mapdta = map(string.rstrip, fp.readlines()[:rows])

    for i in range(rows):
        mapovl.append([" "] * cols)

    fp.close()


def loadsave():
    global armies, nations, cities, ttypes
    global gen, move_table, world, clusters

    armies = []

    # open the game save

    fp = open(GAMESAVE, "r")

    inbuf = fp.readline()

    if inbuf[0:2] != 'G ':
        fp.close()
        raise error, "invalid game save, bad magic line"

    lst = inbuf.split(None, 7)
    ncnt, cnt, ttcnt, acnt, mcnt = map(int, lst[1:6])
    
    if len(lst) > 6:
        gen = int(lst[6][1:])
    else:
        gen = 0

    if len(lst) == 8:
        world = string.replace(lst[7], "_", " ")
    else:
        world = ""

    if cnt < 1 or ttcnt < 1 or ncnt < 0 or acnt < 0:
        fp.close()
        raise error, "invalid game save, counts out of range"

    # load nations table

    if ncnt == 0:
        ncnt = 1
        nations.append({
            "name": "God",
            "uid": 0,
            "city": 0,
            "mark": 0,
            "idle_turns": 0
        })

    else:

        fp.readline() # skip one

        for i in range(ncnt):

            newnat = {}

            inbuf = fp.readline().rstrip()

            newnat["name"], inbuf = inbuf.split(":", 1)
            lst = map(lambda x: int(x[1:]), inbuf.split())

            newnat["uid"] = lst[0]
            newnat["city"] = lst[1]
            newnat["mark"] = lst[2]
            newnat["idle_turns"] = lst[3]

            nations.append(newnat)

    # load cities table

    for i in range(cnt):

        fp.readline() # skip one

        # city id line 

        inbuf = fp.readline()

        d = {}

        d["name"], inbuf = inbuf.split(":", 1)

        lst = inbuf.strip().split()

        d["cluster"] = int(lst[0][1:])
        d["nation"] = int(lst[1][1:])
        d["row"], d["col"] = map(int, lst[2][1:].split(":"))
        d["prod_type"], d["turns_left"] = map(int, lst[3][1:].split(":"))
        d["defense"] = int(lst[4][1:])

        # troop types 

        d["types"] = t = []

        for j in range(4):
            inbuf = fp.readline().strip()
            t.append(map(int, inbuf.split()))

        d["cityno"] = len(cities)

        cities.append(d)
        
        if not clusters.has_key(d["cluster"]):
            clusters[d["cluster"]] = []

        clusters[d["cluster"]].append(d)

    # load troop types table 

    fp.readline() # skip line

    for i in range(ttcnt):

        inbuf = fp.readline().strip()

        d = {}

        d["name"], inbuf = inbuf.split(":", 1)

        lst = inbuf.strip().split()

        d["combat"] = int(lst[0][1:])

        lst = map(int, lst[1][1:].split(":"))

        d["move_rate"] = lst[0]
        d["move_tbl"] = lst[1]
        d["special_mv"] = lst[2]

        ttypes.append(d)

    # load armies table 

    if acnt > 0:

        fp.readline() # skip one

        for i in range(acnt):

            inbuf = fp.readline().strip()

            d = {}

            d["name"], inbuf = inbuf.split(":", 1)

            # N%d @%d:%d C%d:%d M%d:%d S%d
            lst = inbuf.split()

            d["nation"] = int(lst[0][1:])
            d["row"], d["col"] = map(int, lst[1][1:].split(":"))
            d["combat"], d["hero"] = map(int, lst[2][1:].split(":"))
            d["move_rate"], d["move_tbl"] = map(int, lst[3][1:].split(":"))
            d["special_mv"] = int(lst[4][1:])
            d["move_left"] = d["move_rate"]

            armies.append(d)

    # load move table 

    fp.readline() # skip one

    for i in range(mcnt):

        inbuf = fp.readline().strip()

        lst = map(int, inbuf.split())

        move_table.append(lst)

    # link move table records to armies and troop types

    for a in armies:
        a["move_table"] = move_table[a["move_tbl"]]

    for t in ttypes:
        t["move_table"] = move_table[t["move_tbl"]]

    # add cities to map overlay 

    for c in cities:
        mapovl[c["row"]][c["col"]] = \
            marks[0][nations[c["nation"]]["mark"]]
        # mapdta[c["row"]] = mapdta[c["row"]][0:c["col"]] + \
        #     marks[0][nations[c["nation"]]["mark"]] + \
        #     mapdta[c["row"]][c["col"]+1:]

    # clean up 

    fp.close()


def roll(max):
    return randrange(max)


def city_at(r, c):

    for i in range(len(cities)):
        if cities[i]["row"] == r and cities[i]["col"] == c:
            return i

    return -1


def instance(n):
    if n > 10 and n < 20:
        return "%dth" % n
    if n % 10 == 1:
        return "%dst" % n
    if n % 10 == 2:
        return "%dnd" % n
    if n % 10 == 3:
        return "%drd" % n
    return "%dth" % n


def armyname(a):
    if armies[a]["name"][0] != '.':
        return armies[a]["name"]
    c, t, i = map(int, armies[a]["name"][1:].split("/"))
    if i < 1:
        return "????"
    buff = cities[c]["name"], instance(i), ttypes[t]["name"]
    return string.join(buff, " ")


def isgreater(a1, a2, mode):

    if mode == GT_DISPLAY:
        if armies[a1]["hero"] > armies[a2]["hero"]:
            return 1
        if armies[a1]["hero"] < armies[a2]["hero"]:
            return 0
        if armies[a1]["special_mv"] > armies[a2]["special_mv"]:
            return 1
        if armies[a1]["special_mv"] < armies[a2]["special_mv"]:
            return 0
    else:
        if armies[a1]["special_mv"] > armies[a2]["special_mv"]:
            return 1
        if armies[a1]["special_mv"] < armies[a2]["special_mv"]:
            return 0
        if armies[a1]["hero"] > armies[a2]["hero"]:
            return 1
        if armies[a1]["hero"] < armies[a2]["hero"]:
            return 0

    if armies[a1]["combat"] > armies[a2]["combat"]:
        return 1
    if armies[a1]["combat"] < armies[a2]["combat"]:
        return 0

    if armies[a1]["move_rate"] > armies[a2]["move_rate"]:
        return 1
    if armies[a1]["move_rate"] < armies[a2]["move_rate"]:
        return 0

    if armies[a1]["name"][0] != '.' and armies[a2]["name"][0] == '.':
        return 1

    if armies[a1]["name"][0] == '.' and armies[a2]["name"][0] != '.':
        return 0

    if armies[a1]["name"][0] != '.' and armies[a2]["name"][0] != '.':
        return armies[a1]["name"] > armies[a2]["name"]

    a1tup = tuple(map(int, armies[a1]["name"][1:].split("/")))
    a2tup = tuple(map(int, armies[a2]["name"][1:].split("/")))

    return a1tup > a2tup


def cityowner(c):

    n = cities[c]["nation"]

    if n == 0:
        return "Neutral"

    return cities[nations[n]["city"]]["name"]


def turn():
    return gen


def nationname(n):
    return nations[n]["name"]


def nationcity(n):
    if n == 0:
        return "Militia"
    return cities[nations[n]["city"]]["name"]


if __name__ == "__main__":

    os.chdir("game")
    loadmap()
    loadsave()

    for r in range(10):
        print mapdta[r][:10]

# end of file. 
