# vi: set sw=4 ts=4:
#
# $Id: Makefile,v 1.1 2004/01/13 23:02:53 chris2511 Exp $
#
######################################################################

sinclude ../Local.mak

all: e2fsimage

install:

OBJS=initfs.o main.o copy.o

e2fsimage: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@ $^

clean:
	rm -f $(OBJS) e2fsimage
