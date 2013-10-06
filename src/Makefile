
CC = gcc
# CFLAGS = -g -DCURSES_DRIVER -DANSI_DRIVER -DWARTERM_DRIVER -DAUTHPGM='"/home/$(USER)/bin/authpgm \"%s\" \"%s\""'
CFLAGS = -g -DCURSES_DRIVER -DANSI_DRIVER
TARGETS = ../bin/war ../bin/warupd ../bin/warnews ../bin/warmap ../bin/wartool

all: $(TARGETS)

WAROBJS = war.o display.o data.o reader.o execpriv.o \
    mailer.o execsupp.o dispsupp.o global.o \
    stdver.o genio.o ansidrv.o cursdrv.o writemap.o
# OMITTED: wtermdrv.o

UPDOBJS = warupd.o data.o execpriv.o execsupp.o execuser.o \
    combat.o deserter.o creator.o \
    global.o stdver.o savegame.o

NEWSOBJS = warnews.o reader.o global.o \
    stdver.o execsupp.o genio.o ansidrv.o cursdrv.o
# OMITTED: wtermdrv.o

MAPOBJS = warmap.o data.o execpriv.o execsupp.o \
    global.o stdver.o savegame.o genio.o ansidrv.o cursdrv.o
# OMITTED: wtermdrv.o

TOOLOBJS = wartool.o data.o execpriv.o execsupp.o \
    global.o stdver.o display.o dispsupp.o savegame.o \
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

