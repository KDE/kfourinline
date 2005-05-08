GRAFIXDIRS = default 

grafixdir = $(kde_datadir)/kwin4/grafix

uninstall-local:
	rm -rf $(DESTDIR)$(grafixdir)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(grafixdir)
	for i in $(GRAFIXDIRS); do \
       if test -d $(DESTDIR)$(grafixdir)/$$i; then \
                echo "refreshing and removing $$i" ;\
                rm -rf $(DESTDIR)$(grafixdir)/$$i;\
        fi ;\
        echo "installing $$i" ;\
	if test ! -d $(DESTDIR)$(grafixdir)/$$i; then \
		cp -r $(srcdir)/$$i $(DESTDIR)$(grafixdir)/$$i ;\
		rm -rf $(DESTDIR)$(grafixdir)/$$i/CVS ;\
                rm -rf $(DESTDIR)$(grafixdir)/$$i/.svn ;\
	fi ;\
	done      

