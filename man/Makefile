# vi: set sw=4 ts=4:
#
# $Id: Makefile,v 1.5 2004/01/27 16:48:25 chris2511 Exp $
#
######################################################################

sinclude ../Local.mak

MANPAGES= e2fsimage.1.gz

all: $(MANPAGES)

%.1.gz:%.man
	cat $< | sed "s/_VERSION_/$(VERSION)/" | \
		sed "s/_DATE_/`date '+%d %B %G'`/" | gzip >$@

clean:
	rm -f $(MANPAGES)
	
install:
	install -d $(basedir)$(prefix)/$(mandir)/man1
	install $(MANPAGES) $(basedir)$(prefix)/$(mandir)/man1
	
