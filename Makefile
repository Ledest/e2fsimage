#
# vi: set sw=4 ts=4: 
#
# $Id: Makefile,v 1.4 2004/01/28 13:18:27 chris2511 Exp $
#
######################################################################


SUBDIRS=src man

all clean install: $(SUBDIRS)
	for x in $(SUBDIRS); do \
		make -C $$x $@; \
	done


$(SUBDIRS): Local.mak

Local.mak: configure
	./$<

distclean: clean
	rm Local.mak

TAG=$(shell echo "VERSION.$(TVERSION)" |sed "s/\./_/g" )
TARGET=e2fsimage-$(TVERSION)

dist: 
	test ! -z "$(TVERSION)"
	rm -rf $(TARGET) 
	cvs export -r $(TAG) -d $(TARGET) e2fsimage && \
	tar zcf $(TARGET).tar.gz $(TARGET) && \
	cd $(TARGET) && dpkg-buildpackage -uc -b -rfakeroot

.PHONY: $(SUBDIRS)

