# vi: set sw=4 ts=4:
#
# $Id: Makefile,v 1.17 2006/01/11 21:57:27 chris2511 Exp $
#
######################################################################

sinclude ../Local.mak

all: e2fsimage

OBJS=main.o copy.o symlink.o util.o mkdir.o \
    dirent.o mke2fs.o inodb.o sfile.o uiddb.o uids.o \
	malloc.o passwd.o group.o

unused_OBJ=initfs.o

e2fsimage: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c Makefile ../Local.mak e2fsimage.h
	$(CC) $(CPPFLAGS) -DVER=\"$(VERSION)\" $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) e2fsimage

install:
	install -pD -m 0755 e2fsimage $(DESTDIR)$(prefix)/bin/e2fsimage
