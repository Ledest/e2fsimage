#
# vi: set sw=4 ts=4: 
#
# $Id: Makefile,v 1.2 2004/01/15 00:24:36 chris2511 Exp $
#
######################################################################


SUBDIRS=src man

all clean: $(SUBDIRS)
	for x in $(SUBDIRS); do \
		make -C $$x $@; \
	done


$(SUBDIRS): Local.mak

Local.mak: configure
	./$<

distclean: clean
	rm Local.mak

.PHONY: $(SUBDIRS)

