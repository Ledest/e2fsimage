#
# vi: set sw=4 ts=4: 
#
# $Id: Makefile,v 1.1 2004/01/13 23:02:53 chris2511 Exp $
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


.PHONY: $(SUBDIRS)

