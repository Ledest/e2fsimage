# vi: set sw=4 ts=4:
#
# $Id: Makefile,v 1.2 2004/01/26 16:02:57 chris2511 Exp $
#
######################################################################

sinclude ../Local.mak

MANPAGES= e2fsimage.1

all: $(MANPAGES)

%.1:%.man
	cat $< | sed "s/_VERSION_/$(VERSION)/" >$@

clean:
	rm -f $(MANPAGES)
	
install:
	install -d $(basedir)$(prefix)/man/man1
	install $(MANPAGES) $(basedir)$(prefix)/man/man1
	
