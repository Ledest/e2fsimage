# vi: set sw=4 ts=4:
#
# $Id: Makefile,v 1.3 2004/01/27 00:04:57 chris2511 Exp $
#
######################################################################

sinclude ../Local.mak

MANPAGES= e2fsimage.1.gz

all: $(MANPAGES)

%.1.gz:%.man
	cat $< | sed "s/_VERSION_/$(VERSION)/" | gzip >$@

clean:
	rm -f $(MANPAGES)
	
install:
	install -d $(basedir)$(prefix)/man/man1
	install $(MANPAGES) $(basedir)$(prefix)/man/man1
	
