# vi: set sw=4 ts=4:
#
# $Id: Makefile,v 1.5 2004/01/17 22:13:59 chris2511 Exp $
#
######################################################################

sinclude ../Local.mak

all: e2fsimage

install:

OBJS=initfs.o main.o copy.o symlink.o util.o mkdir.o dirent.o

e2fsimage: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) e2fsimage
