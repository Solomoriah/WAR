
CC = gcc
# CFLAGS = -g -DCURSES_DRIVER -DANSI_DRIVER -DWARTERM_DRIVER -DAUTHPGM='"/home/$(USER)/bin/authpgm \"%s\" \"%s\""'
CFLAGS = -g -DCURSES_DRIVER -DANSI_DRIVER
TARGETS = ../bin/war ../bin/warupd ../bin/warnews ../bin/warmap ../bin/wartool

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
	gcc -o ../bin/war $(WAROBJS) -lncurses

../bin/warupd: $(UPDOBJS)
	gcc -o ../bin/warupd $(UPDOBJS)

../bin/warnews: $(NEWSOBJS)
	gcc -o ../bin/warnews $(NEWSOBJS) -lncurses

../bin/warmap: $(MAPOBJS)
	gcc -o ../bin/warmap $(MAPOBJS) -lncurses

../bin/wartool: $(TOOLOBJS)
	gcc -o ../bin/wartool $(TOOLOBJS) -lncurses

clean:
	rm -f $(TARGETS) *.o

install: all
	cp $(TARGETS) /home/$(USER)/bin

ansidrv.o: ansidrv.c
	$(CC) $(CFLAGS) -c ansidrv.c

combat.o: combat.c
	$(CC) $(CFLAGS) -c combat.c

creator.o: creator.c
	$(CC) $(CFLAGS) -c creator.c

cursdrv.o: cursdrv.c
	$(CC) $(CFLAGS) -c cursdrv.c

data.o: data.c
	$(CC) $(CFLAGS) -c data.c

deserter.o: deserter.c
	$(CC) $(CFLAGS) -c deserter.c

display.o: display.c
	$(CC) $(CFLAGS) -c display.c

dispsupp.o: dispsupp.c
	$(CC) $(CFLAGS) -c dispsupp.c

execpriv.o: execpriv.c
	$(CC) $(CFLAGS) -c execpriv.c

execsupp.o: execsupp.c
	$(CC) $(CFLAGS) -c execsupp.c

execuser.o: execuser.c
	$(CC) $(CFLAGS) -c execuser.c

genio.o: genio.c
	$(CC) $(CFLAGS) -c genio.c

mailer.o: mailer.c
	$(CC) $(CFLAGS) -c mailer.c

reader.o: reader.c
	$(CC) $(CFLAGS) -c reader.c

savegame.o: savegame.c
	$(CC) $(CFLAGS) -c savegame.c

stdver.o: stdver.c
	$(CC) $(CFLAGS) -c stdver.c

war.o: war.c
	$(CC) $(CFLAGS) -c war.c

warmap.o: warmap.c
	$(CC) $(CFLAGS) -c warmap.c

warnews.o: warnews.c
	$(CC) $(CFLAGS) -c warnews.c

wartool.o: wartool.c
	$(CC) $(CFLAGS) -c wartool.c

warupd.o: warupd.c
	$(CC) $(CFLAGS) -c warupd.c

writemap.o: writemap.c
	$(CC) $(CFLAGS) -c writemap.c

wtermdrv.o: wtermdrv.c
	$(CC) $(CFLAGS) -c wtermdrv.c

