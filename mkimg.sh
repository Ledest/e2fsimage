#!/bin/sh

if test "$1" = ""; then
  echo "Usage: ./mkimg.sh <e2file>";
  exit 1
fi

dd if=/dev/zero of=$1 bs=1024 count=16384
/sbin/mke2fs -F $1
