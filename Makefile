

all: 0xA000.ttf    				         \
		 0xA000-Bold.ttf               \
		 0xA000-Mono.ttf       				 \
		 0xA000-Mono-Bold.ttf          \
	   0xA000-Pixelated.ttf  				 \
	   0xA000-Pixelated-Mono.ttf     \
	   0xA000-Boxes.ttf      				 \
		 0xA000-Boxes-Mono.ttf         \
	   0xA000-Dots.ttf 				       \
		 0xA000-Dots-Mono.ttf          \
	   0xA000-Monochrome.ttf 				 \
	   0xA000-Monochrome-Mono.ttf    \
		 0xA000.zip										 \
		 \
		 Glyphs.html                   \
		 Tech.html                     \
		 index.html fit

#    0xA000-Tiled.ttf    	         \
#    0xA000-Edit-Mono.ttf          \
		 0xA000-Pen.ttf        				 \
		 0xA000-Pen-Mono.ttf           \

foo:
	rm 0xA000.ttf 0xA000.ufo -rf; make fit

GAP = 0.26
SNAP = 0
X_SHIFT = -90
BIG_SCALE = 1.15

fit: 0xA000.ttf 0xA000-Bold.ttf Makefile
	rm -rf 0xA000b.ufo
	cp -rv 0xA000.ufo 0xA000b.ufo
	kernagic -bs $(BIG_SCALE) -g $(GAP) -s $(SNAP) --x_shift $(X_SHIFT) 0xA000b.ufo -o 0xA000.ufo --center-glyphs "ijlI|'.:;"
	./fontconvert 0xA000.ufo -t
	rm -rf 0xA000b.ufo
	cp -rv 0xA000-Bold.ufo 0xA000b.ufo
	kernagic -bs $(BIG_SCALE) -g $(GAP) -s $(SNAP) --x_shift $(X_SHIFT) 0xA000b.ufo -o 0xA000-Bold.ufo --center-glyphs "ijlI|'.:;"
	./fontconvert 0xA000-Bold.ufo -t
	touch fit


CFLAGS += -O2 -g

0xA000.zip: 0xA000.ttf 0xA000-Bold.ttf 0xA000-Mono.ttf 0xA000-Mono-Bold.ttf \
	          0xA000-Monochrome.ttf 0xA000-Monochrome-Mono.ttf \
						0xA000-Dots.ttf 0xA000-Dots-Mono.ttf \
						0xA000-Boxes.ttf 0xA000-Boxes-Mono.ttf
	zip $@ *.ttf LICENSE.OFL

%.ttf: %.asc glyphs-*.asc *.asc bake_ttf.sh
	./bake_ttf.sh `echo $< | sed s/\.asc//`

%.html: %.content head.html
	cat head.html neck.html $< end.html > $@

components-regular.asc: wgen
	./wgen > components-regular.asc

# this also relies on all ufo dirs existing.
# it has to be manually invoked
Glyphs.content: Glyphs.content.sh UnicodeData.txt
	./$< > $@

# not including such a huge file in the repo..
UnicodeData.txt:
	wget ftp://ftp.unicode.org/Public/UNIDATA/UnicodeData.txt
clean: 
	rm -rf *.ttf *.ufo
	rm -rf *.pal fit clean

install: fit
	install -d /usr/share/fonts/truetype/0xA000/
	install *.ttf /usr/share/fonts/truetype/0xA000/
	fc-cache -fv

uninstall:
	rm -rf /usr/share/fonts/truetype/0xA000/
	fc-cache -fv

all: wgen
wgen: wgen.c
	gcc wgen.c -o wgen

head.html: head.html.in
	cat head.html.in fonts.head > head.html

