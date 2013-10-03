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

d = "."

if len(sys.argv) > 1:
    d = sys.argv[1]

os.chdir(d)

data.loadmap()
data.loadsave()

for nation in data.nations:
    nation["cities"] = 0
    nation["armies"] = 0
    nation["heroes"] = 0

for city in data.cities:
    if city["nation"] > 0:
        data.nations[city["nation"]]["cities"] += 1

for army in data.armies:
    if army["nation"] > 0:
        if army["hero"] > 0:
            data.nations[army["nation"]]["heroes"] += 1
        else:
            data.nations[army["nation"]]["armies"] += 1

output = [
    "<center>",
    "<table border=0 cellspacing=4 cellpadding=0 width=90%>"
]

elems = [
    "City",
    "Ruler", "No. Cities", 
    "No. Heroes", "No. Armies",
]

output.append("<tr>")
for e in elems:
    output.append("<td width=20%%><b>%s</b></td>" % e)
output.append("</tr>\n")

for nation in data.nations[1:]:
    elems = [
        data.cities[nation["city"]]["name"],
        nation["name"], nation["cities"],
        nation["heroes"], nation["armies"]
    ]
    output.append("<tr>")
    for e in elems:
        output.append("<td>%s</td>" % e)
    output.append("</tr>\n")

output.append("</table>")
output.append("</center>")

print '\n'.join(output)

# end of file.
