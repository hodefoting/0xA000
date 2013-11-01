#!/bin/sh

rm -rf $1.ufo
fontile $1.asc
./fontconvert $1.ufo -t
