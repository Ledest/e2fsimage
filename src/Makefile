# vi: set sw=4 ts=4:
#
# $Id: Makefile,v 1.3 2004/01/14 16:01:32 chris2511 Exp $
#
######################################################################

sinclude ../Local.mak

all: e2fsimage

install:

OBJS=initfs.o main.o copy.o symlink.o

e2fsimage: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) e2fsimage
