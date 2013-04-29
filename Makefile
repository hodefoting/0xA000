BIN_PKGMODULES=glib-2.0
BIN_LD_FLAGS = -lm

CFLAGS += -O2 -g

0xA000.ttf: 0xA000.ufo/glyphs/contents.plist
	../tinytypetools/fontconvert/fontconvert 0xA000.ufo -t
0xA000.ufo/glyphs/contents.plist: 0xA000
	./0xA000 regular.png 0xA000

include .mm/magic
include .mm/bin
