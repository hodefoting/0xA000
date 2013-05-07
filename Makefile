BIN_PKGMODULES=glib-2.0
BIN_LD_FLAGS = -lm

CFLAGS += -O2 -g
all: 0xA000-Mono.ttf \
	   0xA000-Pixelated.ttf \
		 0xA000-Sans.ttf \
		 0xA000-MonoSans.ttf \
		 0xA000-MonoSansA.ttf \
		 Contribute.html \
		 Glyphs.html \
		 index.html
	
%.ttf: %.sh 0xA000 *.asc
	./$<

%.html: %.content head.html
	cat head.html neck.html $< end.html > $@

Glyphs.content: Glyphs.content.sh
	./$< > $@

clean: clean-too
clean-too:
	rm -rf *.ttf *.ufo

include .mm/magic
include .mm/bin
