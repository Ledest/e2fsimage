#
# vi: set sw=4 ts=4: 
#
# $Id: Makefile,v 1.5 2004/02/03 23:22:30 chris2511 Exp $
#
######################################################################


SUBDIRS=src man

all clean install: $(SUBDIRS)
	for x in $(SUBDIRS); do \
		$(MAKE) -C $$x $@; \
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

