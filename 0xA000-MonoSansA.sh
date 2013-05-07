#!/bin/sh

PNG=MonoSans.asc
family=0xA000
variant=MonoSansA
baselineoffset=0

rm -rf $family-$variant.ufo
./0xA000 $PNG $family-$variant $variant $baselineoffset 1
./fontconvert $family-$variant.ufo -t
#rm -rf $family-$variant.ufo
