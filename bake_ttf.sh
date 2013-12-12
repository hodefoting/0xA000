#!/bin/sh

rm -rf $1.ufo
fontile $1.asc

# do kerning if the font file contains kernagic instructions
grep kernagic-options $1.asc && (
 KOPTIONS=`grep kernagic-options $1.asc | sed s/.*kernagic-options://`
 rm -rf input.ufo
 cp -rv $1.ufo input.ufo
 kernagic $KOPTIONS input.ufo -o $1.ufo
 rm -rf input.ufo
)

./fontconvert $1.ufo -t > /dev/null 2>&1

# build per variant specimen page for TTF

echo "<style>@font-face {font-family:'custom'; src: url('$1.ttf');}</style>" > $1.content

echo "<h2 style='font-family:"custom";' contenteditable='true'>
  fix problem quickly with galvanized jets
  THE FIVE BOXING WIZARDS JUMP QUICKLY
  ꜰᴇᴡ ʙʟᴀᴄᴋ ᴛᴀᴚɪꜱ ᴅʀɪᴠᴇ ᴜᴘ ᴍᴀᴊᴏʀ ʀᴏᴀᴅꜱ ᴏɴ ʜᴀᴢʏ Ǫᴜɪᴇᴛ ɴɪɢʜᴛꜱ
</h2>" >> $1.content

echo "<h2 style='font-family:\"custom\";'><a href='$1.ttf' style='font-family:\"custom\"'>$1.ttf</a> (`ls -sh $1.ttf | cut -f 1 -d ' '`)</h2>" >> $1.content 

echo "<div class='glyphs'>" >> $1.content

for a in `ls $1.ufo/glyphs -1 | sort | uniq | sed s/\.glif//`; do
  line=`grep "^$a"";" UnicodeData.txt`
  description=`echo $line|cut -f 2 -d ';'`
  if [ -n "$description" ]; then
    echo "
    <div style='float: left; display: box-inline; width: 4em; height: 6em; border: 0px solid red;'>
      <center><span style='font-size: 4em; border: 1px solid gray; font-family:custom'>&#x$a;</span></center>
    </div>" >> $1.content
  fi
done

echo "</div><div class='clear'></div>" >> $1.content

make $1.html

