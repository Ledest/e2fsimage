#!/bin/sh

dd if=/dev/zero of=$1 bs=4096 count=4096
/sbin/mke2fs $1
