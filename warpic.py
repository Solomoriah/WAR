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

import sys, os.path, gd

sys.path.append("./python")

import data

scale = 9
margin = 12

d = "."

if len(sys.argv) > 1:
    d = sys.argv[1]

os.chdir(d)

data.loadmap()
data.loadsave()

rows = len(data.mapdta) 
cols = len(data.mapdta[0])

pic = gd.image((cols * scale + (margin * 2 + 3) + 10, rows * scale + (margin * 2 + 3)))

black = pic.colorAllocate((0,0,0))
white = pic.colorAllocate((255,255,255))
pic.filledRectangle((0,0), \
    (cols * scale + (margin * 2 + 3) + 10, rows * scale + (margin * 2 + 3)), white)

# terrain:
#  ~ water
#  . clear
#  % forest
#  # hills
#  ^ mountains

terrain = {
    "~":    pic.colorAllocate((196, 196, 255)), # blue
    ".":    pic.colorAllocate((196, 255, 196)), # light green
    "%":    pic.colorAllocate((128, 255, 128)), # green
    "#":    pic.colorAllocate((255, 204, 128)), # yellow
    "^":    pic.colorAllocate((196, 196, 196)), # grey
}

for row in range(rows):
    for col in range(cols):
        spot = data.mapdta[row][col]
        if terrain.has_key(spot):
            c = terrain[spot]
        else:
            c = white
        if data.mapovl[row][col] in data.marks[0]:
            c = black
        pic.filledRectangle((col*scale+(margin + 2), row*scale+(margin + 2)), \
            (col*scale+(scale+margin),row*scale+(scale+margin)), c)

for city in data.cities:
    pos = (city["col"]*scale + margin + scale + 6, city["row"]*scale+ margin)
    pic.string(gd.gdFontSmall, pos, city["name"], white)
#    pos = (pos[0]-2, pos[1]-2)
#    pic.string(gd.gdFontSmall, pos, city["name"], black)
#    pos = (pos[0]+2, pos[1])
#    pic.string(gd.gdFontSmall, pos, city["name"], black)
#    pos = (pos[0], pos[1]+2)
#    pic.string(gd.gdFontSmall, pos, city["name"], black)
    pos = (pos[0]-1, pos[1]-1)
    pic.string(gd.gdFontSmall, pos, city["name"], black)

pic.writePng("map.png")

# end of file.
