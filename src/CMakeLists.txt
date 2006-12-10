
include_directories( ${CMAKE_SOURCE_DIR}/libkdegames ${CMAKE_SOURCE_DIR}/libkdegames/kgame  )


########### next target ###############

set(kwin4_SRCS 
   main.cpp 
   kwin4.cpp 
   spritenotify.cpp
   thememanager.cpp
   displayintro.cpp 
   displaygame.cpp 
   kwin4view.cpp 
   kwin4doc.cpp 
   kwin4player.cpp 
   introsprite.cpp
   piecesprite.cpp
   pixmapsprite.cpp
   scorewidget.cpp )

kde4_automoc(${kwin4_SRCS})

kde4_add_ui_files(kwin4_SRCS settings.ui statuswidget.ui statistics.ui )

kde4_add_kcfg_files(kwin4_SRCS prefs.kcfgc )

kde4_add_executable(kwin4 ${kwin4_SRCS})

target_link_libraries(kwin4  ${KDE4_KDE3SUPPORT_LIBS} kdegames )

install(TARGETS kwin4  DESTINATION ${BIN_INSTALL_DIR} )


########### next target ###############

set(kwin4proc_SRCS kwin4proc.cpp )

kde4_automoc(${kwin4proc_SRCS})

kde4_add_executable(kwin4proc ${kwin4proc_SRCS})

target_link_libraries(kwin4proc  ${KDE4_KDECORE_LIBS} kdegames )

install(TARGETS kwin4proc  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES kwin4.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kwin4ui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kwin4 )




#original Makefile.am contents follow:

#
#bin_PROGRAMS = kwin4 kwin4proc
#kwin4_SOURCES = main.cpp kwin4.cpp kwin4view.cpp kwin4doc.cpp \
#                kwin4player.cpp kspritecache.cpp \
#	            	scorewidget.cpp prefs.kcfgc settings.ui statistics.ui statuswidget.ui
#
#kwin4_LDADD   = $(LIB_KFILE) $(LIB_KDEGAMES)
#kwin4_DEPENDENCIES = $(LIB_KDEGAMES_DEP)
#
#kwin4proc_SOURCES = kwin4proc.cpp
#kwin4proc_LDADD   = $(LIB_KFILE) $(LIB_KDEGAMES)
#kwin4proc_DEPENDENCIES = $(LIB_KDEGAMES_DEP)
#
#INCLUDES = -I$(top_srcdir)/libkdegames -I$(top_srcdir)/libkdegames/kgame $(all_includes)
#
#METASOURCES = AUTO
#
#rcdir = $(kde_datadir)/kwin4
#rc_DATA = kwin4ui.rc
#
#kwin4_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#kwin4proc_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#
#kde_kcfg_DATA = kwin4.kcfg
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/kwin4.pot 
#
