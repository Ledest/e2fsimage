#
# vi: set sw=4 ts=4: 
#
# $Id: Makefile,v 1.8 2004/03/02 08:32:32 chris2511 Exp $
#
######################################################################


SUBDIRS=src man

all install: $(SUBDIRS)
all install clean: 
	for x in $(SUBDIRS); do \
		$(MAKE) -C $$x $@; \
	done


$(SUBDIRS): Local.mak

Local.mak: configure
	./$<

distclean: clean
	rm -f Local.mak

TAG=$(shell echo "VERSION.$(TVERSION)" |sed "s/\./_/g" )
TARGET=e2fsimage-$(shell echo "$(TVERSION)" |sed "s/\-.*//")

dist: 
	test ! -z "$(TVERSION)"
	rm -rf $(TARGET) 
	cvs export -r $(TAG) -d $(TARGET) e2fsimage && \
	tar -zc --exclude debian -f $(TARGET).tar.gz $(TARGET) && \
	cd $(TARGET) && dpkg-buildpackage -rfakeroot

.PHONY: $(SUBDIRS)

