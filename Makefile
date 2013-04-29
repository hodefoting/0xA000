BIN_PKGMODULES=glib-2.0
BIN_LD_FLAGS = -lm

CFLAGS += -O2 -g

RetroAA.ttf: RetroAA.ufo/glyphs/contents.plist
	../tinytypetools/fontconvert/fontconvert RetroAA.ufo -t

RetroAA.ufo/glyphs/contents.plist: retro-aa.ufo
	./retro-aa.ufo ascii3.png RetroAA


include .mm/magic
include .mm/bin
