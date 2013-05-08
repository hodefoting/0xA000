#!/bin/sh

PNG=mono.asc
family=0xA000
variant=Mono

rm -rf $family-$variant.ufo
./0xA000 $PNG $family-$variant $variant
./fontconvert $family-$variant.ufo -t
