#!/usr/bin/env python

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


import sys, string

fp = open("map.orig", "r")

rows, cols = map(int, fp.readline().strip().split()[1:])

print (rows, cols)

# skip
fp.readline()

lines = fp.readlines()

if len(lines) != rows:
    print "Row Count Incorrect!"
    sys.exit(0)

lines = map(string.strip, lines)

if filter(lambda x: len(x) != cols, lines):
    print "Column Count Incorrect!"
    sys.exit(0)

fp.close()

# transform!

# 1. rotate top and bottom

lines = lines[rows/2:] + lines[:rows/2]

# 2. rotate left and right

lines = map(lambda x: x[cols/2:] + x[:cols/2], lines)

# 3. mirror

lines.reverse()

def strrev(x):
    x = list(x)
    x.reverse()
    return ''.join(x)

lines = map(strrev, lines)

fp = open("map.new", "w")

fp.write("M %d %d\n\n" % (rows, cols))

for l in lines:
    fp.write("%s\n" % l)

fp.close()

# end of file.
