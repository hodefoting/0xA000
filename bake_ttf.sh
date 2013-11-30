#!/bin/sh

rm -rf $1.ufo
fontile $1.asc
./fontconvert $1.ufo -t

echo "<style>@font-face {font-family:'custom'; src: url('$1.ttf');}</style>" > $1.content
echo "<h1><a href='index.html'>$1</a></h1><h2
style='font-family:"custom"'>The quick brown fox jumps over the lazy dog.</h2>" >> $1.content
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

