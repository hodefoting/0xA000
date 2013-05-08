#!/bin/sh

PNG=Sans.asc
family=0xA000
variant=Sans

rm -rf $family-$variant.ufo
./0xA000 $PNG $family-$variant $variant
./fontconvert $family-$variant.ufo -t
