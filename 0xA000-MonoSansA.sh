#!/bin/sh

PNG=MonoSansA.asc
family=0xA000
variant=MonoSansA

rm -rf $family-$variant.ufo
./0xA000 $PNG $family-$variant $variant
./fontconvert $family-$variant.ufo -t
