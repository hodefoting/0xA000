all: 0xA000-Regular.ttf         \
		 0xA000-Mono.ttf            \
		 0xA000-Bold.ttf            \
	   0xA000-Pixelated.ttf       \
	   0xA000-Pixelated-Mono.ttf  \
	   0xA000-Boxes.ttf           \
		 0xA000-Boxes-Mono.ttf      \
	   0xA000-Dots.ttf            \
		 0xA000-Dots-Mono.ttf       \
	   0xA000-Monochrome.ttf      \
	   0xA000-Monochrome-Mono.ttf \
		 0xA000-Edit-Mono.ttf       \
		 Beta-Regular.ttf           \
		 Beta-Edit.ttf           	  \
		 \
		 Glyphs.html                \
		 Tech.html                  \
		 index.html

CFLAGS += -O2 -g

0xA000: 0xA000.c
	gcc $< -o $@ `pkg-config --cflags --libs glib-2.0`
0xA000-cgen: 0xA000-cgen.c
	gcc $< -o $@ `pkg-config --cflags --libs gegl`

Beta-Regular.ttf: squiggly.pal
Beta-Edit.ttf: squiggly.pal

%.pal: %.png 0xA000-cgen
	./0xA000-cgen $< > $@
%.ttf: %.asc 0xA000 *.asc
	./bake_ttf.sh `echo $< | sed s/\.asc//`

%.html: %.content head.html
	cat head.html neck.html $< end.html > $@

# this also relies on all ufo dirs existing.
# it has to be manually invoked
Glyphs.content: Glyphs.content.sh UnicodeData.txt
	./$< > $@

# not including such a huge file in the repo..
UnicodeData.txt:
	wget ftp://ftp.unicode.org/Public/UNIDATA/UnicodeData.txt

clean: 
	rm -rf *.ttf *.ufo
	rm -rf *.pal
	rm -rf 0xA000 0xA000-cgen
