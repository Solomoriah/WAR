#!/usr/bin/python

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

import sys, os.path, gd, string

sys.path.append("./python")

scale = 9
margin = 12

if len(sys.argv) < 2:
    print "Error, Arguments Required"
    sys.exit(0)

fp = open(sys.argv[1])

line = fp.readline()

lst = string.split(line.strip())
rows, cols = map(int, lst[1:3])
mark = lst[0]

fp.readline()

map = []

for i in range(rows):
    map.append(fp.readline())

pic = gd.image((cols * scale + (margin * 2 + 3), rows * scale + (margin * 2 + 3)))

black = pic.colorAllocate((0,0,0))
white = pic.colorAllocate((255,255,255))
red = pic.colorAllocate((255,0,0))
pic.filledRectangle((0,0), \
    (cols * scale + (margin * 2 + 3), rows * scale + (margin * 2 + 3)), white)

# terrain:
#  ~ water
#  . clear
#  % forest
#  # hills
#  ^ mountains

terrain = {
    "~":    pic.colorAllocate((216, 216, 255)), # blue
    ".":    pic.colorAllocate((196, 255, 196)), # light green
    "%":    pic.colorAllocate((128, 255, 128)), # green
    "#":    pic.colorAllocate((255, 204, 128)), # yellow
    "^":    pic.colorAllocate((196, 196, 196)), # grey
}

for row in range(rows):
    for col in range(cols):
        spot = map[row][col*2]
        if terrain.has_key(spot):
            c = terrain[spot]
        else:
            c = white
        if map[row][col*2+1] in string.uppercase \
        or map[row][col*2+1] == '!' \
        or map[row][col*2+1] == '*':
            c = black
        pic.filledRectangle((col*scale+(margin + 2), row*scale+(margin + 2)), \
            (col*scale+(scale+margin),row*scale+(scale+margin)), c)
        if map[row][col*2+1] != ' ':
            if c == black:
                p = white
            else:
                p = black
            pos = (col*scale+(margin + 4), row*scale+(margin + 2))
            pic.string(gd.gdFontTiny, pos, map[row][col*2+1], p)

pic.writePng(sys.argv[1]+".png")

# end of file.
