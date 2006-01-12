#
# vi: set sw=4 ts=4: 
#
# $Id: Makefile,v 1.14 2006/01/12 20:22:32 chris2511 Exp $
#
######################################################################


SUBDIRS=src man

all: Local.mak
install: all
all install clean: 
	for x in $(SUBDIRS); do $(MAKE) CFLAGS="$(CFLAGS)" -C $$x $@ ||exit 1; done

Local.mak: configure
	./$<

distclean: clean
	rm -f Local.mak

TAG=$(shell echo "VERSION.$(TVERSION)" |sed "s/\./_/g" )
STAG=$(shell echo "$(TVERSION)" |sed "s/\-.*//")
TARGET=e2fsimage-$(STAG)

dist: 
	test ! -z "$(TVERSION)"
	rm -rf $(TARGET) 
	cvs export -r $(TAG) -d $(TARGET) e2fsimage && \
	tar -zc --exclude debian -f $(TARGET).tar.gz $(TARGET) && \
	ln $(TARGET).tar.gz e2fsimage_$(STAG).orig.tar.gz
	cd $(TARGET) && dpkg-buildpackage -rfakeroot

.PHONY: $(SUBDIRS)

