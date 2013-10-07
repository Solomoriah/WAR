#!/usr/bin/env python

import gd

img = gd.image("newmap.png")

fp = open("map", "w")

w, h = img.size()
fp.write("M %d %d\n\n" % (h, w))

terrain = "~#^.%"

for r in range(h):
    for c in range(w):
        fp.write(terrain[img.getPixel((c, r))])
    fp.write("\n")

fp.close()

