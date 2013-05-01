BIN_PKGMODULES=glib-2.0
BIN_LD_FLAGS = -lm

CFLAGS += -O2 -g
all: 0xA000.ttf 0xA000-Mono.ttf
%.ttf: %.sh 0xA000
	./$<

clean: clean-too
clean-too:
	rm -rf *.ttf *.ufo

include .mm/magic
include .mm/bin
