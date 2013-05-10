BIN_PKGMODULES=glib-2.0

CFLAGS += -O2 -g
all: 0xA000-Regular.ttf    \
		 0xA000-Mono.ttf       \
	   0xA000-Pixelated-Regular.ttf \
	   0xA000-Pixelated-Mono.ttf \
	   0xA000-Dots-Regular.ttf \
		 0xA000-Dots-Mono.ttf \
		 0xA000-Edit-Mono.ttf \
		 \
		 Contribute.html      \
		 Glyphs.html          \
		 Tech.html            \
		 index.html
	
%.ttf: %.asc 0xA000 *.asc
	./bake_ttf.sh `echo $< | sed s/\.asc//`

%.html: %.content head.html
	cat head.html neck.html $< end.html > $@

# this also relies on all ufo dirs existing.
Glyphs.content: Glyphs.content.sh UnicodeData.txt
	./$< > $@

clean: clean-too
clean-too:
	rm -rf *.ttf *.ufo

UnicodeData.txt:
	wget ftp://ftp.unicode.org/Public/UNIDATA/UnicodeData.txt

include .mm/magic
include .mm/bin
