BIN_PKGMODULES=glib-2.0

CFLAGS += -O2 -g
all: 0xA000-Mono.ttf      \
	   0xA000-Pixelated.ttf \
		 0xA000-Sans.ttf      \
		 0xA000-MonoSans.ttf  \
		 0xA000-MonoSansA.ttf \
		 \
		 Contribute.html      \
		 Glyphs.html          \
		 Tech.html            \
		 index.html
	
0xA000-%.ttf: %.asc 0xA000 *.asc
	./bake_ttf.sh `echo $< | sed s/\.asc//`

%.html: %.content head.html
	cat head.html neck.html $< end.html > $@

# this also relies on all ufo dirs existing.
Glyphs.content: Glyphs.content.sh
	./$< > $@

clean: clean-too
clean-too:
	rm -rf *.ttf *.ufo

include .mm/magic
include .mm/bin
