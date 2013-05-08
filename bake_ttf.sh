#!/bin/sh
family=0xA000
variant=$1

input=$variant.asc
rm -rf $family-$variant.ufo
./0xA000 $input
./fontconvert $family-$variant.ufo -t
