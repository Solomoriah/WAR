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

import sys, os.path, string

sys.path.append("./python")

import data

d = "."

if len(sys.argv) > 1:
    d = sys.argv[1]

os.chdir(d)

data.loadmap()
data.loadsave()

def say_stat(txt, val):
    rc = [ "<table border=0 cellspacing=2 cellpadding=0 width=100%>" ]
    rc.append("<tr><td>%s</td>" % txt)
    rc.append("<td align=right>%d</td></tr>" % val)
    rc.append("</table>")
    return string.join(rc, "\n")

for t in data.ttypes:

    print "\n<!-- Army Type %s -->" % t["name"]

    print "<p>\n<table border=1 cellspacing=0 cellpadding=3 width=450>"

    print "<tr>",
    print "<td><b>%s</b></td>" % t["name"],
    print "<td align=right colspan=2>\n%s\n</td>" \
        % say_stat("Combat", t["combat"]),
    print "<td align=right colspan=3>\n%s\n</td>" \
        % say_stat("Movement", t["move_rate"]),
    print "</tr>"

    print "<tr>",
    print "<td align=right>Move Costs</td>",
    for i in range(len(t["move_table"])):
        print "<td align=right width=13%%>\n%s\n</td>" \
            % say_stat(data.terrain.keys()[i], t["move_table"][i]),
    print "</tr>"

    if t["special_mv"] > 0:
        notes = "transports " + data.special_moves[t["special_mv"]]
        print "<tr>",
        print "<td align=right>Notes</td>",
        print "<td colspan=5>\n%s\n</td>" % notes,
        print "</tr>"

    print "</table>"

# end of file.
