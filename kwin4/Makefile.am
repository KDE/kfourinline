
bin_PROGRAMS = kwin4 kwin4proc
kwin4_SOURCES = msgdlg.cpp kwin4aboutdlg.cpp statdlg.cpp \
                namedlg.cpp geom.cpp kwin4view.cpp kwin4doc.cpp kwin4.cpp \
                main.cpp kwin4player.cpp kspritecache.cpp scorewidget.cpp \
                kwlabel.cpp statuswidget.cpp
kwin4_LDADD   = $(LIB_KFILE) $(LIB_KDEGAMES)

kwin4proc_SOURCES = kwin4proc.cpp
kwin4proc_LDADD   = $(LIB_KFILE) $(LIB_KDEGAMES)

INCLUDES = -I$(top_srcdir)/libkdegames -I$(top_srcdir)/libkdegames/kgame $(all_includes)

METASOURCES = AUTO

kwin4_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kwin4proc_LDFLAGS = $(all_libraries) $(KDE_RPATH)


messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kwin4.pot 

