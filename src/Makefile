# vi: set sw=4 ts=4:
#
# $Id: Makefile,v 1.4 2004/01/15 00:24:36 chris2511 Exp $
#
######################################################################

sinclude ../Local.mak

all: e2fsimage

install:

OBJS=initfs.o main.o copy.o symlink.o util.o mkdir.o

e2fsimage: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) e2fsimage
