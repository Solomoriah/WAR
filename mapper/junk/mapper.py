#!/usr/bin/python

"mapper -- map generator"

import os, sys, time, random

Map = []
MapB = []

for i in range(160):
    Map.append([None]*160)
    MapB.append([None]*160)

maprows = mapcols = 0

terrain = "~.%#^"
count = [0]*5

################################################################

def lo3r(Max):

    return random.randint(0, Max - 1)

    res1 = random.randint(0, Max - 1)
    res2 = random.randint(0, Max - 1)
    res3 = random.randint(0, Max - 1)

    return min(res1, res2, res3)

################################################################

def fixcol(col):
    return (col + mapcols) % mapcols

################################################################

def fixrow(row):
    return (row + maprows) % maprows

################################################################

def genmap():

    print "genmap"

    i = j = 0

    for i in range(0, maprows):
        for j in range(0, mapcols):
            Map[i][j] = terrain[lo3r(5)]

#    for i in range(0, 4):
#        for j in range(0, mapcols):
#            Map[i][j] = '~'
#            Map[maprows-i-1][j] = '~'
#        }
#
#    for j in range(0, 4):
#        for i in range(0, maprows):
#            Map[i][j] = '~'
#            Map[i][mapcols-j-1] = '~'
#        }

################################################################

def savemap():

    print "savemap"

    fp = open("map.out", "w")

    fp.write("M %d %d\n" % (maprows / 16, mapcols / 16))

    for i in range(0, maprows / 16):
        for j in range(0, mapcols / 16):
            fp.write('\n')
            for k in range(0, 16):
                for l in range(0, 16):
                    fp.write(Map[i*16+k][j*16+l])
                if k == 0:
                    fp.write("  R%d C%d" % (i, j))
                fp.write('\n')

    fp.close()

    for i in range(0, maprows):
        for j in range(0, j < mapcols):
            c = Map[i][j]
            if c == '~':
                sys.stdout.write(' ')
            else:
                sys.stdout.write(c)
        print

################################################################

def main():

    print "main"

    gencount = 0
    xforms = 0

    genmap()

    gencount = 1

    xforms = generation() 

    while xforms > (maprows*mapcols/30) and gencount < 11:
        gencount += 1
        print "Generation %4d -- %5d Transforms\n" % (gencount, xforms)
        xforms = generation() 

    print "Generation %4d -- %5d Transforms\n" % (gencount, xforms)

    savemap()

################################################################

def docount(r, c, mode):

    pos = 0

    for i in range(0, 5):
        count[i] = 0
        if Map[r][c] == terrain[i]:
            pos = i

    for i in range(-1, 2):
        for j in range(-1, 2):
                for k in range(0, 5):
                    if Map[fixrow(r+i)][fixcol(c+j)] == terrain[k]:
                        diff = 1
                        if r == i and c == j:
                            diff = 2
                        if terrain[k] != '~' or mode:
                            count[k] += diff

#   if count[pos] >= pos:
#       return pos

    i = 0
    j = -1

    for k in range(1, 5):
        if count[k] > count[i]:
            i = k
        if count[k] > 5:
            j = k

#   if(!mode and j == -1)
#       return pos

    return i

################################################################

def generation():

    print "generation"

    trans = 0

    for i in range(0, maprows):
        for j in range(0, mapcols):

            if Map[i][j] == '~':

                pass
#               old = Map[i][j]
#               if(lo3r(15) == 0)
#                   Map[i][j] = terrain[lo3r(5)]
#               if(old != Map[i][j])
#                   trans += 1

            else:
                old = Map[i][j]
                MapB[i][j] = terrain[docount(i, j, 1)]
                if old != MapB[i][j]:
                    trans += 1


    for i in range(0, maprows):
        for j in range(0, mapcols):
            Map[i][j] = MapB[i][j]

    return trans

################################################################

if __name__ == "__main__":

    maprows = 96
    mapcols = 96

    main()

# end of file.
