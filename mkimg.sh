#!/bin/sh

dd if=/dev/zero of=e2file bs=4096 count=4096
/sbin/mke2fs e2file
