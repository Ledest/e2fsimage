#
# vi: set sw=4 ts=4: 
#
# $Id: Makefile,v 1.3 2004/01/26 16:02:57 chris2511 Exp $
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

.PHONY: $(SUBDIRS)

