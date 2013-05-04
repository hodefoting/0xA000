BIN_PKGMODULES=glib-2.0
BIN_LD_FLAGS = -lm

CFLAGS += -O2 -g
all: 0xA000.ttf 0xA000-Mono.ttf 0xA000-Pixelated.ttf 0xA000-Sans.ttf 0xA000-MonoSans.ttf
%.ttf: %.sh 0xA000 *.asc
	./$<

clean: clean-too
clean-too:
	rm -rf *.ttf *.ufo

include .mm/magic
include .mm/bin
