####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = kproc4
kproc4_SOURCES = kproc4.cpp KChildConnect.cpp KInputChildProcess.cpp KEMessage.cpp KMessageEntry.cpp main.cpp 
kproc4_LDADD   = $(LIB_KFILE)


EXTRA_DIST = main.cpp KMessageEntry.cpp KMessageEntry.h KEMessage.h KEMessage.cpp KInputChildProcess.cpp KInputChildProcess.h KChildConnect.cpp KChildConnect.h KConnectTypes.h kproc4.cpp kproc4.h 

####### kdevelop will overwrite this part!!! (end)############
# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install
# you can add here more. This one gets installed 
bin_PROGRAMS = kproc4 

METASOURCES = AUTO

# the library search path. 
kproc4_LDFLAGS = $(all_libraries) $(KDE_RPATH)
