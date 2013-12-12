
all: .dep \
	   0xA000.ttf    				         \
		 0xA000-Contrast.ttf           \
		 0xA000-Contrast-Mono.ttf      \
		 0xA000-Ultralight.ttf         \
		 0xA000-Thin.ttf               \
		 0xA000-Light.ttf              \
		 0xA000-Semibold.ttf           \
		 0xA000-Extra-Bold.ttf         \
		 0xA000-Ultra-Black.ttf        \
		 0xA000-Bold.ttf               \
		 0xA000-Heavy.ttf              \
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
		 index.html test.html 


components-ultralight.asc: wgen 
	./wgen 1.0 > components-ultralight.asc  # 100
components-thin.asc: wgen 
	./wgen 0.8 > components-thin.asc        # 200
components-light.asc: wgen
	./wgen 0.6 > components-light.asc       # 300
components-regular.asc: wgen
	./wgen 0.4 > components-regular.asc     # 400
components-medium.asc: wgen
	./wgen 0.3 > components-medium.asc      # 500
components-semibold.asc: wgen
	./wgen 0.15 > components-semibold.asc    # 600
components-bold.asc: wgen 
	./wgen 0.0 > components-bold.asc        # 700
components-extra-bold.asc: wgen 
	./wgen -0.125 > components-extra-bold.asc # 800
components-heavy.asc: wgen 
	./wgen -0.3  > components-heavy.asc      # 800
components-ultra-black.asc: wgen 
	./wgen -0.5 > components-ultra-black.asc # 900

CFLAGS += -O2 -g

0xA000.zip: 0xA000.ttf 0xA000-Bold.ttf 0xA000-Mono.ttf 0xA000-Mono-Bold.ttf \
	          0xA000-Monochrome.ttf 0xA000-Monochrome-Mono.ttf \
						0xA000-Dots.ttf 0xA000-Dots-Mono.ttf \
						0xA000-Boxes.ttf 0xA000-Boxes-Mono.ttf
	zip $@ *.ttf LICENSE.OFL

%.ttf: %.asc bake_ttf.sh
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
	rm -rf *.pal clean

install: 
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

# dependency tracking
include .dep
.dep: *.asc makedep.sh fonts.list
	./makedep.sh > .dep
