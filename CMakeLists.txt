project(kwin4)

add_subdirectory( kwin4 ) 
add_subdirectory( grafix ) 




########### install files ###############

install( FILES kwin4.desktop  DESTINATION  ${XDG_APPS_DIR} )

kde4_install_icons( ${ICON_INSTALL_DIR}   )




#original Makefile.am contents follow:

#SUBDIRS = kwin4 grafix 
#
#AUTOMAKE_OPTIONS = foreign 
#
#KDE_ICON = kwin4
#
#xdg_apps_DATA = kwin4.desktop
