#!/bin/sh

rm -rf $1.ufo
fontile $1.asc
./fontconvert $1.ufo -t

echo "<style>@font-face {font-family:'custom'; src: url('$1.ttf');}</style>" > $1.content
echo "<h1><a href='$1.ttf' style='font-family:\"custom\"'>$1.ttf</a></h1><h2
style='font-family:"custom";' contenteditable='true'>
  fix problem quickly with galvanized jets
  ꜰᴇᴡ ʙʟᴀᴄᴋ ᴛᴀᴚɪꜱ ᴅʀɪᴠᴇ ᴜᴘ ᴍᴀᴊᴏʀ ʀᴏᴀᴅꜱ ᴏɴ ʜᴀᴢʏ Ǫᴜɪᴇᴛ ɴɪɢʜᴛꜱ
  THE FIVE BOXING WIZARDS JUMP QUICKLY
</h2>" >> $1.content
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

