

all: 0xA000.ttf    				         \
		 0xA000-Bold.ttf               \
		 0xA000-Semibold.ttf           \
		 0xA000-Mono.ttf       				 \
		 0xA000-Mono-Bold.ttf          \
		 0xA000-Mono-Semibold.ttf      \
	   0xA000-Pixelated.ttf  				 \
	   0xA000-Pixelated-Mono.ttf     \
	   0xA000-Boxes.ttf      				 \
		 0xA000-Boxes-Mono.ttf         \
	   0xA000-Dots.ttf 				       \
		 0xA000-Dots-Mono.ttf          \
	   0xA000-Monochrome.ttf 				 \
	   0xA000-Monochrome-Mono.ttf    \
		 0xA000-Edit-Mono.ttf          \
		 0xA000-Pen.ttf        				 \
		 0xA000-Pen-Mono.ttf           \
		 0xA000.zip										 \
		 \
		 Glyphs.html                   \
		 Tech.html                     \
		 index.html fit

fit: 0xA000.ttf 0xA000-Bold.ttf Makefile
	rm -rf 0xA000b.ufo
	cp -rv 0xA000.ufo 0xA000b.ufo
	kernagic -g 0.28 -s 1 0xA000b.ufo -o 0xA000.ufo
	./fontconvert 0xA000.ufo -t
	rm -rf 0xA000b.ufo
	cp -rv 0xA000-Bold.ufo 0xA000b.ufo
	kernagic -g 0.28 -s 1 0xA000b.ufo -o 0xA000-Bold.ufo
	./fontconvert 0xA000-Bold.ufo -t



CFLAGS += -O2 -g

0xA000.zip: *.ttf
	zip $@ $< LICENSE.OFL

tilegen: tilegen.c
	gcc $< -o $@ `pkg-config --cflags --libs gegl`

0xA000.ttf:               slimmed.pal
0xA000-Mono.ttf:          slimmed.pal
0xA000-Pen.ttf:           squiggly.pal
0xA000-Pen-Mono.ttf:      squiggly.pal

%.pal: %.png tilegen
	./tilegen $< > $@
%.ttf: %.asc glyphs-*.asc
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
	rm -rf tilegen 

install:
	install -d /usr/share/fonts/truetype/0xA000/
	install *.ttf /usr/share/fonts/truetype/0xA000/
	fc-cache -fv

uninstall:
	rm -rf /usr/share/fonts/truetype/0xA000/
	fc-cache -fv
