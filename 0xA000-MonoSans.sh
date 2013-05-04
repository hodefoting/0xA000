#!/bin/sh

PNG=MonoSans.asc
family=0xA000
variant=MonoSans
baselineoffset=0
glyphs=" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\`abcdefghijklmnopqrstuvwxyz{|}~ æøåÆØÅ€¡Ññ£čšžČŠŽ©";
rm -rf $family-$variant.ufo
./0xA000 $PNG $family-$variant $variant $baselineoffset "$glyphs"
./fontconvert $family-$variant.ufo -t
rm -rf $family-$variant.ufo
