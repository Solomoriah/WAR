
CC = gcc
# CFLAGS = -g -DCURSES_DRIVER -DANSI_DRIVER -DWARTERM_DRIVER -DAUTHPGM='"/home/$(USER)/bin/authpgm \"%s\" \"%s\""'
CFLAGS = -g -DCURSES_DRIVER -DANSI_DRIVER
TARGETS = ../bin/war ../bin/warupd ../bin/warnews ../bin/warmap ../bin/wartool
CURSES = -lcurses
WARPATH = -DWARPATH='"$(HOME)/war"'
WARFLAGS = -DUSEGETUID

all: $(TARGETS)

WAROBJS = war.o display.o data.o reader.o execpriv.o \
    mailer.o execsupp.o dispsupp.o \
    stdver.o genio.o ansidrv.o cursdrv.o writemap.o
# OMITTED: wtermdrv.o

UPDOBJS = warupd.o data.o execpriv.o execsupp.o execuser.o \
    combat.o deserter.o creator.o \
    stdver.o savegame.o

NEWSOBJS = warnews.o reader.o \
    stdver.o execsupp.o genio.o ansidrv.o cursdrv.o
# OMITTED: wtermdrv.o

MAPOBJS = warmap.o data.o execpriv.o execsupp.o \
    stdver.o savegame.o genio.o ansidrv.o cursdrv.o
# OMITTED: wtermdrv.o

TOOLOBJS = wartool.o data.o execpriv.o execsupp.o \
    stdver.o display.o dispsupp.o savegame.o \
    genio.o ansidrv.o cursdrv.o
# OMITTED: wtermdrv.o

../bin/war: $(WAROBJS)
	gcc -o ../bin/war $(WAROBJS) $(CURSES)

../bin/warupd: $(UPDOBJS)
	gcc -o ../bin/warupd $(UPDOBJS)

../bin/warnews: $(NEWSOBJS)
	gcc -o ../bin/warnews $(NEWSOBJS) $(CURSES)

../bin/warmap: $(MAPOBJS)
	gcc -o ../bin/warmap $(MAPOBJS) $(CURSES)

../bin/wartool: $(TOOLOBJS)
	gcc -o ../bin/wartool $(TOOLOBJS) $(CURSES)

clean:
	rm -f $(TARGETS) *.o

install: all
	cp $(TARGETS) /home/$(USER)/bin

ansidrv.o: ansidrv.c genio.h
	$(CC) $(CFLAGS) -c ansidrv.c

combat.o: combat.c war.h warext.h function.h reader.h data.h combat.h
	$(CC) $(CFLAGS) -c combat.c

creator.o: creator.c war.h warext.h function.h
	$(CC) $(CFLAGS) -c creator.c

cursdrv.o: cursdrv.c genio.h
	$(CC) $(CFLAGS) -c cursdrv.c

data.o: data.c war.h function.h data.h
	$(CC) $(CFLAGS) -c data.c

deserter.o: deserter.c war.h warext.h function.h data.h
	$(CC) $(CFLAGS) -c deserter.c

display.o: display.c war.h warext.h genio.h function.h data.h display.h dispsupp.h
	$(CC) $(CFLAGS) -c display.c

dispsupp.o: dispsupp.c war.h warext.h genio.h function.h dispsupp.h
	$(CC) $(CFLAGS) -c dispsupp.c

execpriv.o: execpriv.c war.h warext.h function.h execsupp.h
	$(CC) $(CFLAGS) -c execpriv.c

execsupp.o: execsupp.c
	$(CC) $(CFLAGS) -c execsupp.c

execuser.o: execuser.c war.h warext.h function.h execsupp.h
	$(CC) $(CFLAGS) -c execuser.c

genio.o: genio.c genio.h
	$(CC) $(CFLAGS) -c genio.c

mailer.o: mailer.c war.h genio.h function.h reader.h dispsupp.h
	$(CC) $(CFLAGS) -c mailer.c

reader.o: reader.c genio.h reader.h dispsupp.h
	$(CC) $(CFLAGS) -c reader.c

savegame.o: savegame.c war.h warext.h
	$(CC) $(CFLAGS) -c savegame.c

stdver.o: stdver.c
	$(CC) $(CFLAGS) -c stdver.c

war.o: war.c war.h warext.h genio.h function.h
	$(CC) $(CFLAGS) -c war.c $(WARPATH) $(WARFLAGS)

warmap.o: warmap.c war.h warext.h function.h
	$(CC) $(CFLAGS) -c warmap.c

warnews.o: warnews.c war.h dispsupp.h
	$(CC) $(CFLAGS) -c warnews.c $(WARPATH)

wartool.o: wartool.c war.h warext.h display.h dispsupp.h
	$(CC) $(CFLAGS) -c wartool.c

warupd.o: warupd.c war.h warext.h function.h combat.h
	$(CC) $(CFLAGS) -c warupd.c

writemap.o: writemap.c war.h warext.h function.h
	$(CC) $(CFLAGS) -c writemap.c

wtermdrv.o: wtermdrv.c genio.h
	$(CC) $(CFLAGS) -c wtermdrv.c

