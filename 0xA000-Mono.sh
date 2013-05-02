#!/bin/sh

PNG=mono.asc
family=0xA000
variant=Mono
baselineoffset=0
glyphs=" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\`abcdefghijklmnopqrstuvwxyz{|}~"

rm -rf $family-$variant.ufo
./0xA000 $PNG $family-$variant $variant $baselineoffset "$glyphs"
./fontconvert $family-$variant.ufo -t
rm -rf $family-$variant.ufo
