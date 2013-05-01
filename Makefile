BIN_PKGMODULES=glib-2.0
BIN_LD_FLAGS = -lm

CFLAGS += -O2 -g
all: 0xA000.ttf 0xA000-mono.ttf
0xA000.ttf: 0xA000.ufo/glyphs/contents.plist
	../tinytypetools/fontconvert/fontconvert 0xA000.ufo -t
0xA000.ufo/glyphs/contents.plist: 0xA000
	./0xA000 regular.png 0xA000 Medium


0xA000-mono.ttf: 0xA000-mono.ufo/glyphs/contents.plist
	../tinytypetools/fontconvert/fontconvert 0xA000-mono.ufo -t
0xA000-mono.ufo/glyphs/contents.plist: 0xA000
	./0xA000 mono.png 0xA000-mono Mono

clean: clean-too
clean-too:
	rm -rf *.ttf *.ufo

include .mm/magic
include .mm/bin
