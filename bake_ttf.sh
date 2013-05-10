#!/bin/sh

rm -rf $1.ufo
./0xA000 $1.asc
./fontconvert $1.ufo -t
