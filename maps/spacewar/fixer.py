
import re, string

fp = open("game.sav", "r")
data = fp.readlines()
fp.close()

city_re = re.compile("[\w' ]+: C\d+ N\d @\d+:\d+ P\d+:\d+ D\d+ T\d+ *$")

out = open("game.out", "w")

for row in data:
    mo = city_re.match(row)
    if mo:
        name, data = string.split(row, ":", maxsplit = 1)
        name = name.strip()
        elems = string.split(data.strip())
        r, c = map(int, string.split(elems[2][1:], ":"))
        print name, r, c, "=>",
        r += 32
        r %= 64
        c += 32
        c %= 64
        print r, c
        elems[2] = "@%d:%d" % (r, c)
        data = string.join(elems)
        row = "%s: %s\n" % (name, data)
    out.write(row)

out.close()
