#!/bin/sh

PNG=Sans.asc
family=0xA000
variant=Sans
baselineoffset=0

rm -rf $family-$variant.ufo
./0xA000 $PNG $family-$variant $variant $baselineoffset
./fontconvert $family-$variant.ufo -t
rm -rf $family-$variant.ufo
