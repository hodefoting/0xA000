#!/bin/sh

PNG=Pixelated.asc
family=0xA000
variant=Pixelated

rm -rf $family-$variant.ufo
./0xA000 $PNG $family-$variant $variant
./fontconvert $family-$variant.ufo -t
