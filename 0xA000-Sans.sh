#!/bin/sh

PNG=Sans.asc
family=0xA000
variant=Sans
baselineoffset=0
# XXX: dont need to pass glyphs anymore - this is a relic
glyphs=" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_\`abcdefghijklmnopqrstuvwxyz{|}~ æøåÆØÅ€¡Ññ£čšžČŠŽ©";
rm -rf $family-$variant.ufo
./0xA000 $PNG $family-$variant $variant $baselineoffset "$glyphs"
./fontconvert $family-$variant.ufo -t
rm -rf $family-$variant.ufo
