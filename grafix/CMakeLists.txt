kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kwin4/grafix FILES  )

kde4_footer()



#original Makefile.am contents follow:

#GRAFIXDIRS = default 
#
#grafixdir = $(kde_datadir)/kwin4/grafix
#
#uninstall-local:
#	rm -rf $(DESTDIR)$(grafixdir)
#
#install-data-local:
#	$(mkinstalldirs) $(DESTDIR)$(grafixdir)
#	for i in $(GRAFIXDIRS); do \
#       if test -d $(DESTDIR)$(grafixdir)/$$i; then \
#                echo "refreshing and removing $$i" ;\
#                rm -rf $(DESTDIR)$(grafixdir)/$$i;\
#        fi ;\
#        echo "installing $$i" ;\
#	if test ! -d $(DESTDIR)$(grafixdir)/$$i; then \
#		cp -r $(srcdir)/$$i $(DESTDIR)$(grafixdir)/$$i ;\
#		rm -rf $(DESTDIR)$(grafixdir)/$$i/CVS ;\
#                rm -rf $(DESTDIR)$(grafixdir)/$$i/.svn ;\
#	fi ;\
#	done      
#
