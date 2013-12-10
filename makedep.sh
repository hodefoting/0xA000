#!/bin/sh

for a in `cat fonts.list`; do
  echo -n $a.ttf": $a.asc "
  for b in `cat $a.asc | grep include | grep -v '^#' | cut -f 2 -d ' '`; do
    echo -n " $b"
  done
  echo ''
done
