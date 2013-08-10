# vi: set sw=4 ts=4:
#
# $Id: Makefile,v 1.7 2004/02/18 00:09:19 chris2511 Exp $
#
######################################################################

sinclude ../Local.mak

MANPAGES= e2fsimage.1

all: $(MANPAGES)

%.1:%.man
	sed "s/_VERSION_/$(VERSION)/;s/_DATE_/$(date '+%B %G')/" $< > $@

clean:
	rm -f $(MANPAGES)

install:
	install -d -m 0755 $(DESTDIR)$(mandir)/man1
	install -p -m 0644 $(MANPAGES) $(DESTDIR)$(mandir)/man1/
