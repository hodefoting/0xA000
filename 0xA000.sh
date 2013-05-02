#!/bin/sh

PNG=regular.asc
family=0xA000
variant=Medium
baselineoffset=0
glyphs=" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\`abcdefghijklmnopqrstuvwxyz{|}~"

rm -rf $family.ufo
./0xA000 $PNG $family $variant $baselineoffset "$glyphs"
./fontconvert $family.ufo -t
rm -rf $family.ufo
