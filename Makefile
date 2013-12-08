

all: 0xA000.ttf    				         \
		 0xA000-Contrast.ttf           \
		 0xA000-Contrast-Mono.ttf      \
		 0xA000-Thin.ttf               \
		 0xA000-Light.ttf              \
		 0xA000-Semibold.ttf           \
		 0xA000-Bold.ttf               \
		 0xA000-Mono.ttf       				 \
		 0xA000-Mono-Bold.ttf          \
		 0xA000-Mono-Thin.ttf          \
		 0xA000-Mono-Light.ttf         \
		 0xA000-Mono-Semibold.ttf      \
	   0xA000-Pixelated.ttf  				 \
	   0xA000-Pixelated-Mono.ttf     \
	   0xA000-Square.ttf     				 \
	   0xA000-Squareish.ttf  				 \
	   0xA000-Square-Bold.ttf     	 \
	   0xA000-Squareish-Bold.ttf  	 \
	   0xA000-Squareish-Mono.ttf  	\
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
		 index.html test.html fit

#    0xA000-Tiled.ttf    	         \
#    0xA000-Edit-Mono.ttf          \
		 0xA000-Pen.ttf        				 \
		 0xA000-Pen-Mono.ttf           \


components-ultralight.asc: wgen Makefile
	./wgen 0.9 > components-ultralight.asc
components-thin.asc: wgen Makefile
	./wgen 0.8 > components-thin.asc
components-light.asc: wgen Makefile
	./wgen 0.6 > components-light.asc
components-regular.asc: wgen Makefile
	./wgen 0.4 > components-regular.asc
components-semibold.asc: wgen Makefile
	./wgen 0.1 > components-semibold.asc
components-bold.asc: wgen Makefile
	./wgen -0.2 > components-bold.asc

foo:
	rm 0xA000.ttf 0xA000.ufo -rf; make fit

GAP_THIN     = 0.205
GAP_LIGHT    = 0.22
GAP_REGULAR  = 0.23
GAP_SEMIBOLD = 0.24 
GAP_BOLD     = 0.25
SNAP = 0
X_SHIFT = -95
BIG_SCALE = 1.15
OVERRIDES = \
			--override T 0.25 0.75 \
			--override ᴛ 0.25 0.75 \
			--override 1 0.66 0.66 \
			--override 7 0.25 0.97 \
			--override j 0.60 0.60

fit: 0xA000.ttf 0xA000-Bold.ttf 0xA000-Thin.ttf 0xA000-Semibold.ttf 0xA000-Light.ttf Makefile 0xA000-Square.ttf 0xA000-Squareish.ttf 0xA000-Contrast.ttf
	rm -rf 0xA000b.ufo
	cp -rv 0xA000.ufo 0xA000b.ufo
	kernagic -bs $(BIG_SCALE) -g $(GAP_REGULAR) -s $(SNAP) --x_shift $(X_SHIFT) 0xA000b.ufo -o 0xA000.ufo --center-glyphs "ilI|'.:;ıɪ˙" $(OVERRIDES)
	./fontconvert 0xA000.ufo -t > /dev/null 2>&1
	
	rm -rf 0xA000b.ufo
	cp -rv 0xA000-Contrast.ufo 0xA000b.ufo
	kernagic -bs $(BIG_SCALE) -g $(GAP_REGULAR) -s $(SNAP) --x_shift $(X_SHIFT) 0xA000b.ufo -o 0xA000-Contrast.ufo --center-glyphs "ilI|'.:;ıɪ˙" $(OVERRIDES)
	./fontconvert 0xA000-Contrast.ufo -t > /dev/null 2>&1
	
	rm -rf 0xA000b.ufo
	cp -rv 0xA000-Square.ufo 0xA000b.ufo
	kernagic -bs $(BIG_SCALE) -g $(GAP_REGULAR) -s $(SNAP) --x_shift $(X_SHIFT) 0xA000b.ufo -o 0xA000-Square.ufo --center-glyphs "ilI|ıɪ'.:;" $(OVERRIDES)
	./fontconvert 0xA000-Square.ufo -t > /dev/null 2>&1
	
	rm -rf 0xA000b.ufo
	cp -rv 0xA000-Squareish.ufo 0xA000b.ufo
	kernagic -bs $(BIG_SCALE) -g $(GAP_REGULAR) -s $(SNAP) --x_shift $(X_SHIFT) 0xA000b.ufo -o 0xA000-Squareish.ufo --center-glyphs "ilI|ıɪ'.:;" $(OVERRIDES)
	./fontconvert 0xA000-Squareish.ufo -t > /dev/null 2>&1
	
	rm -rf 0xA000b.ufo
	cp -rv 0xA000-Bold.ufo 0xA000b.ufo
	kernagic -bs $(BIG_SCALE) -g $(GAP_BOLD) -s $(SNAP) --x_shift $(X_SHIFT) 0xA000b.ufo -o 0xA000-Bold.ufo --center-glyphs "ilI|ıɪ'.:;" $(OVERRIDES)
	./fontconvert 0xA000-Bold.ufo -t > /dev/null 2>&1
	
	rm -rf 0xA000b.ufo
	cp -rv 0xA000-Square-Bold.ufo 0xA000b.ufo
	kernagic -bs $(BIG_SCALE) -g $(GAP_BOLD) -s $(SNAP) --x_shift $(X_SHIFT) 0xA000b.ufo -o 0xA000-Square-Bold.ufo --center-glyphs "ilI|ıɪ'.:;" $(OVERRIDES)
	./fontconvert 0xA000-Square-Bold.ufo -t > /dev/null 2>&1
	
	rm -rf 0xA000b.ufo
	cp -rv 0xA000-Squareish-Bold.ufo 0xA000b.ufo
	kernagic -bs $(BIG_SCALE) -g $(GAP_BOLD) -s $(SNAP) --x_shift $(X_SHIFT) 0xA000b.ufo -o 0xA000-Squareish-Bold.ufo --center-glyphs "ilI|ıɪ'.:;" $(OVERRIDES)
	./fontconvert 0xA000-Squareish-Bold.ufo -t > /dev/null 2>&1
	
	rm -rf 0xA000b.ufo
	cp -rv 0xA000-Semibold.ufo 0xA000b.ufo
	kernagic -bs $(BIG_SCALE) -g $(GAP_SEMIBOLD) -s $(SNAP) --x_shift $(X_SHIFT) 0xA000b.ufo -o 0xA000-Semibold.ufo --center-glyphs "ilI|ıɪ'.:;" $(OVERRIDES)
	./fontconvert 0xA000-Semibold.ufo -t > /dev/null 2>&1
	
	rm -rf 0xA000b.ufo
	cp -rv 0xA000-Thin.ufo 0xA000b.ufo
	kernagic -bs $(BIG_SCALE) -g $(GAP_THIN) -s $(SNAP) --x_shift $(X_SHIFT) 0xA000b.ufo -o 0xA000-Thin.ufo --center-glyphs "ilI|'.:;ıɪ˙" $(OVERRIDES)
	./fontconvert 0xA000-Thin.ufo -t > /dev/null 2>&1
	
	rm -rf 0xA000b.ufo
	cp -rv 0xA000-Light.ufo 0xA000b.ufo
	kernagic -bs $(BIG_SCALE) -g $(GAP_LIGHT) -s $(SNAP) --x_shift $(X_SHIFT) 0xA000b.ufo -o 0xA000-Light.ufo --center-glyphs "ilI|'.:;ıɪ˙" $(OVERRIDES)
	./fontconvert 0xA000-Light.ufo -t > /dev/null 2>&1
	
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

fonts.head: fonts.list Makefile
	echo "<table><tr><td valign='top'><a href='index.html' style='font-family:\"0xA000-Pixelated\";font-size:3.5em'>0xA000</a><br/><span style='font-family:"0xA000";font-size:11px'>Metamorphic-modular font-family.<ul><li>Sans sharp at small sizes</li><li>Minimalistic geometry</li><li>Extended Latin Support</li><li>Dedicated mono-space design</li></ul></span></td><td valign='top'><div style='font-size:1.5em;margin:0 0 0 0;'>" > fonts.head
	for a in `cat fonts.list`;do \
		echo "<a style='font-family:\"$$a\";' href='$$a.html'>`echo $$a | sed s/0xA000-// | sed s/0xA000/Regular/`</a> " >> fonts.head;\
	done;\
	echo "</div></td></tr></table>" >> fonts.head

head.html: head.html.in fonts.head
	cat head.html.in fonts.head > head.html

