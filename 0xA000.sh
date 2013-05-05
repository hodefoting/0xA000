#!/bin/sh

PNG=regular.asc
family=0xA000
variant=Medium
baselineoffset=0

rm -rf $family.ufo
./0xA000 $PNG $family $variant $baselineoffset
./fontconvert $family.ufo -t
#rm -rf $family.ufo
