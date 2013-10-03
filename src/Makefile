
CC = gcc
CFLAGS = -g -DCURSES_DRIVER -DANSI_DRIVER -DWARTERM_DRIVER -DAUTHPGM='"/home/$(USER)/bin/authpgm \"%s\" \"%s\""'
TARGETS = war warupd warnews warmap wartool

all: $(TARGETS)

WAROBJS = war.o display.o data.o reader.o execpriv.o \
    mailer.o execsupp.o dispsupp.o global.o \
    stdver.o genio.o ansidrv.o wtermdrv.o cursdrv.o writemap.o

UPDOBJS = warupd.o data.o execpriv.o execsupp.o execuser.o \
    combat.o deserter.o creator.o \
    global.o stdver.o savegame.o

NEWSOBJS = warnews.o reader.o global.o \
    stdver.o execsupp.o genio.o ansidrv.o wtermdrv.o cursdrv.o

MAPOBJS = warmap.o data.o execpriv.o execsupp.o \
    global.o stdver.o savegame.o genio.o ansidrv.o wtermdrv.o cursdrv.o

TOOLOBJS = wartool.o data.o execpriv.o execsupp.o \
    global.o stdver.o display.o dispsupp.o savegame.o \
    genio.o ansidrv.o wtermdrv.o cursdrv.o

war: $(WAROBJS)
	gcc -o war $(WAROBJS) -lncurses

warupd: $(UPDOBJS)

warnews: $(NEWSOBJS)
	gcc -o warnews $(NEWSOBJS) -lncurses

warmap: $(MAPOBJS)
	gcc -o warmap $(MAPOBJS) -lncurses

wartool: $(TOOLOBJS)
	gcc -o wartool $(TOOLOBJS) -lncurses

clean:
	rm -f $(TARGETS) *.o

install: all
	cp $(TARGETS) /home/$(USER)/bin

