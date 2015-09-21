
FONTS=$(shell cat fonts.list | sed s/$$/.ttf/)

all: .dep \
	   $(FONTS) \
		 0xA000.zip										 \
		 \
		 Glyphs.html                   \
		 Tech.html                     \
		 index.html 0xA000.css

0xA000.spacing: 0xA000spacing.ttf
	cp 0xA000.spacing~ 0xA000.spacing

0xA000.css: fonts.list
	echo "" > $@
	for a in `cat fonts.list`; do echo "@font-face {font-family:'"$$a"';src:url('"$$a".ttf');}" >> $@; done

components-ultralight.asc: wgen  Makefile
	./wgen 1.0 > components-ultralight.asc  # 100
components-thin.asc: wgen  Makefile
	./wgen 0.8  > components-thin.asc        # 200
components-light.asc: wgen Makefile
	./wgen 0.6 > components-light.asc       # 300
components-regular.asc: wgen Makefile
	./wgen 0.4 > components-regular.asc     # 400
components-medium.asc: wgen Makefile
	./wgen 0.3  > components-medium.asc      # 500
components-semibold.asc: wgen Makefile
	./wgen 0.2 > components-semibold.asc    # 600
components-bold.asc: wgen  Makefile
	./wgen 0.1 > components-bold.asc        # 700
components-extra-bold.asc: wgen  Makefile
	./wgen 0.0 > components-extra-bold.asc # 800
components-heavy.asc: wgen  Makefile
	./wgen -0.1 > components-heavy.asc      # 800
components-ultra-black.asc: wgen  Makefile
	./wgen -0.2 > components-ultra-black.asc # 900

CFLAGS += -O2 -g

0xA000.zip: 0xA000-Bold.ttf 0xA000-Mono.ttf 0xA000-Mono-Bold.ttf \
	          0xA000-Monochrome.ttf  \
						0xA000-Dots.ttf 0xA000-Dots-Mono.ttf \
						0xA000-Boxes.ttf
	zip $@ *.ttf LICENSE.OFL

%.ttf: %.asc bake_ttf.sh feature.fea
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
