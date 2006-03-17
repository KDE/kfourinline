kde4_header()
project(kwin4)

add_subdirectory( kwin4 ) 
add_subdirectory( grafix ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES kwin4.desktop )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = kwin4 grafix 
#
#AUTOMAKE_OPTIONS = foreign 
#
#KDE_ICON = kwin4
#
#xdg_apps_DATA = kwin4.desktop
