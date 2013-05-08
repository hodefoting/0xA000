#!/bin/sh

PNG=MonoSans.asc
family=0xA000
variant=MonoSans

rm -rf $family-$variant.ufo
./0xA000 $PNG $family-$variant $variant
./fontconvert $family-$variant.ufo -t
