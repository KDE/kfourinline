bin_PROGRAMS = kwin4
kwin4_SOURCES = msgdlg.cpp kenddlg.cpp kwin4aboutdlg.cpp KRemoteConnect.cpp KRSocket.cpp KProcessConnect.cpp KMessageEntry.cpp KInteractiveConnect.cpp KInputChildProcess.cpp KEMessage.cpp KEInput.cpp KConnectEntry.cpp KChildConnect.cpp statdlg.cpp networkdlg.cpp namedlg.cpp tabelle.cpp geom.cpp kwin4view.cpp kwin4doc.cpp kwin4.cpp main.cpp 
kwin4_LDADD   = $(LIB_KFILE)

INCLUDES = $(all_includes)

METASOURCES = AUTO

kwin4_LDFLAGS = $(all_libraries) $(KDE_RPATH)


messages:
	$(XGETTEXT) *.cpp -o $(podir)/kwin4.pot 

