#!/bin/sh

if [ "x$1" = "x" ]; then
   echo "No source"
   exit 0
fi
if [ "x$2" = "x" ]; then
   echo "No target"
   exit 0
fi

echo "linking $2 -> $1"
ln -s "$2" "$1"
