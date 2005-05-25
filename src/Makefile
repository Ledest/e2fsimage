# vi: set sw=4 ts=4:
#
# $Id: Makefile,v 1.16 2005/05/25 18:06:51 chris2511 Exp $
#
######################################################################

sinclude ../Local.mak

all: e2fsimage

OBJS=main.o copy.o symlink.o util.o mkdir.o \
	dirent.o mke2fs.o inodb.o sfile.o uiddb.o uids.o passwd.o malloc.o

unused_OBJ=initfs.o

e2fsimage: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS) 

%.o: %.c Makefile ../Local.mak e2fsimage.h
	$(CC) $(CPPFLAGS) -DVER=\"$(VERSION)\" $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) e2fsimage

install:
	mkdir -p $(basedir)$(prefix)/bin
	cp  e2fsimage $(basedir)$(prefix)/bin
	$(STRIP) $(basedir)$(prefix)/bin/e2fsimage
