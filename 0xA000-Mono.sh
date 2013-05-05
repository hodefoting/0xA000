#!/bin/sh

PNG=mono.asc
family=0xA000
variant=Mono
baselineoffset=0

rm -rf $family-$variant.ufo
./0xA000 $PNG $family-$variant $variant $baselineoffset 0
./fontconvert $family-$variant.ufo -t
rm -rf $family-$variant.ufo
