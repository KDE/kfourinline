
bin_PROGRAMS = kwin4 kwin4proc
kwin4_SOURCES = main.cpp kwin4.cpp kwin4view.cpp kwin4doc.cpp \
                kwin4player.cpp kspritecache.cpp \
	            	scorewidget.cpp prefs.kcfgc settings.ui statistics.ui statuswidget.ui

kwin4_LDADD   = $(LIB_KFILE) $(LIB_KDEGAMES)
kwin4_DEPENDENCIES = $(LIB_KDEGAMES_DEP)

kwin4proc_SOURCES = kwin4proc.cpp
kwin4proc_LDADD   = $(LIB_KFILE) $(LIB_KDEGAMES)
kwin4proc_DEPENDENCIES = $(LIB_KDEGAMES_DEP)

INCLUDES = -I$(top_srcdir)/libkdegames -I$(top_srcdir)/libkdegames/kgame $(all_includes)

METASOURCES = AUTO

rcdir = $(kde_datadir)/kwin4
rc_DATA = kwin4ui.rc

kwin4_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kwin4proc_LDFLAGS = $(all_libraries) $(KDE_RPATH)

kde_kcfg_DATA = kwin4.kcfg

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kwin4.pot 

