SUBDIRS = kwin4 grafix 

AUTOMAKE_OPTIONS = foreign 

KDE_ICON = kwin4

appsdir = $(kde_appsdir)/Games/Board
apps_DATA = kwin4.desktop
EXTRA_DIST = $(apps_DATA)

rcdir = $(kde_datadir)/kwin4
rc_DATA = kwin4ui.rc

install-data-local:
	rm -rf $(kde_appsdir)/Games/kwin4.desktop
